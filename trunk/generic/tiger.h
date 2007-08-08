/*
 * $Id$
 */

#ifndef __TIGER_H
#define __TIGER_H

#ifdef WORDS_BIGENDIAN
#define BIG_ENDIAN 1
#else
#undef BIG_ENDIAN
#endif

#if defined(_MSC_VER) || defined(__WATCOMC__)
#define LL(x) (x##i64)
#define UL(x) (x##Ui64)
typedef unsigned __int64 word64;
#else
#define LL(x) (x##LL)
#define UL(x) (x##ULL)
typedef unsigned long long int word64;
#endif

/* TODO
 * make use of TCL_WIDE_INT_TYPE,
 * also check for HAVE_TYPES_H
 * and make use of it */
typedef unsigned short word16;
typedef unsigned long word32;
typedef unsigned char byte;

void tiger(word64 *str, word64 length, word64 res[3]);

#endif /* __TIGER_H */

