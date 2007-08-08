/*
 * tclsample.c --
 *
 *	This file implements a Tcl interface to the secure hashing
 *	algorithm functions in sha1.c
 *
 * Copyright (c) 1999 Scriptics Corporation.
 * Copyright (c) 2003 ActiveState Corporation.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * $Id$
 *
 */

/*
 * Modified from tclmd5.c by Dave Dykstra, dwd@bell-labs.com, 4/22/97
 */

#include <tcl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tiger.h"
#include "tigertree.h"
#include "base32.h"

#define TCL_READ_CHUNK_SIZE 4096

static int nctx = 0;
static TT_CONTEXT *ttcontexts = NULL;
/* static int *ctxtotalRead = NULL; */


/*
 *----------------------------------------------------------------------
 *
 * Sha1 --
 *
 *	 Implements the new Tcl "sha1" command.
 *
 * Results:
 *	A standard Tcl result
 *
 * Side effects:
 *	None.
 *
 *----------------------------------------------------------------------
 */

static int
TTH_Cmd(
    ClientData clientData,	/* Not used. */
    Tcl_Interp *interp,		/* Current interpreter */
    int objc,			/* Number of arguments */
    Tcl_Obj *const objv[]	/* Argument strings */
    )
{
	return 0;
}


/*
 *----------------------------------------------------------------------
 *
 * Sample_Init --
 *
 *	Initialize the new package.  The string "Sample" in the
 *	function name must match the PACKAGE declaration at the top of
 *	configure.in.
 *
 * Results:
 *	A standard Tcl result
 *
 * Side effects:
 *	The Tclsha1 package is created.
 *	One new command "sha1" is added to the Tcl interpreter.
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
    if (Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION) != TCL_OK) {
	return TCL_ERROR;
    }
    Tcl_CreateObjCommand(interp, "::tth::tth",
		(Tcl_ObjCmdProc *) TTH_Cmd,
	    (ClientData)NULL, (Tcl_CmdDeleteProc *)NULL);

    nctx = 1;
	ttcontexts = (TT_CONTEXT *) ckalloc(sizeof(TT_CONTEXT));
	/*
    ctxtotalRead = (int *) malloc(sizeof(int));
    ctxtotalRead[0] = 0;
	*/

    return TCL_OK;
}
