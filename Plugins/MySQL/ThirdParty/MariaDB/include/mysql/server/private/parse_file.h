/* -*- C++ -*- */
/* Copyright (c) 2004, 2010, Oracle and/or its affiliates. All rights reserved.

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

#ifndef _PARSE_FILE_H_
#define _PARSE_FILE_H_

#include "sql_string.h"                         // LEX_STRING
#include "sql_alloc.h"                          // Sql_alloc

class THD;

typedef struct st_mem_root MEM_ROOT;

#define PARSE_FILE_TIMESTAMPLENGTH 19

enum file_opt_type {
  FILE_OPTIONS_STRING,		/**< String (LEX_STRING) */
  FILE_OPTIONS_ESTRING,		/**< Escaped string (LEX_STRING) */
  FILE_OPTIONS_ULONGLONG,	/**< ulonglong parameter (ulonglong) */
  FILE_OPTIONS_VIEW_ALGO,	/**< Similar to longlong, but needs conversion */
  FILE_OPTIONS_TIMESTAMP,	/**< timestamp (LEX_STRING have to be
				   allocated with length 20 (19+1) */
  FILE_OPTIONS_STRLIST,         /**< list of escaped strings
                                   (List<LEX_STRING>) */
  FILE_OPTIONS_ULLLIST          /**< list of ulonglong values
                                   (List<ulonglong>) */
};

struct File_option
{
  LEX_CSTRING name;             /**< Name of the option */
  my_ptrdiff_t offset;          /**< offset to base address of value */
  file_opt_type type;           /**< Option type */
};


/**
  This hook used to catch no longer supported keys and process them for
  backward compatibility.
*/

class Unknown_key_hook
{
public:
  Unknown_key_hook() = default;                       /* Remove gcc warning */
  virtual ~Unknown_key_hook() = default;              /* Remove gcc warning */
  virtual bool process_unknown_string(const char *&unknown_key, uchar* base,
                                      MEM_ROOT *mem_root, const char *end)= 0;
};


/** Dummy hook for parsers which do not need hook for unknown keys. */

class File_parser_dummy_hook: public Unknown_key_hook
{
public:
  File_parser_dummy_hook() = default;                 /* Remove gcc warning */
  virtual bool process_unknown_string(const char *&unknown_key, uchar* base,
                                      MEM_ROOT *mem_root, const char *end);
};

extern File_parser_dummy_hook file_parser_dummy_hook;

bool get_file_options_ulllist(const char *&ptr, const char *end,
                              const char *line, uchar* base,
                              File_option *parameter,
                              MEM_ROOT *mem_root);

const char *
parse_escaped_string(const char *ptr, const char *end, MEM_ROOT *mem_root,
                     LEX_CSTRING *str);

class File_parser;
File_parser *sql_parse_prepare(const LEX_CSTRING *file_name,
			       MEM_ROOT *mem_root, bool bad_format_errors);

my_bool
sql_create_definition_file(const LEX_CSTRING *dir,
                           const LEX_CSTRING *file_name,
			   const LEX_CSTRING *type,
			   uchar* base, File_option *parameters);
my_bool rename_in_schema_file(THD *thd,
                              const char *schema, const char *old_name,
                              const char *new_db, const char *new_name);

int sql_backup_definition_file(const LEX_CSTRING *org_name,
                                   LEX_CSTRING *new_name);
int sql_restore_definition_file(const LEX_CSTRING *name);

class File_parser: public Sql_alloc
{
  char *start, *end;
  LEX_CSTRING file_type;
  bool content_ok;
public:
  File_parser() :start(0), end(0), content_ok(0)
    { file_type.str= 0; file_type.length= 0; }

  bool ok() { return content_ok; }
  const LEX_CSTRING *type() const { return &file_type; }
  my_bool parse(uchar* base, MEM_ROOT *mem_root,
		struct File_option *parameters, uint required,
                Unknown_key_hook *hook) const;

  friend File_parser *sql_parse_prepare(const LEX_CSTRING *file_name,
					MEM_ROOT *mem_root,
					bool bad_format_errors);
};
#endif /* _PARSE_FILE_H_ */
