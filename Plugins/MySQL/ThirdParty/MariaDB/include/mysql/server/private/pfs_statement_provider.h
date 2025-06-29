/* Copyright (c) 2012, 2015, Oracle and/or its affiliates. All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  51 Franklin Street, Suite 500, Boston, MA 02110-1335 USA */

#ifndef PFS_STATEMENT_PROVIDER_H
#define PFS_STATEMENT_PROVIDER_H

/**
  @file include/pfs_statement_provider.h
  Performance schema instrumentation (declarations).
*/

#ifdef HAVE_PSI_STATEMENT_INTERFACE
#ifdef MYSQL_SERVER
#ifndef EMBEDDED_LIBRARY
#ifndef MYSQL_DYNAMIC_PLUGIN

#include "mysql/psi/psi.h"

#define PSI_STATEMENT_CALL(M) pfs_ ## M ## _v1
#define PSI_DIGEST_CALL(M) pfs_ ## M ## _v1

C_MODE_START

void pfs_register_statement_v1(const char *category,
                               PSI_statement_info_v1 *info,
                               int count);

PSI_statement_locker*
pfs_get_thread_statement_locker_v1(PSI_statement_locker_state *state,
                                   PSI_statement_key key,
                                   const void *charset,
                                   PSI_sp_share *sp_share);

PSI_statement_locker*
pfs_refine_statement_v1(PSI_statement_locker *locker,
                        PSI_statement_key key);

void pfs_start_statement_v1(PSI_statement_locker *locker,
                            const char *db, uint db_len,
                            const char *src_file, uint src_line);

void pfs_set_statement_text_v1(PSI_statement_locker *locker,
                               const char *text, uint text_len);

void pfs_set_statement_lock_time_v1(PSI_statement_locker *locker,
                                    ulonglong count);

void pfs_set_statement_rows_sent_v1(PSI_statement_locker *locker,
                                    ulonglong count);

void pfs_set_statement_rows_examined_v1(PSI_statement_locker *locker,
                                        ulonglong count);

void pfs_inc_statement_created_tmp_disk_tables_v1(PSI_statement_locker *locker,
                                                  ulong count);

void pfs_inc_statement_created_tmp_tables_v1(PSI_statement_locker *locker,
                                             ulong count);

void pfs_inc_statement_select_full_join_v1(PSI_statement_locker *locker,
                                           ulong count);

void pfs_inc_statement_select_full_range_join_v1(PSI_statement_locker *locker,
                                                 ulong count);

void pfs_inc_statement_select_range_v1(PSI_statement_locker *locker,
                                       ulong count);

void pfs_inc_statement_select_range_check_v1(PSI_statement_locker *locker,
                                             ulong count);

void pfs_inc_statement_select_scan_v1(PSI_statement_locker *locker,
                                      ulong count);

void pfs_inc_statement_sort_merge_passes_v1(PSI_statement_locker *locker,
                                            ulong count);

void pfs_inc_statement_sort_range_v1(PSI_statement_locker *locker,
                                     ulong count);

void pfs_inc_statement_sort_rows_v1(PSI_statement_locker *locker,
                                    ulong count);

void pfs_inc_statement_sort_scan_v1(PSI_statement_locker *locker,
                                    ulong count);

void pfs_set_statement_no_index_used_v1(PSI_statement_locker *locker);

void pfs_set_statement_no_good_index_used_v1(PSI_statement_locker *locker);

void pfs_end_statement_v1(PSI_statement_locker *locker, void *stmt_da);

PSI_digest_locker *pfs_digest_start_v1(PSI_statement_locker *locker);

void pfs_digest_end_v1(PSI_digest_locker *locker,
                       const sql_digest_storage *digest);

C_MODE_END

#endif /* MYSQL_DYNAMIC_PLUGIN */
#endif /* EMBEDDED_LIBRARY */
#endif /* MYSQL_SERVER */
#endif /* HAVE_PSI_STATEMENT_INTERFACE */

#endif

