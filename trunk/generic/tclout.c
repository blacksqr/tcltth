/*
 * tclout.c --
 *
 *	This file implements various methods of converting
 *	a tiger hash value to various output formats.
 *
 * Copyright (c) 2007 Konstantin Khomoutov <flatworm@users.sourceforge.net>
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * $Id$
 *
 */

#include <tcl.h>
#include <string.h>

#include "tclout.h"
#include "base32.h"


/*
 *
 */
Tcl_Obj *
DigestToTHEX (
		byte           digest[],
		DIGEST_BITLEN  bitlen
		)
{
	char TTX[BASE32_DESTLEN(TIGERSIZE)];
	int bytelen;

	bytelen = bitlen / 8;

	to_base32(digest, bytelen, TTX);
	return Tcl_NewStringObj(TTX, -1);
}


/*
 *
 */
Tcl_Obj *
DigestToRaw (
		byte           digest[],
		DIGEST_BITLEN  bitlen
		)
{
	return Tcl_NewByteArrayObj(digest, bitlen / 8);
}


Tcl_Obj *
DigestToHex (
		byte           digest[],
		DIGEST_BITLEN  bitlen
		)
{
	static const char alph[] = "0123456789abcdef";
	int si, di;
	int bytelen;
	char hex[TIGERSIZE * 2];

	bytelen = bitlen / 8;

	for (si = di = 0; si < bytelen; ++si, ++di) {
		hex[di]   = alph[digest[si] >> 4];
		hex[++di] = alph[digest[si] & 0x0F];
	}

	return Tcl_NewStringObj(hex, bytelen * 2);
}

