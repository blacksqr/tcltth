/*
 * tcltiger.h --
 *
 *	This file implements a Tcl interface to the
 *	Tiger Hash (v1) algorythm.
 *
 * Copyright (c) 2007 Konstantin Khomoutov <flatworm@users.sourceforge.net>
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * $Id$
 *
 */

#ifndef __TCLTIGER_H
#define __TCLTIGER_H

#include <tcl.h>

Tcl_Command
Tiger_CreateCmd (
		Tcl_Interp *interp
		);

#endif /* __TCLTIGER_H */

