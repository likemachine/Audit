/* Copyright (c) 2011, 2017, Oracle and/or its affiliates.
   Copyright (c) 2011, 2017, MariaDB

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

#ifndef _welcome_copyright_notice_h_
#define _welcome_copyright_notice_h_

#define COPYRIGHT_NOTICE_CURRENT_YEAR "2018"

/*
  This define specifies copyright notice which is displayed by every MySQL
  program on start, or on help screen.
*/
#define ORACLE_WELCOME_COPYRIGHT_NOTICE(first_year) \
  "Copyright (c) " first_year ", " COPYRIGHT_NOTICE_CURRENT_YEAR \
   ", Oracle, MariaDB Corporation Ab and others.\n"

#ifdef VER
static inline void print_version()
{
  /* NOTE mysql.cc is not using this function! */
  printf("%s from %s, client %s for %s (%s)\n",
         my_progname, MYSQL_SERVER_VERSION, VER, SYSTEM_TYPE, MACHINE_TYPE);
}
#endif
#endif /* _welcome_copyright_notice_h_ */
