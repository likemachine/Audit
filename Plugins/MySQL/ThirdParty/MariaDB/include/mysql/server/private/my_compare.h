/* Copyright (c) 2011, Oracle and/or its affiliates.
   Copyright (c) 1991, 2021, MariaDB Corporation.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1335  USA */

#ifndef _my_compare_h
#define _my_compare_h

#include "myisampack.h"
#ifdef	__cplusplus
extern "C" {
#endif

#include "m_ctype.h"                            /* CHARSET_INFO */

/*
  There is a hard limit for the maximum number of keys as there are only
  8 bits in the index file header for the number of keys in a table.
  This means that 0..255 keys can exist for a table. The idea of
  HA_MAX_POSSIBLE_KEY is to ensure that one can use myisamchk & tools on
  a MyISAM table for which one has more keys than MyISAM is normally
  compiled for. If you don't have this, you will get a core dump when
  running myisamchk compiled for 128 keys on a table with 255 keys.
*/

#define HA_MAX_POSSIBLE_KEY         255         /* For myisamchk */
/*
  The following defines can be increased if necessary.
  But beware the dependency of MI_MAX_POSSIBLE_KEY_BUFF and HA_MAX_KEY_LENGTH.
*/

#define HA_MAX_KEY_LENGTH           1000        /* Max length in bytes */
#define HA_MAX_KEY_SEG              32          /* Max segments for key */

#define HA_MAX_POSSIBLE_KEY_BUFF    (HA_MAX_KEY_LENGTH + 24+ 6+6)
#define HA_MAX_KEY_BUFF  (HA_MAX_KEY_LENGTH+HA_MAX_KEY_SEG*6+8+8)

typedef struct st_HA_KEYSEG		/* Key-portion */
{
  CHARSET_INFO *charset;
  uint32 start;				/* Start of key in record */
  uint32 null_pos;			/* position to NULL indicator */
  uint16 bit_pos;                       /* Position to bit part */
  uint16 flag;
  uint16 length;			/* Keylength */
  uint16 language;
  uint8  type;				/* Type of key (for sort) */
  uint8  null_bit;			/* bitmask to test for NULL */
  uint8  bit_start;
  uint8  bit_length;                    /* Length of bit part */
} HA_KEYSEG;

#define get_key_length(length,key) \
{ if (*(const uchar*) (key) != 255) \
    length= (uint) *(const uchar*) ((key)++); \
  else \
  { length= mi_uint2korr((key)+1); (key)+=3; } \
}

#define get_key_length_rdonly(length,key) \
{ if (*(const uchar*) (key) != 255) \
    length= ((uint) *(const uchar*) ((key))); \
  else \
  { length= mi_uint2korr((key)+1); } \
}

#define get_key_pack_length(length,length_pack,key) \
{ if (*(const uchar*) (key) != 255) \
  { length= (uint) *(const uchar*) ((key)++); length_pack= 1; }\
  else \
  { length=mi_uint2korr((key)+1); (key)+= 3; length_pack= 3; } \
}

#define store_key_length_inc(key,length) \
{ if ((length) < 255) \
  { *(key)++= (uchar)(length); } \
  else \
  { *(key)=255; mi_int2store((key)+1,(length)); (key)+=3; } \
}

#define size_to_store_key_length(length) ((length) < 255 ? 1 : 3)

static inline uchar get_rec_bits(const uchar *ptr, uchar ofs, uint len)
{
  uint16 val= ptr[0];
  if (ofs + len > 8)
    val|= (uint16)(((uint) ptr[1]) << 8);
  return (uchar) ((val >> ofs) & ((1 << len) - 1));
}

static inline void set_rec_bits(uint16 bits, uchar *ptr, uchar ofs, uint len)
{
  ptr[0]= (uchar) ((ptr[0] & ~(((1 << len) - 1) << ofs)) | (bits << ofs));
  if (ofs + len > 8)
    ptr[1]= (uchar) ((ptr[1] & ~((1 << (len - 8 + ofs)) - 1)) |
                     bits >> (8 - ofs));
}

#define clr_rec_bits(bit_ptr, bit_ofs, bit_len) \
  set_rec_bits(0, bit_ptr, bit_ofs, bit_len)

extern int ha_compare_text(CHARSET_INFO *, const uchar *, size_t,
                           const uchar *, size_t , my_bool);
extern int ha_key_cmp(HA_KEYSEG *keyseg, const uchar *a,
		      const uchar *b, uint key_length, uint nextflag,
		      uint *diff_pos);
extern HA_KEYSEG *ha_find_null(HA_KEYSEG *keyseg, const uchar *a);

/*
  Inside an in-memory data record, memory pointers to pieces of the
  record (like BLOBs) are stored in their native byte order and in
  this amount of bytes.
*/
#define portable_sizeof_char_ptr 8
#ifdef	__cplusplus
}
#endif

/**
  Return values for pushed index condition or rowid filter check functions.

  0=CHECK_NEG  - The filter is not satisfied. The engine should discard this
                 index tuple and continue the scan.
  1=CHECK_POS  - The filter is satisfied. Current index tuple should be
                 returned to the SQL layer.
  2=CHECK_OUT_OF_RANGE - the index tuple is outside of the range that we're
                 scanning. (Example: if we're scanning "t.key BETWEEN 10 AND
                 20" and got a "t.key=21" tuple) Tthe engine should stop
                 scanning and return HA_ERR_END_OF_FILE right away).
  3=CHECK_ABORTED_BY_USER - the engine must stop scanning and should return
                            HA_ERR_ABORTED_BY_USER right away
 -1=CHECK_ERROR - Reserved for internal errors in engines. Should not be
                  returned by ICP or rowid filter check functions.
*/

typedef enum check_result {
  CHECK_ERROR=-1,
  CHECK_NEG=0,
  CHECK_POS=1,
  CHECK_OUT_OF_RANGE=2,
  CHECK_ABORTED_BY_USER=3
} check_result_t;

typedef check_result_t (*index_cond_func_t)(void *param);
typedef check_result_t (*rowid_filter_func_t)(void *param);

#endif /* _my_compare_h */
