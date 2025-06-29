/* Copyright (c) 2000, 2016, Oracle and/or its affiliates.
   Copyright (c) 2009, 2016, MariaDB

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1335  USA */

#ifndef SLAVE_H
#define SLAVE_H

/**
  MASTER_DELAY can be at most (1 << 31) - 1.
*/
#define MASTER_DELAY_MAX (0x7FFFFFFF)
#if INT_MAX < 0x7FFFFFFF
#error "don't support platforms where INT_MAX < 0x7FFFFFFF"
#endif

/**
  @defgroup Replication Replication
  @{

  @file
*/

/** 
   Some of defines are need in parser even though replication is not 
   compiled in (embedded).
*/

/**
   The maximum is defined as (ULONG_MAX/1000) with 4 bytes ulong
*/
#define SLAVE_MAX_HEARTBEAT_PERIOD 4294967

#ifdef HAVE_REPLICATION

#include "log.h"
#include "my_list.h"
#include "rpl_filter.h"
#include "rpl_tblmap.h"
#include "rpl_gtid.h"
#include "log_event.h"

#define SLAVE_NET_TIMEOUT  60

#define MAX_SLAVE_ERROR    ER_ERROR_LAST+1

#define MAX_REPLICATION_THREAD 64

// Forward declarations
class Relay_log_info;
class Master_info;
class Master_info_index;
struct rpl_group_info;
struct rpl_parallel_thread;

int init_intvar_from_file(int* var, IO_CACHE* f, int default_val);
int init_strvar_from_file(char *var, int max_size, IO_CACHE *f,
                          const char *default_val);
int init_floatvar_from_file(float* var, IO_CACHE* f, float default_val);
int init_dynarray_intvar_from_file(DYNAMIC_ARRAY* arr, IO_CACHE* f);

/*****************************************************************************

  MySQL Replication

  Replication is implemented via two types of threads:

    I/O Thread - One of these threads is started for each master server.
                 They maintain a connection to their master server, read log
                 events from the master as they arrive, and queues them into
                 a single, shared relay log file.  A Master_info 
                 represents each of these threads.

    SQL Thread - One of these threads is started and reads from the relay log
                 file, executing each event.  A Relay_log_info 
                 represents this thread.

  Buffering in the relay log file makes it unnecessary to reread events from
  a master server across a slave restart.  It also decouples the slave from
  the master where long-running updates and event logging are concerned--ie
  it can continue to log new events while a slow query executes on the slave.

*****************************************************************************/

/*
  MUTEXES in replication:

  LOCK_active_mi: [note: this was originally meant for multimaster, to switch
  from a master to another, to protect active_mi] It is used to SERIALIZE ALL
  administrative commands of replication: START SLAVE, STOP SLAVE, CHANGE
  MASTER, RESET SLAVE, end_slave() (when mysqld stops) [init_slave() does not
  need it it's called early]. Any of these commands holds the mutex from the
  start till the end. This thus protects us against a handful of deadlocks
  (consider start_slave_thread() which, when starting the I/O thread, releases
  mi->run_lock, keeps rli->run_lock, and tries to re-acquire mi->run_lock).

  Currently active_mi never moves (it's created at startup and deleted at
  shutdown, and not changed: it always points to the same Master_info struct),
  because we don't have multimaster. So for the moment, mi does not move, and
  mi->rli does not either.

  In Master_info: run_lock, data_lock
  run_lock protects all information about the run state: slave_running, thd
  and the existence of the I/O thread (to stop/start it, you need this mutex).
  data_lock protects some moving members of the struct: counters (log name,
  position) and relay log (MYSQL_BIN_LOG object).

  In Relay_log_info: run_lock, data_lock
  see Master_info
  However, note that run_lock does not protect
  Relay_log_info.run_state; that is protected by data_lock.
  
  Order of acquisition: if you want to have LOCK_active_mi and a run_lock, you
  must acquire LOCK_active_mi first.

  In MYSQL_BIN_LOG: LOCK_log, LOCK_index of the binlog and the relay log
  LOCK_log: when you write to it. LOCK_index: when you create/delete a binlog
  (so that you have to update the .index file).
*/

extern ulong master_retry_count;
extern MY_BITMAP slave_error_mask;
extern char slave_skip_error_names[];
extern bool use_slave_mask;
extern char slave_transaction_retry_error_names[];
extern uint *slave_transaction_retry_errors;
extern uint slave_transaction_retry_error_length;
extern char *slave_load_tmpdir;
extern char *master_info_file;
extern MYSQL_PLUGIN_IMPORT char *relay_log_info_file;
extern char *opt_relay_logname, *opt_relaylog_index_name;
extern my_bool opt_skip_slave_start, opt_reckless_slave;
extern my_bool opt_log_slave_updates;
extern char *opt_slave_skip_errors;
extern char *opt_slave_transaction_retry_errors;
extern my_bool opt_replicate_annotate_row_events;
extern ulonglong relay_log_space_limit;
extern ulonglong opt_read_binlog_speed_limit;
extern ulonglong slave_skipped_errors;
extern const char *relay_log_index;
extern const char *relay_log_basename;

/*
  4 possible values for Master_info::slave_running and
  Relay_log_info::slave_running.
  The values 0,1,2,3 are very important: to keep the diff small, I didn't
  substitute places where we use 0/1 with the newly defined symbols.
  So don't change these values.
  The same way, code is assuming that in Relay_log_info we use only values
  0/1.
  I started with using an enum, but
  enum_variable=1; is not legal so would have required many line changes.
*/
#define MYSQL_SLAVE_NOT_RUN         0
#define MYSQL_SLAVE_RUN_NOT_CONNECT 1
#define MYSQL_SLAVE_RUN_CONNECT     2
#define MYSQL_SLAVE_RUN_READING     3

#define RPL_LOG_NAME (rli->group_master_log_name[0] ? rli->group_master_log_name :\
 "FIRST")
#define IO_RPL_LOG_NAME (mi->master_log_name[0] ? mi->master_log_name :\
 "FIRST")

/*
  If the following is set, if first gives an error, second will be
  tried. Otherwise, if first fails, we fail.
*/
#define SLAVE_FORCE_ALL 4

/*
  Values for the option --replicate-events-marked-for-skip.
  Must match the names in replicate_events_marked_for_skip_names in sys_vars.cc
*/
#define RPL_SKIP_REPLICATE 0
#define RPL_SKIP_FILTER_ON_SLAVE 1
#define RPL_SKIP_FILTER_ON_MASTER 2


int init_slave();
int init_recovery(Master_info* mi, const char** errmsg);
bool init_slave_skip_errors(const char* arg);
bool init_slave_transaction_retry_errors(const char* arg);
int register_slave_on_master(MYSQL* mysql);
int terminate_slave_threads(Master_info* mi, int thread_mask,
			     bool skip_lock = 0);
int start_slave_threads(THD *thd,
                        bool need_slave_mutex, bool wait_for_start,
			Master_info* mi, const char* master_info_fname,
			const char* slave_info_fname, int thread_mask);
/*
  cond_lock is usually same as start_lock. It is needed for the case when
  start_lock is 0 which happens if start_slave_thread() is called already
  inside the start_lock section, but at the same time we want a
  mysql_cond_wait() on start_cond, start_lock
*/
int start_slave_thread(
#ifdef HAVE_PSI_INTERFACE
                       PSI_thread_key thread_key,
#endif
                       pthread_handler h_func,
                       mysql_mutex_t *start_lock,
                       mysql_mutex_t *cond_lock,
                       mysql_cond_t *start_cond,
                       volatile uint *slave_running,
                       volatile ulong *slave_run_id,
                       Master_info *mi);

/* If fd is -1, dump to NET */
int mysql_table_dump(THD* thd, const char* db,
		     const char* tbl_name, int fd = -1);

/* retrieve table from master and copy to slave*/
int fetch_master_table(THD* thd, const char* db_name, const char* table_name,
		       Master_info* mi, MYSQL* mysql, bool overwrite);

void show_master_info_get_fields(THD *thd, List<Item> *field_list,
                                     bool full, size_t gtid_pos_length);
bool show_master_info(THD* thd, Master_info* mi, bool full);
bool show_all_master_info(THD* thd);
void show_binlog_info_get_fields(THD *thd, List<Item> *field_list);
bool show_binlog_info(THD* thd);
bool rpl_master_has_bug(const Relay_log_info *rli, uint bug_id, bool report,
                        bool (*pred)(const void *), const void *param,
                        bool maria_master= false);
bool rpl_master_erroneous_autoinc(THD* thd);

const char *print_slave_db_safe(const char *db);
void skip_load_data_infile(NET* net);

void slave_prepare_for_shutdown();
void end_slave(); /* release slave threads */
void close_active_mi(); /* clean up slave threads data */
void clear_until_condition(Relay_log_info* rli);
void clear_slave_error(Relay_log_info* rli);
void end_relay_log_info(Relay_log_info* rli);
void init_thread_mask(int* mask,Master_info* mi,bool inverse);
Format_description_log_event *
read_relay_log_description_event(IO_CACHE *cur_log, ulonglong start_pos,
                                 const char **errmsg);

int init_relay_log_pos(Relay_log_info* rli,const char* log,ulonglong pos,
		       bool need_data_lock, const char** errmsg,
                       bool look_for_description_event);

int purge_relay_logs(Relay_log_info* rli, THD *thd, bool just_reset,
		     const char** errmsg);
void set_slave_thread_options(THD* thd);
void set_slave_thread_default_charset(THD *thd, rpl_group_info *rgi);
int rotate_relay_log(Master_info* mi);
int has_temporary_error(THD *thd);
int sql_delay_event(Log_event *ev, THD *thd, rpl_group_info *rgi);
int apply_event_and_update_pos(Log_event* ev, THD* thd,
                               struct rpl_group_info *rgi);
int apply_event_and_update_pos_for_parallel(Log_event* ev, THD* thd,
                                            struct rpl_group_info *rgi);

int init_intvar_from_file(int* var, IO_CACHE* f, int default_val);
int init_floatvar_from_file(float* var, IO_CACHE* f, float default_val);
int init_strvar_from_file(char *var, int max_size, IO_CACHE *f,
                          const char *default_val);
int init_dynarray_intvar_from_file(DYNAMIC_ARRAY* arr, IO_CACHE* f);

pthread_handler_t handle_slave_io(void *arg);
void slave_output_error_info(rpl_group_info *rgi, THD *thd);
pthread_handler_t handle_slave_sql(void *arg);
bool net_request_file(NET* net, const char* fname);
void slave_background_kill_request(THD *to_kill);
void slave_background_gtid_pos_create_request
        (rpl_slave_state::gtid_pos_table *table_entry);
void slave_background_gtid_pending_delete_request(void);

extern Master_info *active_mi; /* active_mi for multi-master */
extern Master_info *default_master_info; /* To replace active_mi */
extern Master_info_index *master_info_index;
extern LEX_CSTRING default_master_connection_name;
extern my_bool replicate_same_server_id;

extern int disconnect_slave_event_count, abort_slave_event_count ;

/* the master variables are defaults read from my.cnf or command line */
extern uint report_port;
extern char *master_info_file, *report_user;
extern char *report_host, *report_password;

extern I_List<THD> threads;

/*
  Check that a binlog event (read from the relay log) is valid to update
  last_master_timestamp. That is, a valid event is one with a consistent
  timestamp which originated from a primary server.
*/
static inline bool event_can_update_last_master_timestamp(Log_event *ev)
{
  return ev && !(ev->is_artificial_event() || ev->is_relay_log_event() ||
                 (ev->when == 0));
}

#else
#define close_active_mi() /* no-op */
#endif /* HAVE_REPLICATION */

/* masks for start/stop operations on io and sql slave threads */
#define SLAVE_IO  1
#define SLAVE_SQL 2

/**
  @} (end of group Replication)
*/

#endif
