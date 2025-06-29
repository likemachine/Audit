/* Copyright (c) 2006, 2010, Oracle and/or its affiliates. All rights reserved.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1335  USA */

/**
  @file
  File containing constants that can be used throughout the server.

  @note This file shall not contain or include any declarations of any kinds.
*/

#ifndef SQL_CONST_INCLUDED
#define SQL_CONST_INCLUDED

#include <mysql_version.h>

#define LIBLEN FN_REFLEN-FN_LEN			/* Max l{ngd p} dev */
/* extra 4+4 bytes for slave tmp tables */
#define MAX_DBKEY_LENGTH (NAME_LEN*2+1+1+4+4)
#define MAX_ALIAS_NAME 256
#define MAX_FIELD_NAME (NAME_LEN+1)             /* Max colum name length +1 */
#define MAX_SYS_VAR_LENGTH 32
#define MAX_KEY MAX_INDEXES                     /* Max used keys */
#define MAX_REF_PARTS 32			/* Max parts used as ref */

/*
  Maximum length of the data part of an index lookup key.

  The "data part" is defined as the value itself, not including the
  NULL-indicator bytes or varchar length bytes ("the Extras"). We need this
  value because there was a bug where length of the Extras were not counted.

  You probably need MAX_KEY_LENGTH, not this constant.
*/
#define MAX_DATA_LENGTH_FOR_KEY 3072
#if SIZEOF_OFF_T > 4
#define MAX_REFLENGTH 8				/* Max length for record ref */
#else
#define MAX_REFLENGTH 4				/* Max length for record ref */
#endif
#define MAX_HOSTNAME  (HOSTNAME_LENGTH + 1)	/* len+1 in mysql.user */
#define MAX_CONNECTION_NAME NAME_LEN

#define MAX_MBWIDTH		3		/* Max multibyte sequence */
#define MAX_FILENAME_MBWIDTH    5
#define MAX_FIELD_CHARLENGTH	255
/*
  In MAX_FIELD_VARCHARLENGTH we reserve extra bytes for the overhead:
  - 2 bytes for the length
  - 1 byte for NULL bits
  to avoid the "Row size too large" error for these three corner definitions:
    CREATE TABLE t1 (c VARBINARY(65533));
    CREATE TABLE t1 (c VARBINARY(65534));
    CREATE TABLE t1 (c VARBINARY(65535));
  Like VARCHAR(65536), they will be converted to BLOB automatically
  in non-strict mode.
*/
#define MAX_FIELD_VARCHARLENGTH	(65535-2-1)
#define MAX_FIELD_BLOBLENGTH UINT_MAX32         /* cf field_blob::get_length() */
#define CONVERT_IF_BIGGER_TO_BLOB 512           /* Threshold *in characters*   */

/* Max column width +1 */
#define MAX_FIELD_WIDTH		(MAX_FIELD_CHARLENGTH*MAX_MBWIDTH+1)

#define MAX_BIT_FIELD_LENGTH    64      /* Max length in bits for bit fields */

#define MAX_DATE_WIDTH		10	/* YYYY-MM-DD */
#define MIN_TIME_WIDTH          10      /* -HHH:MM:SS */
#define MAX_TIME_WIDTH          16      /* -DDDDDD HH:MM:SS */
#define MAX_TIME_FULL_WIDTH     23      /* -DDDDDD HH:MM:SS.###### */
#define MAX_DATETIME_FULL_WIDTH 26	/* YYYY-MM-DD HH:MM:SS.###### */
#define MAX_DATETIME_WIDTH	19	/* YYYY-MM-DD HH:MM:SS */
#define MAX_DATETIME_COMPRESSED_WIDTH 14  /* YYYYMMDDHHMMSS */
#define MAX_DATETIME_PRECISION  6

#define MAX_TABLES	(sizeof(table_map)*8-3)	/* Max tables in join */
#define PARAM_TABLE_BIT	(((table_map) 1) << (sizeof(table_map)*8-3))
#define OUTER_REF_TABLE_BIT	(((table_map) 1) << (sizeof(table_map)*8-2))
#define RAND_TABLE_BIT	(((table_map) 1) << (sizeof(table_map)*8-1))
#define PSEUDO_TABLE_BITS (PARAM_TABLE_BIT | OUTER_REF_TABLE_BIT | \
                           RAND_TABLE_BIT)
#define CONNECT_STRING_MAXLEN   65535           /* stored in 2 bytes in .frm */
#define MAX_FIELDS	4096			/* Limit in the .frm file */
#define MAX_PARTITIONS  8192

#define MAX_SELECT_NESTING (SELECT_NESTING_MAP_SIZE - 1)

#define MAX_SORT_MEMORY 2048*1024
#define MIN_SORT_MEMORY 1024

/* Some portable defines */

#define STRING_BUFFER_USUAL_SIZE 80

/* Memory allocated when parsing a statement / saving a statement */
#define MEM_ROOT_BLOCK_SIZE       8192
#define MEM_ROOT_PREALLOC         8192
#define TRANS_MEM_ROOT_BLOCK_SIZE 4096
#define TRANS_MEM_ROOT_PREALLOC   4096

#define DEFAULT_ERROR_COUNT	64
#define EXTRA_RECORDS	10			/* Extra records in sort */
#define SCROLL_EXTRA	5			/* Extra scroll-rows. */
#define FIELD_NAME_USED ((uint) 32768)		/* Bit set if fieldname used */
#define FORM_NAME_USED	((uint) 16384)		/* Bit set if formname used */
#define FIELD_NR_MASK	16383			/* To get fieldnumber */
#define FERR		-1			/* Error from my_functions */
#define CREATE_MODE	0			/* Default mode on new files */
#define NAMES_SEP_CHAR	255			/* Char to sep. names */

/*
   This is used when reading large blocks, sequential read.
   We assume that reading this much will be roughly the same cost as 1
   seek / fetching one row from the storage engine.
   Cost of one read of DISK_CHUNK_SIZE is DISK_SEEK_BASE_COST (ms).
*/
#define DISK_CHUNK_SIZE	(uint) (65536) /* Size of diskbuffer for tmpfiles */

#define FRM_VER_TRUE_VARCHAR (FRM_VER+4) /* 10 */
#define FRM_VER_EXPRESSSIONS (FRM_VER+5) /* 11 */
#define FRM_VER_CURRENT  FRM_VER_EXPRESSSIONS

/***************************************************************************
  Configuration parameters
****************************************************************************/

#define ACL_CACHE_SIZE		256
#define MAX_PASSWORD_LENGTH	32
#define HOST_CACHE_SIZE		128
#define MAX_ACCEPT_RETRY	10	// Test accept this many times
#define MAX_FIELDS_BEFORE_HASH	32
#define USER_VARS_HASH_SIZE     16
#define SEQUENCES_HASH_SIZE     16
#define TABLE_OPEN_CACHE_MIN    200
#define TABLE_OPEN_CACHE_DEFAULT 2000
#define TABLE_DEF_CACHE_DEFAULT 400
/**
  We must have room for at least 400 table definitions in the table
  cache, since otherwise there is no chance prepared
  statements that use these many tables can work.
  Prepared statements use table definition cache ids (table_map_id)
  as table version identifiers. If the table definition
  cache size is less than the number of tables used in a statement,
  the contents of the table definition cache is guaranteed to rotate
  between a prepare and execute. This leads to stable validation
  errors. In future we shall use more stable version identifiers,
  for now the only solution is to ensure that the table definition
  cache can contain at least all tables of a given statement.
*/
#define TABLE_DEF_CACHE_MIN     400

/**
 Maximum number of connections default value.
 151 is larger than Apache's default max children,
 to avoid "too many connections" error in a common setup.
*/
#define MAX_CONNECTIONS_DEFAULT 151

/*
  Stack reservation.
  Feel free to raise this by the smallest amount you can to get the
  "execution_constants" test to pass.
*/
#define STACK_MIN_SIZE          16000   // Abort if less stack during eval.

#define STACK_MIN_SIZE_FOR_OPEN (1024*80)
#define STACK_BUFF_ALLOC        352     ///< For stack overrun checks
#ifndef MYSQLD_NET_RETRY_COUNT
#define MYSQLD_NET_RETRY_COUNT  10	///< Abort read after this many int.
#endif

#define QUERY_ALLOC_BLOCK_SIZE		16384
#define QUERY_ALLOC_PREALLOC_SIZE   	24576
#define TRANS_ALLOC_BLOCK_SIZE		8192
#define TRANS_ALLOC_PREALLOC_SIZE	4096
#define RANGE_ALLOC_BLOCK_SIZE		4096
#define ACL_ALLOC_BLOCK_SIZE		1024
#define UDF_ALLOC_BLOCK_SIZE		1024
#define TABLE_ALLOC_BLOCK_SIZE		1024
#define WARN_ALLOC_BLOCK_SIZE		2048
#define WARN_ALLOC_PREALLOC_SIZE	1024
/*
  Note that if we are using 32K or less, then TCmalloc will use a local
  heap without locks!
*/
#define SHOW_ALLOC_BLOCK_SIZE           (32768-MALLOC_OVERHEAD)

/*
  The following parameters is to decide when to use an extra cache to
  optimise seeks when reading a big table in sorted order
*/
#define MIN_FILE_LENGTH_TO_USE_ROW_CACHE (10L*1024*1024)
#define MIN_ROWS_TO_USE_TABLE_CACHE	 100
#define MIN_ROWS_TO_USE_BULK_INSERT	 100

/*
  The lower bound of accepted rows when using filter.
  This is used to ensure that filters are not too agressive.
*/
#define MIN_ROWS_AFTER_FILTERING 1.0

/**
  Number of rows in a reference table when refered through a not unique key.
  This value is only used when we don't know anything about the key
  distribution.
*/
#define MATCHING_ROWS_IN_OTHER_TABLE 10

#define MY_CHARSET_BIN_MB_MAXLEN 1

/** Don't pack string keys shorter than this (if PACK_KEYS=1 isn't used). */
#define KEY_DEFAULT_PACK_LENGTH 8

/** Characters shown for the command in 'show processlist'. */
#define PROCESS_LIST_WIDTH 100
/* Characters shown for the command in 'information_schema.processlist' */
#define PROCESS_LIST_INFO_WIDTH 65535

#define PRECISION_FOR_DOUBLE 53
#define PRECISION_FOR_FLOAT  24

/* -[digits].E+## */
#define MAX_FLOAT_STR_LENGTH (FLT_DIG + 6)
/* -[digits].E+### */
#define MAX_DOUBLE_STR_LENGTH (DBL_DIG + 7)

/*
  Default time to wait before aborting a new client connection
  that does not respond to "initial server greeting" timely
*/
#define CONNECT_TIMEOUT		10
 /* Wait 5 minutes before removing thread from thread cache */
#define THREAD_CACHE_TIMEOUT	5*60

/* The following can also be changed from the command line */
#define DEFAULT_CONCURRENCY	10
#define DELAYED_LIMIT		100		/**< pause after xxx inserts */
#define DELAYED_QUEUE_SIZE	1000
#define DELAYED_WAIT_TIMEOUT	(5*60)		/**< Wait for delayed insert */
#define MAX_CONNECT_ERRORS	100		///< errors before disabling host

#define LONG_TIMEOUT ((ulong) 3600L*24L*365L)

/**
  Maximum length of time zone name that we support (Time zone name is
  char(64) in db). mysqlbinlog needs it.
*/
#define MAX_TIME_ZONE_NAME_LENGTH       (NAME_LEN + 1)

#define SP_PSI_STATEMENT_INFO_COUNT 19

#endif /* SQL_CONST_INCLUDED */
