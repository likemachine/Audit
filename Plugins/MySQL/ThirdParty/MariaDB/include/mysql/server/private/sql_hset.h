#ifndef SQL_HSET_INCLUDED
#define SQL_HSET_INCLUDED
/* Copyright (c) 2010, Oracle and/or its affiliates. All rights reserved.

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

#include "my_global.h"
#include "hash.h"


/**
  A type-safe wrapper around mysys HASH.
*/

template <typename T>
class Hash_set
{
public:
  enum { START_SIZE= 8 };
  /**
    Constructs an empty unique hash.
  */
  Hash_set(PSI_memory_key psi_key, uchar *(*K)(const T *, size_t *, my_bool),
           CHARSET_INFO *cs= &my_charset_bin)
  {
    my_hash_init(psi_key, &m_hash, cs, START_SIZE, 0, 0, (my_hash_get_key)K, 0,
                 HASH_UNIQUE);
  }

  Hash_set(PSI_memory_key psi_key, CHARSET_INFO *charset, ulong default_array_elements,
           size_t key_offset, size_t key_length, my_hash_get_key get_key,
           void (*free_element)(void*), uint flags)
  {
    my_hash_init(psi_key, &m_hash, charset, default_array_elements, key_offset,
                 key_length, get_key, free_element, flags);
  }
  /**
    Destroy the hash by freeing the buckets table. Does
    not call destructors for the elements.
  */
  ~Hash_set()
  {
    my_hash_free(&m_hash);
  }
  /**
    Insert a single value into a hash. Does not tell whether
    the value was inserted -- if an identical value existed,
    it is not replaced.

    @retval TRUE  Out of memory.
    @retval FALSE OK. The value either was inserted or existed
                  in the hash.
  */
  bool insert(T *value)
  {
    return my_hash_insert(&m_hash, reinterpret_cast<const uchar*>(value));
  }
  bool remove(T *value)
  {
    return my_hash_delete(&m_hash, reinterpret_cast<uchar*>(value));
  }
  T *find(const void *key, size_t klen) const
  {
    return (T*)my_hash_search(&m_hash, reinterpret_cast<const uchar *>(key), klen);
  }
  /** Is this hash set empty? */
  bool is_empty() const { return m_hash.records == 0; }
  /** Returns the number of unique elements. */
  size_t size() const { return static_cast<size_t>(m_hash.records); }
  /** Erases all elements from the container */
  void clear() { my_hash_reset(&m_hash); }
  const T* at(size_t i) const
  {
    return reinterpret_cast<T*>(my_hash_element(const_cast<HASH*>(&m_hash), i));
  }
  /** An iterator over hash elements. Is not insert-stable. */
  class Iterator
  {
  public:
    Iterator(Hash_set &hash_set)
      : m_hash(&hash_set.m_hash),
        m_idx(0)
    {}
    /**
      Return the current element and reposition the iterator to the next
      element.
    */
    inline T *operator++(int)
    {
      if (m_idx < m_hash->records)
        return reinterpret_cast<T*>(my_hash_element(m_hash, m_idx++));
      return NULL;
    }
    void rewind() { m_idx= 0; }
  private:
    HASH *m_hash;
    uint m_idx;
  };
private:
  HASH m_hash;
};

#endif // SQL_HSET_INCLUDED
