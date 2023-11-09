/* Rijndael (AES) for GnuPG
 * Copyright (C) 2000, 2001, 2002, 2003, 2007,
 *               2008, 2011, 2012 Free Software Foundation, Inc.
 *
 * This file is part of Libgcrypt.
 *
 * Libgcrypt is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * Libgcrypt is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef G10_RIJNDAEL_INTERNAL_H
#define G10_RIJNDAEL_INTERNAL_H

#include "types.h"  /* for byte and u32 typedefs */


#define MAXKC                   (256/32)
#define MAXROUNDS               14
#define BLOCKSIZE               (128/8)


/* Helper macro to force alignment to 16 or 64 bytes.  */
#ifdef HAVE_GCC_ATTRIBUTE_ALIGNED
# define ATTR_ALIGNED_16  __attribute__ ((aligned (16)))
# define ATTR_ALIGNED_64  __attribute__ ((aligned (64)))
#else
# define ATTR_ALIGNED_16
# define ATTR_ALIGNED_64
#endif

#   define USE_AESNI 1

struct RIJNDAEL_context_s;

typedef unsigned int (*rijndael_cryptfn_t)(const struct RIJNDAEL_context_s *ctx,
                                           unsigned char *bx,
                                           const unsigned char *ax);
typedef void (*rijndael_prefetchfn_t)(void);
typedef void (*rijndael_prepare_decfn_t)(struct RIJNDAEL_context_s *ctx);

/* Our context object.  */
typedef struct RIJNDAEL_context_s
{
  /* The first fields are the keyschedule arrays.  This is so that
     they are aligned on a 16 byte boundary if using gcc.  This
     alignment is required for the AES-NI code and a good idea in any
     case.  The alignment is guaranteed due to the way cipher.c
     allocates the space for the context.  The PROPERLY_ALIGNED_TYPE
     hack is used to force a minimal alignment if not using gcc of if
     the alignment requirement is higher that 16 bytes.  */
  union
  {
    PROPERLY_ALIGNED_TYPE dummy;
    byte keyschedule[MAXROUNDS+1][4][4];
    u32 keyschedule32[MAXROUNDS+1][4];
  } u1;
  union
  {
    PROPERLY_ALIGNED_TYPE dummy;
    byte keyschedule[MAXROUNDS+1][4][4];
    u32 keyschedule32[MAXROUNDS+1][4];
  } u2;
  int rounds;                         /* Key-length-dependent number of rounds.  */
  unsigned int decryption_prepared:1; /* The decryption key schedule is available.  */
#ifdef USE_AESNI
  unsigned int use_avx:1;             /* AVX shall be used by AES-NI implementation. */
  unsigned int use_avx2:1;            /* AVX2 shall be used by AES-NI implementation. */
#endif /*USE_AESNI*/
  rijndael_cryptfn_t encrypt_fn;
  rijndael_cryptfn_t decrypt_fn;
  rijndael_prefetchfn_t prefetch_enc_fn;
  rijndael_prefetchfn_t prefetch_dec_fn;
  rijndael_prepare_decfn_t prepare_decryption;
} RIJNDAEL_context ATTR_ALIGNED_16;

/* Macros defining alias for the keyschedules.  */
#define keyschenc   u1.keyschedule
#define keyschenc32 u1.keyschedule32
#define keyschdec   u2.keyschedule
#define keyschdec32 u2.keyschedule32
#define padlockkey  u1.padlock_key

#endif /* G10_RIJNDAEL_INTERNAL_H */
