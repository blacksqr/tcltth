/*
 * $Id$
 */

#ifndef __TIGER_H
#define __TIGER_H

/* TODO figure out somehow why BIG_ENDIAN
 * is defined here when compiling the code using gcc
 * on LE platform
 */

#ifdef WORDS_BIGENDIAN
#define BIG_ENDIAN 1
#else
#undef BIG_ENDIAN
#endif /* WORDS_BIGENDIAN */

#if defined(_MSC_VER) || defined(__WATCOMC__)
#define LL(x) (x##i64)
#define ULL(x) (x##Ui64)
typedef unsigned __int64 word64;
#else
#define LL(x) (x##LL)
#define ULL(x) (x##ULL)
typedef unsigned long long int word64;
#endif

/* TODO
 * Consider making use of TCL_WIDE_INT_TYPE,
 * also check for HAVE_TYPES_H
 * and make use of it */
typedef unsigned short word16;
typedef unsigned long word32;
typedef unsigned char byte;

/* tiger hash result size, in bytes */
#define TIGERSIZE 24

void tiger(word64 *str, word64 length, word64 res[3]);

/* Endianness of the three wide ints produced by tiger()
 * is platform-dependent, but in Tcl extension we want
 * to produce results irrelevant to the platform's endianness
 * so the procedure tiger_to_canonical() should be called
 * after any call to tiger() to get the "canonical" string
 * of bytes (as is three wide ints placed LSB first and
 * concatenated).
 * On LE platforms this procedure is compiled to nothing.
 *
 * NOTE that tt_digest() from tigertree.c always returns
 * "canonical" representation, so no call to
 * tiger_to_canonical() is needed.
 */
#ifdef BIG_ENDIAN
void tiger_to_canonical(byte *s);
#else
#define tiger_to_canonical(x) do {} while (0)
#endif

#endif /* __TIGER_H */

