/* Copyright (c) 2001, 2011, Oracle and/or its affiliates.
   Copyright (c) 2009, 2020, MariaDB Corporation.

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

#ifndef _my_bitmap_h_
#define _my_bitmap_h_

#define MY_BIT_NONE (~(uint) 0)

#include <m_string.h>
#include <my_pthread.h>

typedef uint32 my_bitmap_map;

typedef struct st_bitmap
{
  my_bitmap_map *bitmap;
  my_bitmap_map *last_word_ptr;
  my_bitmap_map last_word_mask;
  uint32	n_bits; /* number of bits occupied by the above */
} MY_BITMAP;

#ifdef	__cplusplus
extern "C" {
#endif

/* Reset memory. Faster then doing a full bzero */
#define my_bitmap_clear(A) ((A)->bitmap= 0)

extern void create_last_word_mask(MY_BITMAP *map);
extern my_bool my_bitmap_init(MY_BITMAP *map, my_bitmap_map *buf, uint n_bits);
extern my_bool bitmap_is_clear_all(const MY_BITMAP *map);
extern my_bool bitmap_is_prefix(const MY_BITMAP *map, uint prefix_size);
extern my_bool bitmap_is_set_all(const MY_BITMAP *map);
extern my_bool bitmap_is_subset(const MY_BITMAP *map1, const MY_BITMAP *map2);
extern my_bool bitmap_is_overlapping(const MY_BITMAP *map1,
                                     const MY_BITMAP *map2);
extern my_bool bitmap_test_and_set(MY_BITMAP *map, uint bitmap_bit);
extern my_bool bitmap_test_and_clear(MY_BITMAP *map, uint bitmap_bit);
extern my_bool bitmap_fast_test_and_set(MY_BITMAP *map, uint bitmap_bit);
extern my_bool bitmap_fast_test_and_clear(MY_BITMAP *map, uint bitmap_bit);
extern my_bool bitmap_union_is_set_all(const MY_BITMAP *map1,
                                       const MY_BITMAP *map2);
extern my_bool bitmap_exists_intersection(const MY_BITMAP **bitmap_array,
                                          uint bitmap_count,
                                          uint start_bit, uint end_bit);

extern uint bitmap_set_next(MY_BITMAP *map);
extern uint bitmap_get_first(const MY_BITMAP *map);
extern uint bitmap_get_first_set(const MY_BITMAP *map);
extern uint bitmap_bits_set(const MY_BITMAP *map);
extern uint bitmap_get_next_set(const MY_BITMAP *map, uint bitmap_bit);
extern void my_bitmap_free(MY_BITMAP *map);
extern void bitmap_set_above(MY_BITMAP *map, uint from_byte, uint use_bit);
extern void bitmap_set_prefix(MY_BITMAP *map, uint prefix_size);
extern void bitmap_intersect(MY_BITMAP *map, const MY_BITMAP *map2);
extern void bitmap_subtract(MY_BITMAP *map, const MY_BITMAP *map2);
extern void bitmap_union(MY_BITMAP *map, const MY_BITMAP *map2);
extern void bitmap_xor(MY_BITMAP *map, const MY_BITMAP *map2);
extern void bitmap_invert(MY_BITMAP *map);
extern void bitmap_copy(MY_BITMAP *map, const MY_BITMAP *map2);

/* Fast, not thread safe, bitmap functions */
#define bitmap_buffer_size(bits) (((bits)+31)/32)*4
#define no_bytes_in_map(map) (((map)->n_bits + 7)/8)
#define no_words_in_map(map) (((map)->n_bits + 31)/32)
#define bytes_word_aligned(bytes) (4*((bytes + 3)/4))
/* The following functions must be compatible with create_last_word_mask()! */
static inline void
bitmap_set_bit(MY_BITMAP *map,uint bit)
{
  uchar *b= (uchar*) map->bitmap + bit / 8;
  DBUG_ASSERT(bit < map->n_bits);
  *b= (uchar) (*b | 1U << (bit & 7));
}
static inline void
bitmap_flip_bit(MY_BITMAP *map,uint bit)
{
  uchar *b= (uchar*) map->bitmap + bit / 8;
  DBUG_ASSERT(bit < map->n_bits);
  *b= (uchar) (*b ^ 1U << (bit & 7));
}
static inline void
bitmap_clear_bit(MY_BITMAP *map,uint bit)
{
  uchar *b= (uchar*) map->bitmap + bit / 8;
  DBUG_ASSERT(bit < map->n_bits);
  *b= (uchar) (*b & ~(1U << (bit & 7)));
}
static inline uint
bitmap_is_set(const MY_BITMAP *map,uint bit)
{
  const uchar *b= (const uchar*) map->bitmap + bit / 8;
  DBUG_ASSERT(bit < map->n_bits);
  return !!(*b & (1U << (bit & 7)));
}

static inline my_bool bitmap_cmp(const MY_BITMAP *map1, const MY_BITMAP *map2)
{
  if (memcmp(map1->bitmap, map2->bitmap, 4*(no_words_in_map(map1)-1)) != 0)
    return FALSE;
  return ((*map1->last_word_ptr | map1->last_word_mask) ==
          (*map2->last_word_ptr | map2->last_word_mask));
}

#define bitmap_clear_all(MAP) \
  { memset((MAP)->bitmap, 0, 4*no_words_in_map((MAP))); }
#define bitmap_set_all(MAP) \
  (memset((MAP)->bitmap, 0xFF, 4*no_words_in_map((MAP))))

#ifdef	__cplusplus
}
#endif

#endif /* _my_bitmap_h_ */
