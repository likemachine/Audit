#ifndef SQL_VIEW_INCLUDED
#define SQL_VIEW_INCLUDED

/* -*- C++ -*- */
/* Copyright (c) 2004, 2010, Oracle and/or its affiliates.
   Copyright (c) 2015, MariaDB

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1335  USA
*/

#include "sql_class.h"     /* Required by sql_lex.h */
#include "sql_lex.h"       /* enum_view_create_mode, enum_drop_mode */

/* Forward declarations */

class File_parser;


/* Function declarations */

bool create_view_precheck(THD *thd, TABLE_LIST *tables, TABLE_LIST *view,
                          enum_view_create_mode mode);

bool mysql_create_view(THD *thd, TABLE_LIST *view,
                       enum_view_create_mode mode);

bool mysql_make_view(THD *thd, TABLE_SHARE *share, TABLE_LIST *table,
                     bool open_view_no_parse);


bool mysql_drop_view(THD *thd, TABLE_LIST *view, enum_drop_mode drop_mode);

bool check_key_in_view(THD *thd, TABLE_LIST * view);

bool insert_view_fields(THD *thd, List<Item> *list, TABLE_LIST *view);

int view_checksum(THD *thd, TABLE_LIST *view);
int view_check(THD *thd, TABLE_LIST *view, HA_CHECK_OPT *check_opt);
int view_repair(THD *thd, TABLE_LIST *view, HA_CHECK_OPT *check_opt);

extern TYPELIB updatable_views_with_limit_typelib;

bool check_duplicate_names(THD *thd, List<Item>& item_list,
                           bool gen_unique_view_names);
bool mysql_rename_view(THD *thd, const LEX_CSTRING *new_db,
                       const LEX_CSTRING *new_name,
                       const LEX_CSTRING *old_db,
                       const LEX_CSTRING *old_name);

void make_valid_column_names(THD *thd, List<Item> &item_list);

#define VIEW_ANY_ACL (SELECT_ACL | UPDATE_ACL | INSERT_ACL | DELETE_ACL)

extern const LEX_CSTRING view_type;

void make_valid_column_names(List<Item> &item_list);

bool mariadb_view_version_get(TABLE_SHARE *share);

#endif /* SQL_VIEW_INCLUDED */
