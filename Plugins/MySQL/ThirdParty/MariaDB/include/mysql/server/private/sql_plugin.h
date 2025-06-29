/* Copyright (c) 2005, 2012, Oracle and/or its affiliates.
   Copyright (c) 2009, 2017, MariaDB Corporation.

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

#ifndef _sql_plugin_h
#define _sql_plugin_h

/*
  the following #define adds server-only members to enum_mysql_show_type,
  that is defined in plugin.h
*/
#define SHOW_always_last SHOW_KEY_CACHE_LONG, \
            SHOW_HAVE, SHOW_MY_BOOL, SHOW_HA_ROWS, SHOW_SYS, \
            SHOW_LONG_NOFLUSH, SHOW_LEX_STRING, SHOW_ATOMIC_COUNTER_UINT32_T, \
      /* SHOW_*_STATUS must be at the end, SHOW_LONG_STATUS being first */ \
            SHOW_LONG_STATUS, SHOW_DOUBLE_STATUS, SHOW_LONGLONG_STATUS, \
            SHOW_UINT32_STATUS
#include "mariadb.h"
#undef SHOW_always_last

#include "m_string.h"                       /* LEX_STRING */
#include "my_alloc.h"                       /* MEM_ROOT */

class sys_var;
enum SHOW_COMP_OPTION { SHOW_OPTION_YES, SHOW_OPTION_NO, SHOW_OPTION_DISABLED};
enum enum_plugin_load_option { PLUGIN_OFF, PLUGIN_ON, PLUGIN_FORCE,
  PLUGIN_FORCE_PLUS_PERMANENT };
extern const char *global_plugin_typelib_names[];

extern volatile int global_plugin_version;
extern ulong dlopen_count;

#include <my_sys.h>
#include "sql_list.h"

#ifdef DBUG_OFF
#define plugin_ref_to_int(A) A
#define plugin_int_to_ref(A) A
#else
#define plugin_ref_to_int(A) (A ? A[0] : NULL)
#define plugin_int_to_ref(A) &(A)
#endif

/*
  the following flags are valid for plugin_init()
*/
#define PLUGIN_INIT_SKIP_PLUGIN_TABLE    1U
#define PLUGIN_INIT_SKIP_INITIALIZATION  2U

#define INITIAL_LEX_PLUGIN_LIST_SIZE    16

typedef enum enum_mysql_show_type SHOW_TYPE;
typedef struct st_mysql_show_var SHOW_VAR;

#define MYSQL_ANY_PLUGIN         -1

/*
  different values of st_plugin_int::state
  though they look like a bitmap, plugin may only
  be in one of those eigenstates, not in a superposition of them :)
  It's a bitmap, because it makes it easier to test
  "whether the state is one of those..."
*/
#define PLUGIN_IS_FREED         1U
#define PLUGIN_IS_DELETED       2U
#define PLUGIN_IS_UNINITIALIZED 4U
#define PLUGIN_IS_READY         8U
#define PLUGIN_IS_DYING         16U
#define PLUGIN_IS_DISABLED      32U

struct st_ptr_backup {
  void **ptr;
  void *value;
  void save(void **p) { ptr= p; value= *p; }
  void save(const char **p) { save((void**)p); }
  void restore() { *ptr= value; }
};

/* A handle for the dynamic library containing a plugin or plugins. */

struct st_plugin_dl
{
  LEX_CSTRING dl;
  void *handle;
  struct st_maria_plugin *plugins;
  st_ptr_backup *ptr_backup;
  uint nbackups;
  uint ref_count;            /* number of plugins loaded from the library */
  int mysqlversion;
  int mariaversion;
  bool   allocated;
};

/* A handle of a plugin */

struct st_plugin_int
{
  LEX_CSTRING name;
  struct st_maria_plugin *plugin;
  struct st_plugin_dl *plugin_dl;
  st_ptr_backup *ptr_backup;
  uint nbackups;
  uint state;
  uint ref_count;               /* number of threads using the plugin */
  uint locks_total;             /* how many times the plugin was locked */
  void *data;                   /* plugin type specific, e.g. handlerton */
  MEM_ROOT mem_root;            /* memory for dynamic plugin structures */
  sys_var *system_vars;         /* server variables for this plugin */
  enum enum_plugin_load_option load_option; /* OFF, ON, FORCE, F+PERMANENT */
};


extern mysql_mutex_t LOCK_plugin;

/*
  See intern_plugin_lock() for the explanation for the
  conditionally defined plugin_ref type
*/
#ifdef DBUG_OFF
typedef struct st_plugin_int *plugin_ref;
#define plugin_ref_to_int(A) A
#define plugin_int_to_ref(A) A
#define plugin_decl(pi) ((pi)->plugin)
#define plugin_dlib(pi) ((pi)->plugin_dl)
#define plugin_data(pi,cast) ((cast)((pi)->data))
#define plugin_name(pi) (&((pi)->name))
#define plugin_state(pi) ((pi)->state)
#define plugin_load_option(pi) ((pi)->load_option)
#define plugin_equals(p1,p2) ((p1) == (p2))
#else
typedef struct st_plugin_int **plugin_ref;
#define plugin_ref_to_int(A) (A ? A[0] : NULL)
#define plugin_int_to_ref(A) &(A)
#define plugin_decl(pi) ((pi)[0]->plugin)
#define plugin_dlib(pi) ((pi)[0]->plugin_dl)
#define plugin_data(pi,cast) ((cast)((pi)[0]->data))
#define plugin_name(pi) (&((pi)[0]->name))
#define plugin_state(pi) ((pi)[0]->state)
#define plugin_load_option(pi) ((pi)[0]->load_option)
#define plugin_equals(p1,p2) ((p1) && (p2) && (p1)[0] == (p2)[0])
#endif

typedef int (*plugin_type_init)(struct st_plugin_int *);

extern I_List<i_string> *opt_plugin_load_list_ptr;
extern char *opt_plugin_dir_ptr;
extern MYSQL_PLUGIN_IMPORT char opt_plugin_dir[FN_REFLEN];
extern const LEX_CSTRING plugin_type_names[];
extern ulong plugin_maturity;
extern TYPELIB plugin_maturity_values;
extern const char *plugin_maturity_names[];

extern int plugin_init(int *argc, char **argv, int init_flags);
extern void plugin_shutdown(void);
void add_plugin_options(DYNAMIC_ARRAY *options, MEM_ROOT *mem_root);
extern bool plugin_is_ready(const LEX_CSTRING *name, int type);
#define my_plugin_lock_by_name(A,B,C) plugin_lock_by_name(A,B,C)
#define my_plugin_lock(A,B) plugin_lock(A,B)
extern plugin_ref plugin_lock(THD *thd, plugin_ref ptr);
extern plugin_ref plugin_lock_by_name(THD *thd, const LEX_CSTRING *name,
                                      int type);
extern void plugin_unlock(THD *thd, plugin_ref plugin);
extern void plugin_unlock_list(THD *thd, plugin_ref *list, size_t count);
extern bool mysql_install_plugin(THD *thd, const LEX_CSTRING *name,
                                 const LEX_CSTRING *dl);
extern bool mysql_uninstall_plugin(THD *thd, const LEX_CSTRING *name,
                                   const LEX_CSTRING *dl);
extern bool plugin_register_builtin(struct st_mysql_plugin *plugin);
extern void plugin_thdvar_init(THD *thd);
extern void plugin_thdvar_cleanup(THD *thd);
sys_var *find_plugin_sysvar(st_plugin_int *plugin, st_mysql_sys_var *var);
void plugin_opt_set_limits(struct my_option *, const struct st_mysql_sys_var *);
extern SHOW_COMP_OPTION plugin_status(const char *name, size_t len, int type);
extern bool check_valid_path(const char *path, size_t length);
extern void plugin_mutex_init();

typedef my_bool (plugin_foreach_func)(THD *thd,
                                      plugin_ref plugin,
                                      void *arg);
#define plugin_foreach(A,B,C,D) plugin_foreach_with_mask(A,B,C,PLUGIN_IS_READY,D)
extern bool plugin_foreach_with_mask(THD *thd, plugin_foreach_func *func,
                                     int type, uint state_mask, void *arg);
extern void sync_dynamic_session_variables(THD* thd, bool global_lock);

extern bool plugin_dl_foreach(THD *thd, const LEX_CSTRING *dl,
                              plugin_foreach_func *func, void *arg);

extern void sync_dynamic_session_variables(THD* thd, bool global_lock);
#endif

#ifdef WITH_WSREP
extern void wsrep_plugins_pre_init();
extern void wsrep_plugins_post_init();
#endif /* WITH_WSREP */
