/* types.h - some common typedefs
 *	Copyright (C) 1998, 2000, 2002, 2003 Free Software Foundation, Inc.
 *
 * This file is part of Libgcrypt.
 *
 * Libgcrypt is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser general Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * Libgcrypt is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */

#ifndef GCRYPT_TYPES_H
#define GCRYPT_TYPES_H


/* The AC_CHECK_SIZEOF() in configure fails for some machines.
 * we provide some fallback values here */
#if !SIZEOF_UNSIGNED_SHORT
# undef SIZEOF_UNSIGNED_SHORT
# define SIZEOF_UNSIGNED_SHORT 2
#endif
#if !SIZEOF_UNSIGNED_INT
# undef SIZEOF_UNSIGNED_INT
# define SIZEOF_UNSIGNED_INT 4
#endif
#if !SIZEOF_UNSIGNED_LONG
# undef SIZEOF_UNSIGNED_LONG
# define SIZEOF_UNSIGNED_LONG 4
#endif


#include <sys/types.h>

/* Provide uintptr_t */
#ifdef HAVE_STDINT_H
# include <stdint.h> /* uintptr_t */
#elif defined(HAVE_INTTYPES_H)
# include <inttypes.h>
#else
/* In this case, uintptr_t is provided by config.h. */
#endif




   typedef unsigned char byte;


  typedef unsigned short ushort;



   typedef unsigned short u16;



   typedef unsigned int  u32;


   typedef unsigned long u64;


typedef union
{
  int a;
  short b;
  char c[1];
  long d;
  u64 e;
  float f;
  double g;
} PROPERLY_ALIGNED_TYPE;

#endif /*GCRYPT_TYPES_H*/
