/*
 * tclinit.c --
 *
 *	This file initializes the "tcltth" package
 *	arranging for any necessary commands to be created
 *	and the package to be "provided".
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

#include "tcltth.h"
#include "tcltiger.h"

#ifdef BUILD_tcltth
#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLEXPORT
#endif /* BUILD_tcltth */

EXTERN int	Tcltth_Init(Tcl_Interp * interp);


/*
 *----------------------------------------------------------------------
 *
 * Tcltth_Init --
 *
 *	Initializes package "tcltth".
 *
 * Results:
 *	A standard Tcl result
 *
 * Side effects:
 *	The "tcltth" package is created.
 *	One new command "::tth::tth" is added to the Tcl interpreter.
 *
 *----------------------------------------------------------------------
 */

int
Tcltth_Init(Tcl_Interp *interp)
{
	/*
	 * This may work with 8.0, but we are using strictly stubs here,
	 * which requires 8.1.
	 */
	if (Tcl_InitStubs(interp, "8.1", 0) == NULL) {
		return TCL_ERROR;
	}
	if (Tcl_PkgRequire(interp, "Tcl", "8.1", 0) == NULL) {
		return TCL_ERROR;
	}

	if (Tiger_CreateCmd(interp) == NULL) { return TCL_ERROR; }
	if (TTH_CreateCmd(interp) == NULL) { return TCL_ERROR; }

	if (Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION) != TCL_OK) {
		return TCL_ERROR;
	}

	return TCL_OK;
}

