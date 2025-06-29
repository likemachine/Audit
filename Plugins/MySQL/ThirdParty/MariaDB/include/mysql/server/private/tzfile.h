#ifndef TZFILE_INCLUDED
#define TZFILE_INCLUDED

/* Copyright (c) 2004, 2006, 2007 MySQL AB, 2009 Sun Microsystems, Inc.
   Use is subject to license terms.

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

/* 
   This file is based on public domain code from ftp://elsie.ncih.nist.gov/
   Initial source code is in the public domain, so clarified as of
   1996-06-05 by Arthur David Olson (arthur_david_olson@nih.gov). 
*/

/*
  Information about time zone files.
*/

#ifndef TZDIR
#define TZDIR	"/usr/share/zoneinfo" /* Time zone object file directory */
#endif /* !defined TZDIR */

/*
  Each file begins with. . .
*/

#define	TZ_MAGIC	"TZif"

struct tzhead {
 	uchar	tzh_magic[4];		/* TZ_MAGIC */
	uchar	tzh_reserved[16];	/* reserved for future use */
	uchar	tzh_ttisgmtcnt[4];	/* coded number of trans. time flags */
	uchar	tzh_ttisstdcnt[4];	/* coded number of trans. time flags */
	uchar	tzh_leapcnt[4];		/* coded number of leap seconds */
	uchar	tzh_timecnt[4];		/* coded number of transition times */
	uchar	tzh_typecnt[4];		/* coded number of local time types */
	uchar	tzh_charcnt[4];		/* coded number of abbr. chars */
};

/*
  . . .followed by. . .
  
  tzh_timecnt (char [4])s               coded transition times a la time(2)
  tzh_timecnt (unsigned char)s          types of local time starting at above
  tzh_typecnt repetitions of
    one (char [4])                      coded UTC offset in seconds
    one (unsigned char)                 used to set tm_isdst
    one (unsigned char)                 that's an abbreviation list index
  tzh_charcnt (char)s                   '\0'-terminated zone abbreviations
  tzh_leapcnt repetitions of
    one (char [4])                      coded leap second transition times
    one (char [4])                      total correction after above
  tzh_ttisstdcnt (char)s                indexed by type; if TRUE, transition
                                        time is standard time, if FALSE,
                                        transition time is wall clock time
                                        if absent, transition times are
                                        assumed to be wall clock time
  tzh_ttisgmtcnt (char)s                indexed by type; if TRUE, transition
                                        time is UTC, if FALSE,
                                        transition time is local time
                                        if absent, transition times are
                                        assumed to be local time
*/

/*
  In the current implementation, we refuse to deal with files that
  exceed any of the limits below.
*/

#ifndef TZ_MAX_TIMES
/*
  The TZ_MAX_TIMES value below is enough to handle a bit more than a
  year's worth of solar time (corrected daily to the nearest second) or
  138 years of Pacific Presidential Election time
  (where there are three time zone transitions every fourth year).
*/
#define TZ_MAX_TIMES	370
#endif /* !defined TZ_MAX_TIMES */

#ifndef TZ_MAX_TYPES
#ifdef SOLAR
#define TZ_MAX_TYPES	256 /* Limited by what (unsigned char)'s can hold */
#else
/*
  Must be at least 14 for Europe/Riga as of Jan 12 1995,
  as noted by Earl Chew <earl@hpato.aus.hp.com>.
*/
#define TZ_MAX_TYPES	20	/* Maximum number of local time types */
#endif /* defined SOLAR */
#endif /* !defined TZ_MAX_TYPES */

#ifndef TZ_MAX_CHARS
#define TZ_MAX_CHARS	50	/* Maximum number of abbreviation characters */
				/* (limited by what unsigned chars can hold) */
#endif /* !defined TZ_MAX_CHARS */

#ifndef TZ_MAX_LEAPS
#define TZ_MAX_LEAPS	50	/* Maximum number of leap second corrections */
#endif /* !defined TZ_MAX_LEAPS */

#ifndef TZ_MAX_REV_RANGES
#ifdef SOLAR
/* Solar (Asia/RiyadhXX) zones need significantly bigger TZ_MAX_REV_RANGES */
#define TZ_MAX_REV_RANGES (TZ_MAX_TIMES*2+TZ_MAX_LEAPS*2+2)
#else
#define TZ_MAX_REV_RANGES (TZ_MAX_TIMES+TZ_MAX_LEAPS+2)
#endif
#endif

#define SECS_PER_MIN	60
#define MINS_PER_HOUR	60
#define HOURS_PER_DAY	24
#define DAYS_PER_WEEK	7
#define DAYS_PER_NYEAR	365
#define DAYS_PER_LYEAR	366
#define SECS_PER_HOUR	(SECS_PER_MIN * MINS_PER_HOUR)
#define SECS_PER_DAY	((long) SECS_PER_HOUR * HOURS_PER_DAY)
#define MONS_PER_YEAR	12

#define TM_YEAR_BASE	1900

#define EPOCH_YEAR	1970

/*
  Accurate only for the past couple of centuries,
  that will probably do.
*/

#define isleap(y) (((y) % 4) == 0 && (((y) % 100) != 0 || ((y) % 400) == 0))

#endif
