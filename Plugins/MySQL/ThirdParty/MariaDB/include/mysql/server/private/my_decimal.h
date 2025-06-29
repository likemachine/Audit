/* Copyright (c) 2005, 2013, Oracle and/or its affiliates.
   Copyright (c) 2011, 2014, SkySQL Ab.

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

/**
  @file

  It is interface module to fixed precision decimals library.

  Most functions use 'uint mask' as parameter, if during operation error
  which fit in this mask is detected then it will be processed automatically
  here. (errors are E_DEC_* constants, see include/decimal.h)

  Most function are just inline wrappers around library calls
*/

#ifndef my_decimal_h
#define my_decimal_h

#include "sql_basic_types.h"

#if defined(MYSQL_SERVER) || defined(EMBEDDED_LIBRARY)
#include "sql_string.h"                         /* String */
#endif

C_MODE_START
#include <decimal.h>
#include <my_decimal_limits.h>
C_MODE_END

class String;
class Field;
typedef struct st_mysql_time MYSQL_TIME;

/**
  maximum size of packet length.
*/
#define DECIMAL_MAX_FIELD_SIZE DECIMAL_MAX_PRECISION


inline uint my_decimal_size(decimal_digits_t precision,
                            decimal_digits_t scale)
{
  /*
    Always allocate more space to allow library to put decimal point
    where it want
  */
  return decimal_size(precision, scale) + 1;
}


inline decimal_digits_t my_decimal_int_part(decimal_digits_t precision,
                                            decimal_digits_t decimals)
{
  return (decimal_digits_t) (precision -
                             ((decimals == DECIMAL_NOT_SPECIFIED) ? 0 :
                              decimals));
}


#ifndef MYSQL_CLIENT
int decimal_operation_results(int result, const char *value, const char *type);
#else
inline int decimal_operation_results(int result, const char *value,
                                     const char *type)
{
  return result;
}
#endif /*MYSQL_CLIENT*/


inline int check_result(uint mask, int result)
{
  if (result & mask)
    decimal_operation_results(result, "", "DECIMAL");
  return result;
}


/**
  my_decimal class limits 'decimal_t' type to what we need in MySQL.

  It contains internally all necessary space needed by the instance so
  no extra memory is needed. One should call fix_buffer_pointer() function
  when he moves my_decimal objects in memory.
*/

class my_decimal :public decimal_t
{
  /*
    Several of the routines in strings/decimal.c have had buffer
    overrun/underrun problems. These are *not* caught by valgrind.
    To catch them, we allocate dummy fields around the buffer,
    and test that their values do not change.
   */
#if !defined(DBUG_OFF)
  int foo1;
#endif

  decimal_digit_t buffer[DECIMAL_BUFF_LENGTH];

#if !defined(DBUG_OFF)
  int foo2;
  static const int test_value= 123;
#endif

public:

  my_decimal(const my_decimal &rhs) : decimal_t(rhs)
  {
    init();
    for (uint i= 0; i < DECIMAL_BUFF_LENGTH; i++)
      buffer[i]= rhs.buffer[i];
  }

  my_decimal& operator=(const my_decimal &rhs)
  {
    if (this == &rhs)
      return *this;
    decimal_t::operator=(rhs);
    for (uint i= 0; i < DECIMAL_BUFF_LENGTH; i++)
      buffer[i]= rhs.buffer[i];
    fix_buffer_pointer();
    return *this;
  }

  void init()
  {
#if !defined(DBUG_OFF)
    foo1= test_value;
    foo2= test_value;
#endif
    len= DECIMAL_BUFF_LENGTH;
    buf= buffer;
    TRASH_ALLOC(buffer, sizeof(buffer));
  }

  my_decimal()
  {
    init();
  }
  my_decimal(const uchar *bin, decimal_digits_t prec, decimal_digits_t scale)
  {
    init();
    check_result(E_DEC_FATAL_ERROR, bin2decimal(bin, this, prec, scale));
  }
  my_decimal(Field *field);
  ~my_decimal()
  {
    sanity_check();
  }

  void sanity_check()
  {
    DBUG_SLOW_ASSERT(foo1 == test_value);
    DBUG_SLOW_ASSERT(foo2 == test_value);
  }

  void fix_buffer_pointer() { buf= buffer; }

  bool sign() const { return decimal_t::sign; }
  void sign(bool s) { decimal_t::sign= s; }
  decimal_digits_t precision() const { return (decimal_digits_t) (intg + frac); }
  void set_zero()
  {
    /*
      We need the up-cast here, since my_decimal has sign() member functions,
      which conflicts with decimal_t::sign
      (and decimal_make_zero is a macro, rather than a funcion).
    */
    decimal_make_zero(static_cast<decimal_t*>(this));
  }
  int cmp(const my_decimal *other) const
  {
    return decimal_cmp(this, other);
  }

#ifndef MYSQL_CLIENT
  bool to_bool() const
  {
    return !decimal_is_zero(this);
  }
  double to_double() const
  {
    double res;
    decimal2double(this, &res);
    return res;
  }
  longlong to_longlong(bool unsigned_flag) const;
  /*
    Return the value as a signed or unsigned longlong, depending on the sign.
    - Positive values are returned as unsigned.
    - Negative values are returned as signed.
    This is used by bit SQL operators: | & ^ ~
    as well as by the SQL function BIT_COUNT().
  */
  longlong to_xlonglong() const
  { return to_longlong(!sign()); }

  // Convert to string returning decimal2string() error code
  int to_string_native(String *to, uint prec, uint dec, char filler,
                       uint mask= E_DEC_FATAL_ERROR) const;
  // Convert to string returning the String pointer
  String *to_string(String *to, uint prec, uint dec, char filler) const
  {
    return to_string_native(to, prec, dec, filler) ? NULL : to;
  }
  String *to_string(String *to) const
  {
    return to_string(to, 0, 0, 0);
  }
  String *to_string_round(String *to, decimal_digits_t scale,
                          my_decimal *round_buff) const
  {
    (void) round_to(round_buff, scale, HALF_UP); // QQ: check result?
    return round_buff->to_string(to);
  }
  /* Scale can be negative here when called from truncate() */
  int round_to(my_decimal *to, int scale, decimal_round_mode mode,
               int mask= E_DEC_FATAL_ERROR) const
  {
    return check_result(mask, decimal_round(this, to, scale, mode));
  }
  int to_binary(uchar *bin, int prec, decimal_digits_t scale,
                uint mask= E_DEC_FATAL_ERROR) const;
#endif
  /** Swap two my_decimal values */
  void swap(my_decimal &rhs)
  {
    swap_variables(my_decimal, *this, rhs);
  }
};


#ifndef DBUG_OFF
void print_decimal(const my_decimal *dec);
void print_decimal_buff(const my_decimal *dec, const uchar* ptr, int length);
const char *dbug_decimal_as_string(char *buff, const my_decimal *val);
#else
#define dbug_decimal_as_string(A) NULL
#endif

bool str_set_decimal(uint mask, const my_decimal *val, uint fixed_prec,
                     uint fixed_dec, char filler, String *str,
                     CHARSET_INFO *cs);

extern my_decimal decimal_zero;

inline
void max_my_decimal(my_decimal *to, decimal_digits_t precision,
                    decimal_digits_t frac)
{
  DBUG_ASSERT((precision <= DECIMAL_MAX_PRECISION)&&
              (frac <= DECIMAL_MAX_SCALE));
  max_decimal(precision, frac, to);
}

inline void max_internal_decimal(my_decimal *to)
{
  max_my_decimal(to, DECIMAL_MAX_PRECISION, 0);
}

inline int check_result_and_overflow(uint mask, int result, my_decimal *val)
{
  if (check_result(mask, result) & E_DEC_OVERFLOW)
  {
    bool sign= val->sign();
    val->fix_buffer_pointer();
    max_internal_decimal(val);
    val->sign(sign);
  }
  return result;
}

inline decimal_digits_t my_decimal_length_to_precision(decimal_digits_t length,
                                                       decimal_digits_t scale,
                                                       bool unsigned_flag)
{
  /* Precision can't be negative thus ignore unsigned_flag when length is 0. */
  DBUG_ASSERT(length || !scale);
  return (decimal_digits_t) (length - (scale>0 ? 1:0) -
                             (unsigned_flag || !length ? 0:1));
}

inline decimal_digits_t
my_decimal_precision_to_length_no_truncation(decimal_digits_t precision,
                                             decimal_digits_t scale,
                                             bool unsigned_flag)
{
  /*
    When precision is 0 it means that original length was also 0. Thus
    unsigned_flag is ignored in this case.
  */
  DBUG_ASSERT(precision || !scale);
  return (decimal_digits_t)(precision + (scale > 0 ? 1 : 0) +
                            (unsigned_flag || !precision ? 0 : 1));
}

inline decimal_digits_t
my_decimal_precision_to_length(decimal_digits_t precision,
                               decimal_digits_t scale,
                               bool unsigned_flag)
{
  /*
    When precision is 0 it means that original length was also 0. Thus
    unsigned_flag is ignored in this case.
  */
  DBUG_ASSERT(precision || !scale);
  set_if_smaller(precision, DECIMAL_MAX_PRECISION);
  return my_decimal_precision_to_length_no_truncation(precision, scale,
                                                      unsigned_flag);
}

inline
uint my_decimal_string_length(const my_decimal *d)
{
  /* length of string representation including terminating '\0' */
  return decimal_string_size(d);
}


inline
uint my_decimal_max_length(const my_decimal *d)
{
  /* -1 because we do not count \0 */
  return decimal_string_size(d) - 1;
}


inline
uint my_decimal_get_binary_size(decimal_digits_t precision,
                                decimal_digits_t scale)
{
  return decimal_bin_size(precision, scale);
}


inline
void my_decimal2decimal(const my_decimal *from, my_decimal *to)
{
  *to= *from;
}


inline
int binary2my_decimal(uint mask, const uchar *bin, my_decimal *d,
                      decimal_digits_t prec, decimal_digits_t scale)
{
  return check_result(mask, bin2decimal(bin, d, prec, scale));
}


inline
int my_decimal_set_zero(my_decimal *d)
{
  d->set_zero();
  return 0;
}


inline
bool my_decimal_is_zero(const my_decimal *decimal_value)
{
  return decimal_is_zero(decimal_value);
}


inline bool str_set_decimal(const my_decimal *val, String *str,
                            CHARSET_INFO *cs)
{
  return str_set_decimal(E_DEC_FATAL_ERROR, val, 0, 0, 0, str, cs);
}


bool my_decimal2seconds(const my_decimal *d, ulonglong *sec,
                        ulong *microsec, ulong *nanosec);

my_decimal *seconds2my_decimal(bool sign, ulonglong sec, ulong microsec,
                               my_decimal *d);

#define TIME_to_my_decimal(TIME, DECIMAL)                       \
     seconds2my_decimal((TIME)->neg, TIME_to_ulonglong(TIME),   \
                        (TIME)->second_part, (DECIMAL))

int my_decimal2int(uint mask, const decimal_t *d, bool unsigned_flag,
		   longlong *l, decimal_round_mode round_type= HALF_UP);

inline
int my_decimal2double(uint, const decimal_t *d, double *result)
{
  /* No need to call check_result as this will always succeed */
  return decimal2double(d, result);
}


inline
int str2my_decimal(uint mask, const char *str, my_decimal *d, char **end)
{
  return check_result_and_overflow(mask, string2decimal(str, d, end), d);
}


int str2my_decimal(uint mask, const char *from, size_t length,
                   CHARSET_INFO *charset, my_decimal *decimal_value,
                   const char **end);

inline int str2my_decimal(uint mask, const char *from, size_t length,
                          CHARSET_INFO *charset, my_decimal *decimal_value)
{
  const char *end;
  return str2my_decimal(mask, from, length, charset, decimal_value, &end);
}

#if defined(MYSQL_SERVER) || defined(EMBEDDED_LIBRARY)
inline
int string2my_decimal(uint mask, const String *str, my_decimal *d)
{
  const char *end;
  return str2my_decimal(mask, str->ptr(), str->length(), str->charset(),
                        d, &end);
}


my_decimal *date2my_decimal(const MYSQL_TIME *ltime, my_decimal *dec);


#endif /*defined(MYSQL_SERVER) || defined(EMBEDDED_LIBRARY) */

inline
int double2my_decimal(uint mask, double val, my_decimal *d)
{
  return check_result_and_overflow(mask, double2decimal(val, d), d);
}


inline
int int2my_decimal(uint mask, longlong i, my_bool unsigned_flag, my_decimal *d)
{
  return check_result(mask, (unsigned_flag ?
			     ulonglong2decimal((ulonglong)i, d) :
			     longlong2decimal(i, d)));
}

inline
void decimal2my_decimal(decimal_t *from, my_decimal *to)
{
  DBUG_ASSERT(to->len >= from->len);
  to->intg= from->intg;
  to->frac= from->frac;
  to->sign(from->sign);
  memcpy(to->buf, from->buf, to->len*sizeof(decimal_digit_t));
}


inline
void my_decimal_neg(decimal_t *arg)
{
  if (decimal_is_zero(arg))
  {
    arg->sign= 0;
    return;
  }
  decimal_neg(arg);
}


inline
int my_decimal_add(uint mask, my_decimal *res, const my_decimal *a,
		   const my_decimal *b)
{
  return check_result_and_overflow(mask,
                                   decimal_add(a, b, res),
                                   res);
}


inline
int my_decimal_sub(uint mask, my_decimal *res, const my_decimal *a,
		   const my_decimal *b)
{
  return check_result_and_overflow(mask,
                                   decimal_sub(a, b, res),
                                   res);
}


inline
int my_decimal_mul(uint mask, my_decimal *res, const my_decimal *a,
		   const my_decimal *b)
{
  return check_result_and_overflow(mask,
                                   decimal_mul(a, b, res),
                                   res);
}


inline
int my_decimal_div(uint mask, my_decimal *res, const my_decimal *a,
		   const my_decimal *b, int div_scale_inc)
{
  return check_result_and_overflow(mask,
                                   decimal_div(a, b, res, div_scale_inc),
                                   res);
}


inline
int my_decimal_mod(uint mask, my_decimal *res, const my_decimal *a,
		   const my_decimal *b)
{
  return check_result_and_overflow(mask,
                                   decimal_mod(a, b, res),
                                   res);
}

/**
  @return
    -1 if a<b, 1 if a>b and 0 if a==b
*/
inline
int my_decimal_cmp(const my_decimal *a, const my_decimal *b)
{
  return decimal_cmp(a, b);
}


inline
int my_decimal_intg(const my_decimal *a)
{
  return decimal_intg(a);
}


void my_decimal_trim(ulonglong *precision, decimal_digits_t *scale);


#endif /*my_decimal_h*/

