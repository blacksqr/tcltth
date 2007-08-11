/*
 * tcltth.h --
 *
 *	This file implements a Tcl interface to the
 *	Tiger Tree Hashing (TTH) algorythm with support for TTX.
 *
 * Copyright (c) 2007 Konstantin Khomoutov <flatworm@users.sourceforge.net>
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * $Id$
 *
 */

#ifndef __TCLTTH_H
#define __TCLTTH_H

#include <tcl.h>

Tcl_Command
TTH_CreateCmd (
		Tcl_Interp *interp
		);

#endif /* __TCLTTH_H */

