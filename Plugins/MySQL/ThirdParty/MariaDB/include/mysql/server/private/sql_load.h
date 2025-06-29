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

#ifndef SQL_LOAD_INCLUDED
#define SQL_LOAD_INCLUDED

#include "sql_list.h"                           /* List */

class Item;

#include "sql_class.h"                          /* enum_duplicates */

class sql_exchange;

int mysql_load(THD *thd, const sql_exchange *ex, TABLE_LIST *table_list,
	        List<Item> &fields_vars, List<Item> &set_fields,
                List<Item> &set_values_list,
                enum enum_duplicates handle_duplicates, bool ignore,
                bool local_file);


#endif /* SQL_LOAD_INCLUDED */
