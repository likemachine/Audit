/* Copyright (C) 2006 MySQL AB
   Copyright (c) 2017, MariaDB Corporation.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335 USA */

/* Definitions needed for myisamchk/mariachk.c */

#ifndef _myisamchk_h
#define _myisamchk_h

#include <my_stack_alloc.h>

/*
  Flags used by xxxxchk.c or/and ha_xxxx.cc that are NOT passed
  to xxxcheck.c follows:
*/

#define TT_USEFRM               1U
#define TT_FOR_UPGRADE          2U
#define TT_FROM_MYSQL           4U

/* Bits set in out_flag */
#define O_NEW_DATA	2U
#define O_DATA_LOST	4U

/* 
  MARIA/MYISAM supports several statistics collection
  methods. Currently statistics collection method is not stored in
  MARIA file and has to be specified for each table analyze/repair
  operation in MI_CHECK::stats_method.
*/

typedef enum
{
  /* Treat NULLs as inequal when collecting statistics (default for 4.1/5.0) */
  MI_STATS_METHOD_NULLS_NOT_EQUAL,
  /* Treat NULLs as equal when collecting statistics (like 4.0 did) */
  MI_STATS_METHOD_NULLS_EQUAL,
  /* Ignore NULLs - count only tuples without NULLs in the index components */
  MI_STATS_METHOD_IGNORE_NULLS
} enum_handler_stats_method;

struct st_myisam_info;

typedef struct st_handler_check_param
{
  char *isam_file_name;
  MY_TMPDIR *tmpdir;
  void *thd;
  const char *db_name, *table_name, *op_name;
  ulonglong auto_increment_value;
  ulonglong max_data_file_length;
  ulonglong keys_in_use;
  ulonglong max_record_length;
  /* 
     The next two are used to collect statistics, see update_key_parts for
     description.
  */
  ulonglong unique_count[HA_MAX_KEY_SEG + 1];
  ulonglong notnull_count[HA_MAX_KEY_SEG + 1];
  ulonglong max_allowed_lsn;
  my_off_t search_after_block;
  my_off_t new_file_pos, key_file_blocks;
  my_off_t keydata, totaldata, key_blocks, start_check_pos;
  my_off_t used, empty, splits, del_length, link_used, lost;
  ha_rows total_records, total_deleted, records,del_blocks;
  ha_rows full_page_count, tail_count;
  ha_checksum record_checksum, glob_crc;
  ha_checksum key_crc[HA_MAX_POSSIBLE_KEY];
  ha_checksum tmp_key_crc[HA_MAX_POSSIBLE_KEY];
  ha_checksum tmp_record_checksum;
  ulonglong   org_key_map;
  ulonglong   testflag;

  /* Following is used to check if rows are visible */
  ulonglong max_trid, max_found_trid;
  ulonglong not_visible_rows_found;
  ulonglong sort_buffer_length, orig_sort_buffer_length;
  ulonglong use_buffers;                        /* Used as param to getopt() */
  size_t read_buffer_length, write_buffer_length, sort_key_blocks;
  time_t backup_time;                           /* To sign backup files */
  ulong rec_per_key_part[HA_MAX_KEY_SEG * HA_MAX_POSSIBLE_KEY];
  double new_rec_per_key_part[HA_MAX_KEY_SEG * HA_MAX_POSSIBLE_KEY];
  uint out_flag, error_printed, verbose;
  uint opt_sort_key, total_files, max_level;
  uint key_cache_block_size, pagecache_block_size;
  uint skip_lsn_error_count;
  int tmpfile_createflag, err_count;
  myf myf_rw;
  uint16 language;
  my_bool warning_printed, note_printed, wrong_trd_printed;
  my_bool using_global_keycache, opt_lock_memory, opt_follow_links;
  my_bool retry_repair, force_sort, calc_checksum, static_row_size;
  char temp_filename[FN_REFLEN];
  IO_CACHE read_cache;
  void **stack_end_ptr;
  enum_handler_stats_method stats_method;
  /* For reporting progress */
  uint stage, max_stage;
  uint progress_counter;             /* How often to call _report_progress() */
  ulonglong progress, max_progress;

  int (*fix_record)(struct st_myisam_info *info, uchar *record, int keynum);

  mysql_mutex_t print_msg_mutex;
  my_bool need_print_msg_lock;
  myf malloc_flags;
} HA_CHECK;


typedef struct st_buffpek {
  my_off_t file_pos;                    /* Where we are in the sort file */
  uchar *base, *key;                    /* Key pointers */
  ha_rows count;                        /* Number of rows in table */
  ha_rows mem_count;                    /* Numbers of keys in memory */
  ha_rows max_keys;                     /* Max keys in buffert */
} BUFFPEK;

#endif /* _myisamchk_h */
