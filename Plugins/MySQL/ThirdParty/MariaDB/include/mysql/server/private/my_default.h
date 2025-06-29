/* Copyright (C) 2013 Monty Program Ab

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335 USA */

/* Definitions for mysys/my_default.c */

#ifndef MY_DEFAULT_INCLUDED
#define MY_DEFAULT_INCLUDED

C_MODE_START

extern MYSQL_PLUGIN_IMPORT const char *my_defaults_extra_file;
extern const char *my_defaults_group_suffix;
extern MYSQL_PLUGIN_IMPORT const char *my_defaults_file;
extern my_bool my_defaults_mark_files;

extern int get_defaults_options(char **argv);
extern int my_load_defaults(const char *conf_file, const char **groups,
                            int *argc, char ***argv, const char ***);
extern int load_defaults(const char *conf_file, const char **groups,
                         int *argc, char ***argv);
extern void free_defaults(char **argv);
extern void my_print_default_files(const char *conf_file);
extern void print_defaults(const char *conf_file, const char **groups);


/** Simplify load_defaults() common use */
#define load_defaults_or_exit(A, B, C, D) switch (load_defaults(A, B, C, D)) { \
                                          case 0: break; \
                                          case 4: my_end(0); exit(0); \
                                          default: my_end(0); exit(1); }

C_MODE_END

#endif /* MY_DEFAULT_INCLUDED */
