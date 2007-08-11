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

Tcl_Obj *
DigestToBase32 (
		byte    digest[]
		);

Tcl_Obj *
DigestToRaw (
		byte    digest[]
		);

Tcl_Obj *
DigestToHex (
		byte   digest[]
		);

#endif /* __TCLOUT_H */

