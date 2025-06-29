/* Copyright (c) 2000, 2010, Oracle and/or its affiliates. 
   Copyright (C) 2000, 2019, MariaDB Corporation.

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

#ifndef _my_dbug_h
#define _my_dbug_h

#ifndef _WIN32
#include <signal.h>
#endif

#ifdef  __cplusplus
extern "C" {
#endif
#if !defined(DBUG_OFF) && !defined(_lint)

struct _db_stack_frame_ {
  const char *func;      /* function name of the previous stack frame       */
  const char *file;      /* filename of the function of previous frame      */
  uint level;            /* this nesting level, highest bit enables tracing */
  int line;              /* line of DBUG_RETURN                             */
  struct _db_stack_frame_ *prev; /* pointer to the previous frame */
};

struct  _db_code_state_;
extern  my_bool _dbug_on_;
extern  my_bool _db_keyword_(struct _db_code_state_ *, const char *, int);
extern  int _db_explain_(struct _db_code_state_ *cs, char *buf, size_t len);
extern  int _db_explain_init_(char *buf, size_t len);
extern	int _db_is_pushed_(void);
extern  void _db_setjmp_(void);
extern  void _db_longjmp_(void);
extern  void _db_process_(const char *name);
extern  void _db_push_(const char *control);
extern  void _db_pop_(void);
extern  void _db_set_(const char *control);
extern  void _db_set_init_(const char *control);
extern void _db_enter_(const char *_func_, const char *_file_, uint _line_,
                       struct _db_stack_frame_ *_stack_frame_);
extern  void _db_return_(struct _db_stack_frame_ *_stack_frame_);
extern  int _db_pargs_(uint _line_,const char *keyword);
extern  void _db_doprnt_(const char *format,...)
#ifdef WAITING_FOR_BUGFIX_TO_VSPRINTF
  ATTRIBUTE_FORMAT(printf, 1, 2)
#endif
  ;
extern  void _db_dump_(uint _line_,const char *keyword,
                       const unsigned char *memory, size_t length);
extern  void _db_end_(void);
extern  void _db_lock_file_(void);
extern  void _db_unlock_file_(void);
ATTRIBUTE_COLD
extern  my_bool _db_my_assert(const char *file, int line, const char *msg);
extern  FILE *_db_fp_(void);
extern void _db_flush_(void);
extern void dbug_swap_code_state(void **code_state_store);
extern void dbug_free_code_state(void **code_state_store);
extern  const char* _db_get_func_(void);
extern int (*dbug_sanity)(void);

#ifdef DBUG_TRACE
#define DBUG_LEAVE do { \
    _db_stack_frame_.line= __LINE__; \
    _db_return_ (&_db_stack_frame_); \
    _db_stack_frame_.line= 0; \
  } while(0)

#define DBUG_PRINT(keyword,arglist) \
        do if (_db_pargs_(__LINE__,keyword)) _db_doprnt_ arglist; while(0)

#ifdef HAVE_ATTRIBUTE_CLEANUP
#define DBUG_ENTER(a) struct _db_stack_frame_ _db_stack_frame_  __attribute__((cleanup(_db_return_))); \
        _db_enter_ (a,__FILE__,__LINE__,&_db_stack_frame_)
#define DBUG_RETURN(a1) do { _db_stack_frame_.line=__LINE__; return(a1);} while(0)
#define DBUG_VOID_RETURN do { _db_stack_frame_.line=__LINE__; return;} while(0)
#else
#define DBUG_ENTER(a) struct _db_stack_frame_ _db_stack_frame_; \
        _db_enter_ (a,__FILE__,__LINE__,&_db_stack_frame_)
#define DBUG_RETURN(a1) do {DBUG_LEAVE; return(a1);} while(0)
#define DBUG_VOID_RETURN do {DBUG_LEAVE; return;} while(0)
#endif

#else
#define DBUG_LEAVE
#define DBUG_ENTER(a)
#define DBUG_RETURN(a1) return(a1)
#define DBUG_VOID_RETURN return
#define DBUG_PRINT(keyword,arglist) do{} while(0)
#endif

#define DBUG_EXECUTE(keyword,a1) \
        do {if (_db_keyword_(0, (keyword), 0)) { a1 }} while(0)
#define DBUG_EXECUTE_IF(keyword,a1) \
        do {if (_db_keyword_(0, (keyword), 1)) { a1 }} while(0)

#define DBUG_IF(keyword) _db_keyword_(0, (keyword), 1)

#define DBUG_PUSH_EMPTY if (_dbug_on_) { DBUG_PUSH(""); }
#define DBUG_POP_EMPTY  if (_dbug_on_) { DBUG_POP(); }
#define DBUG_PUSH(a1) _db_push_ (a1)
#define DBUG_POP() _db_pop_ ()
#define DBUG_SET(a1) _db_set_ (a1)
#define DBUG_SET_INITIAL(a1) _db_set_init_ (a1)
#define DBUG_PROCESS(a1) _db_process_(a1)
#define DBUG_FILE _db_fp_()
#define DBUG_DUMP(keyword,a1,a2) _db_dump_(__LINE__,keyword,a1,a2)
#define DBUG_END()  _db_end_ ()
#define DBUG_LOCK_FILE _db_lock_file_()
#define DBUG_UNLOCK_FILE _db_unlock_file_()
#define DBUG_ASSERT(A) do { \
  if (unlikely(!(A)) && _db_my_assert(__FILE__, __LINE__, #A)) assert(A); \
} while (0)
#define DBUG_SLOW_ASSERT(A) DBUG_ASSERT(A)
#define DBUG_ASSERT_EXISTS
#define DBUG_EXPLAIN(buf,len) _db_explain_(0, (buf),(len))
#define DBUG_EXPLAIN_INITIAL(buf,len) _db_explain_init_((buf),(len))
#define DEBUGGER_OFF                    do { _dbug_on_= 0; } while(0)
#define DEBUGGER_ON                     do { _dbug_on_= 1; } while(0)
#define IF_DBUG(A,B)                    A
#define IF_DBUG_ASSERT(A,B)             A
#define DBUG_SWAP_CODE_STATE(arg) dbug_swap_code_state(arg)
#define DBUG_FREE_CODE_STATE(arg) dbug_free_code_state(arg)
#undef DBUG_ASSERT_AS_PRINTF

#ifndef _WIN32
#define DBUG_ABORT()                    (_db_flush_(), abort())
#else
/*
  Avoid popup with abort/retry/ignore buttons. When BUG#31745 is fixed we can
  call abort() instead of _exit(3) (now it would cause a "test signal" popup).
*/
#include <crtdbg.h>
#define DBUG_ABORT() (_db_flush_(),\
                     (void)_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE),\
                     (void)_CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR),\
                     TerminateProcess(GetCurrentProcess(),3))
#endif

/*
  Make the program fail, without creating a core file.
  abort() will send SIGABRT which (most likely) generates core.
  Use SIGKILL instead, which cannot be caught.
  We also pause the current thread, until the signal is actually delivered.
  An alternative would be to use _exit(EXIT_FAILURE),
  but then valgrind would report lots of memory leaks.
 */
#ifdef _WIN32
#define DBUG_SUICIDE() DBUG_ABORT()
#else
extern void _db_suicide_(void);
#define DBUG_SUICIDE() (_db_flush_(), _db_suicide_())
#endif /* _WIN32 */

#else                                           /* No debugger */

#define DBUG_ENTER(a1)
#define DBUG_VIOLATION_HELPER_LEAVE do { } while(0)
#define DBUG_LEAVE
#define DBUG_RETURN(a1)                 do { return(a1); } while(0)
#define DBUG_VOID_RETURN                do { return; } while(0)
#define DBUG_PRINT(keyword, arglist)    do { } while(0)
#define DBUG_EXECUTE(keyword,a1)        do { } while(0)
#define DBUG_EXECUTE_IF(keyword,a1)     do { } while(0)
#define DBUG_IF(keyword) 0
#define DBUG_PRINT(keyword,arglist)     do { } while(0)
#define DBUG_PUSH_EMPTY                 do { } while(0)
#define DBUG_POP_EMPTY                  do { } while(0)
#define DBUG_PUSH(a1)                   do { } while(0)
#define DBUG_SET(a1)                    do { } while(0)
#define DBUG_SET_INITIAL(a1)            do { } while(0)
#define DBUG_POP()                      do { } while(0)
#define DBUG_PROCESS(a1)                do { } while(0)
#define DBUG_DUMP(keyword,a1,a2)        do { } while(0)
#define DBUG_END()                      do { } while(0)
#define DBUG_SLOW_ASSERT(A)             do { } while(0)
#define DBUG_LOCK_FILE                  do { } while(0)
#define DBUG_FILE (stderr)
#define DBUG_UNLOCK_FILE                do { } while(0)
#define DBUG_EXPLAIN(buf,len)
#define DBUG_EXPLAIN_INITIAL(buf,len)
#define DEBUGGER_OFF                    do { } while(0)
#define DEBUGGER_ON                     do { } while(0)
#define IF_DBUG(A,B)                    B
#define DBUG_SWAP_CODE_STATE(arg)       do { } while(0)
#define DBUG_FREE_CODE_STATE(arg)       do { } while(0)
#define DBUG_ABORT()                    do { } while(0)
#define DBUG_CRASH_ENTER(func)
#define DBUG_CRASH_RETURN(val)          do { return(val); } while(0)
#define DBUG_CRASH_VOID_RETURN          do { return; } while(0)
#define DBUG_SUICIDE()                  ((void) 0)

#ifdef DBUG_ASSERT_AS_PRINTF
extern void (*my_dbug_assert_failed)(const char *assert_expr, const char* file, unsigned long line);
#define DBUG_ASSERT(assert_expr) do { if (!(assert_expr)) { my_dbug_assert_failed(#assert_expr, __FILE__, __LINE__); }} while (0)
#define DBUG_ASSERT_EXISTS
#define IF_DBUG_ASSERT(A,B)             A
#else
#define DBUG_ASSERT(A)                  do { } while(0)
#define IF_DBUG_ASSERT(A,B)             B
#endif /* DBUG_ASSERT_AS_PRINTF */
#endif /* !defined(DBUG_OFF) && !defined(_lint) */

#ifdef EXTRA_DEBUG
/**
  Sync points allow us to force the server to reach a certain line of code
  and block there until the client tells the server it is ok to go on.
  The client tells the server to block with SELECT GET_LOCK()
  and unblocks it with SELECT RELEASE_LOCK(). Used for debugging difficult
  concurrency problems
*/
#define DBUG_SYNC_POINT(lock_name,lock_timeout) \
 debug_sync_point(lock_name,lock_timeout)
void debug_sync_point(const char* lock_name, uint lock_timeout);
#else
#define DBUG_SYNC_POINT(lock_name,lock_timeout)
#endif /* EXTRA_DEBUG */

#ifdef __cplusplus
}
/*
  DBUG_LOG() was initially intended for InnoDB. To be able to use it elsewhere
  one should #include <sstream>. We intentionally avoid including it here to save
  compilation time.
*/
# ifdef DBUG_OFF
#  define DBUG_LOG(keyword, v) do {} while (0)
# else
#  define DBUG_LOG(keyword, v) do { \
  if (_db_pargs_(__LINE__, keyword)) { \
    std::ostringstream _db_s; _db_s << v; \
    _db_doprnt_("%s", _db_s.str().c_str()); \
  }} while (0)
# endif
#endif

#endif /* _my_dbug_h */
