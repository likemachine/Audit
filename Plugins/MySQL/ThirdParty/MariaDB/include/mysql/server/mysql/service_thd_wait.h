/* Copyright (c) 2010, 2011, Oracle and/or its affiliates. All rights reserved.

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

#ifndef MYSQL_SERVICE_THD_WAIT_INCLUDED
#define MYSQL_SERVICE_THD_WAIT_INCLUDED

/**
  @file include/mysql/service_thd_wait.h
  This service provides functions for plugins and storage engines to report
  when they are going to sleep/stall.
  
  SYNOPSIS
  thd_wait_begin() - call just before a wait begins
  thd                     Thread object
                          Use NULL if the thd is NOT known.
  wait_type               Type of wait
                          1 -- short wait (e.g. for mutex)
                          2 -- medium wait (e.g. for disk io)
                          3 -- large wait (e.g. for locked row/table)
  NOTES
    This is used by the threadpool to have better knowledge of which
    threads that currently are actively running on CPUs. When a thread
    reports that it's going to sleep/stall, the threadpool scheduler is
    free to start another thread in the pool most likely. The expected wait
    time is simply an indication of how long the wait is expected to
    become, the real wait time could be very different.

  thd_wait_end() called immediately after the wait is complete

  thd_wait_end() MUST be called if thd_wait_begin() was called.

  Using thd_wait_...() service is optional but recommended.  Using it will
  improve performance as the thread pool will be more active at managing the
  thread workload.
*/

#ifdef __cplusplus
extern "C" {
#endif

/*
  One should only report wait events that could potentially block for a
  long time. A mutex wait is too short of an event to report. The reason
  is that an event which is reported leads to a new thread starts
  executing a query and this has a negative impact of usage of CPU caches
  and thus the expected gain of starting a new thread must be higher than
  the expected cost of lost performance due to starting a new thread.

  Good examples of events that should be reported are waiting for row locks
  that could easily be for many milliseconds or even seconds and the same
  holds true for global read locks, table locks and other meta data locks.
  Another event of interest is going to sleep for an extended time.
*/
typedef enum _thd_wait_type_e {
  THD_WAIT_SLEEP= 1,
  THD_WAIT_DISKIO= 2,
  THD_WAIT_ROW_LOCK= 3,
  THD_WAIT_GLOBAL_LOCK= 4,
  THD_WAIT_META_DATA_LOCK= 5,
  THD_WAIT_TABLE_LOCK= 6,
  THD_WAIT_USER_LOCK= 7,
  THD_WAIT_BINLOG= 8,
  THD_WAIT_GROUP_COMMIT= 9,
  THD_WAIT_SYNC= 10,
  THD_WAIT_NET= 11,
  THD_WAIT_LAST= 12
} thd_wait_type;

extern struct thd_wait_service_st {
  void (*thd_wait_begin_func)(MYSQL_THD, int);
  void (*thd_wait_end_func)(MYSQL_THD);
} *thd_wait_service;

#ifdef MYSQL_DYNAMIC_PLUGIN

#define thd_wait_begin(_THD, _WAIT_TYPE) \
  thd_wait_service->thd_wait_begin_func(_THD, _WAIT_TYPE)
#define thd_wait_end(_THD) thd_wait_service->thd_wait_end_func(_THD)

#else

void thd_wait_begin(MYSQL_THD thd, int wait_type);
void thd_wait_end(MYSQL_THD thd);

#endif

#ifdef __cplusplus
}
#endif

#endif

