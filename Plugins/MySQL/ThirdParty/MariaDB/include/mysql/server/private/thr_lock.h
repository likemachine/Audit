/* Copyright (c) 2000, 2010, Oracle and/or its affiliates. All rights reserved.
   Copyright (c) 2017, MariaDB Corporation.

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

/* For use with thr_lock:s */

#ifndef _thr_lock_h
#define _thr_lock_h
#ifdef	__cplusplus
extern "C" {
#endif
#include <my_pthread.h>
#include <my_list.h>

struct st_thr_lock;
extern ulong locks_immediate,locks_waited ;

/*
  Important: if a new lock type is added, a matching lock description
             must be added to sql_test.cc's lock_descriptions array.
*/
enum thr_lock_type { TL_IGNORE=-1,
		     TL_UNLOCK,			/* UNLOCK ANY LOCK */
                     /*
                       Parser only! At open_tables() becomes TL_READ or
                       TL_READ_NO_INSERT depending on the binary log format
                       (SBR/RBR) and on the table category (log table).
                       Used for tables that are read by statements which
                       modify tables.
                     */
                     TL_READ_DEFAULT,
		     TL_READ,			/* Read lock */
		     TL_READ_WITH_SHARED_LOCKS,
		     /* High prior. than TL_WRITE. Allow concurrent insert */
		     TL_READ_HIGH_PRIORITY,
		     /* READ, Don't allow concurrent insert */
		     TL_READ_NO_INSERT,
		     /* READ, but skip locks if found */
		     TL_READ_SKIP_LOCKED,
		     /* 
			Write lock, but allow other threads to read / write.
			Used by BDB tables in MySQL to mark that someone is
			reading/writing to the table.
		      */
		     TL_WRITE_ALLOW_WRITE,
		     /*
		       WRITE lock used by concurrent insert. Will allow
		       READ, if one could use concurrent insert on table.
		     */
		     TL_WRITE_CONCURRENT_INSERT,
		     /* Write used by INSERT DELAYED.  Allows READ locks */
		     TL_WRITE_DELAYED,
                     /* 
                       parser only! Late bound low_priority flag. 
                       At open_tables() becomes thd->update_lock_default.
                     */
                     TL_WRITE_DEFAULT,
		     /* WRITE lock that has lower priority than TL_READ */
		     TL_WRITE_LOW_PRIORITY,
		     /* WRITE, but skip locks if found */
		     TL_WRITE_SKIP_LOCKED,
		     /* Normal WRITE lock */
		     TL_WRITE,
		     /* Abort new lock request with an error */
		     TL_WRITE_ONLY};

/*
  TL_FIRST_WRITE is here to impose some consistency in the sql
  layer on determining read/write transactions and to
  provide some API compatibility if additional transactions
  are added. Above or equal to TL_FIRST_WRITE is a write transaction
  while < TL_FIRST_WRITE is a read transaction.
*/
#define TL_FIRST_WRITE TL_WRITE_ALLOW_WRITE

enum enum_thr_lock_result { THR_LOCK_SUCCESS= 0, THR_LOCK_ABORTED= 1,
                            THR_LOCK_WAIT_TIMEOUT= 2, THR_LOCK_DEADLOCK= 3 };


/* Priority for locks */
#define THR_LOCK_LATE_PRIV  1U	/* For locks to be merged with org lock */
#define THR_LOCK_MERGE_PRIV 2U	/* For merge tables */

#define THR_UNLOCK_UPDATE_STATUS 1U

extern ulong max_write_lock_count;
extern my_bool thr_lock_inited;
extern enum thr_lock_type thr_upgraded_concurrent_insert_lock;

/*
  A description of the thread which owns the lock. The address
  of an instance of this structure is used to uniquely identify the thread.
*/

typedef struct st_thr_lock_info
{
  pthread_t thread;
  my_thread_id thread_id;
  void *mysql_thd;        // THD pointer
} THR_LOCK_INFO;


typedef struct st_thr_lock_data {
  THR_LOCK_INFO *owner;
  struct st_thr_lock_data *next,**prev;
  struct st_thr_lock *lock;
  mysql_cond_t *cond;
  void *status_param;			/* Param to status functions */
  void *debug_print_param;              /* For error messages */
  struct PSI_table *m_psi;
  enum thr_lock_type type;
  enum thr_lock_type org_type;		/* Cache for MariaDB */
  uint priority;
} THR_LOCK_DATA;

struct st_lock_list {
  THR_LOCK_DATA *data,**last;
};

typedef struct st_thr_lock {
  LIST list;
  mysql_mutex_t mutex;
  struct st_lock_list read_wait;
  struct st_lock_list read;
  struct st_lock_list write_wait;
  struct st_lock_list write;
  /* write_lock_count is incremented for write locks and reset on read locks */
  ulong write_lock_count;
  uint read_no_write_count;
  my_bool (*get_status)(void*, my_bool);/* Called when one gets a lock */
  void (*copy_status)(void*,void*);
  void (*update_status)(void*);		/* Before release of write */
  void (*restore_status)(void*);        /* Before release of read */
  my_bool (*start_trans)(void*);	/* When all locks are taken */
  my_bool (*check_status)(void *);
  void   (*fix_status)(void *, void *);/* For thr_merge_locks() */
  const char *name;                     /* Used for error reporting */
  my_bool allow_multiple_concurrent_insert;
} THR_LOCK;


extern LIST *thr_lock_thread_list;
extern mysql_mutex_t THR_LOCK_lock;
struct st_my_thread_var;

my_bool init_thr_lock(void);		/* Must be called once/thread */
void thr_lock_info_init(THR_LOCK_INFO *info, struct st_my_thread_var *tmp);
void thr_lock_init(THR_LOCK *lock);
void thr_lock_delete(THR_LOCK *lock);
void thr_lock_data_init(THR_LOCK *lock,THR_LOCK_DATA *data,
			void *status_param);
void thr_unlock(THR_LOCK_DATA *data, uint unlock_flags);
enum enum_thr_lock_result thr_multi_lock(THR_LOCK_DATA **data,
                                         uint count, THR_LOCK_INFO *owner,
                                         ulong lock_wait_timeout);
void thr_multi_unlock(THR_LOCK_DATA **data,uint count, uint unlock_flags);
void thr_merge_locks(THR_LOCK_DATA **data, uint org_count, uint new_count);
void thr_abort_locks(THR_LOCK *lock, my_bool upgrade_lock);
my_bool thr_abort_locks_for_thread(THR_LOCK *lock, my_thread_id thread);
void thr_print_locks(void);		/* For debugging */
my_bool thr_upgrade_write_delay_lock(THR_LOCK_DATA *data,
                                     enum thr_lock_type new_lock_type,
                                     ulong lock_wait_timeout);
void    thr_downgrade_write_lock(THR_LOCK_DATA *data,
                                 enum thr_lock_type new_lock_type);
my_bool thr_reschedule_write_lock(THR_LOCK_DATA *data,
                                  ulong lock_wait_timeout);
void thr_set_lock_wait_callback(void (*before_wait)(void),
                                void (*after_wait)(void));

#ifdef WITH_WSREP
  typedef my_bool (* wsrep_thd_is_brute_force_fun)(const MYSQL_THD, my_bool);
  typedef my_bool(* wsrep_abort_thd_fun)(MYSQL_THD, MYSQL_THD, my_bool);
  typedef my_bool (* wsrep_on_fun)(const MYSQL_THD);
  void wsrep_thr_lock_init(
    wsrep_thd_is_brute_force_fun bf_fun, wsrep_abort_thd_fun abort_fun,
    my_bool debug, my_bool convert_LOCK_to_trx, wsrep_on_fun on_fun);
#endif

#ifdef	__cplusplus
}
#endif
#endif /* _thr_lock_h */
