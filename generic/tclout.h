/*
 * tclout.h --
 *
 *	This file implements interface for tclout.c
 *	to other parts of the library.
 *
 * Copyright (c) 2007 Konstantin Khomoutov <flatworm@users.sourceforge.net>
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * $Id$
 *
 */
#ifndef __TCLOUT_H
#define __TCLOUT_H

#include <tcl.h>
#include "tiger.h"

typedef enum {
	DL_192 = 192,  /* -192, default */
	DL_160 = 160,  /* -160 */
	DL_128 = 128   /* -128 */
} DIGEST_BITLEN;

Tcl_Obj *
DigestToTHEX (
		byte           digest[],
		DIGEST_BITLEN  bitlen
		);

Tcl_Obj *
DigestToRaw (
		byte           digest[],
		DIGEST_BITLEN  bitlen
		);

Tcl_Obj *
DigestToHex (
		byte           digest[],
		DIGEST_BITLEN  bitlen
		);

#endif /* __TCLOUT_H */

