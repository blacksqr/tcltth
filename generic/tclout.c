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
DigestToBase32 (
		byte    digest[]
		)
{
	char TTX[BASE32_DESTLEN(TIGERSIZE)];

	to_base32(digest, TIGERSIZE, TTX);
	return Tcl_NewStringObj(TTX, -1);
}


/*
 *
 */
Tcl_Obj *
DigestToRaw (
		byte    digest[]
		)
{
	return Tcl_NewByteArrayObj(digest, TIGERSIZE);
}


Tcl_Obj *
DigestToHex (
		byte   digest[]
		)
{
	static const char alph[] = "0123456789abcdef";
	int si, di;
	const int len = TIGERSIZE * 2;
	char hex[len];

	for (si = di = 0; si < TIGERSIZE; ++si, ++di) {
		hex[di]   = alph[digest[si] >> 4];
		hex[++di] = alph[digest[si] & 0x0F];
	}

	return Tcl_NewStringObj(hex, len);
}

