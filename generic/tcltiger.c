/*
 * tcltiger.c --
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

#include <tcl.h>

#include "tiger.h"
#include "tclout.h"
#include "tcltiger.h"


/*
 *
 */
static int
Tiger_Cmd(
	ClientData clientData,  /* pointer to a TTH_State object */
	Tcl_Interp *interp,     /* Current interpreter */
	int objc,               /* Number of arguments */
	Tcl_Obj *const objv[]   /* Argument strings */
	)
{
	static const char *options[] = { "-base32", "-raw", "-hex", NULL };
	typedef enum { TO_BASE32, TO_RAW, TO_HEX } Tiger_Option;
	int op, len;
	unsigned char *dataPtr;
	byte digest[TIGERSIZE];

	if (objc < 2) {
		Tcl_SetResult(interp,
				"wrong # args: should be tiger ?options? stringValue",
				TCL_VOLATILE);
		return TCL_ERROR;
	}

	if (Tcl_GetIndexFromObj(interp, objv[1], options, "option",
			0, &op) != TCL_OK) { return TCL_ERROR; }

	dataPtr = Tcl_GetByteArrayFromObj(objv[objc - 1], &len);
	tiger((word64 *) dataPtr, (word64) len, (word64 *) digest);
	tiger_to_canonical(digest);

	switch (op) {
		case TO_BASE32:
			Tcl_SetObjResult(interp,
				DigestToBase32(digest));
		break;
		case TO_RAW:
			Tcl_SetObjResult(interp,
				DigestToRaw(digest));
		break;
		case TO_HEX:
			Tcl_SetObjResult(interp,
				DigestToHex(digest));
		break;
	}

	return TCL_OK;
}


/*
 *
 */
Tcl_Command
Tiger_CreateCmd (
		Tcl_Interp *interp
		)
{
	return Tcl_CreateObjCommand(interp, "::tth::tiger",
		(Tcl_ObjCmdProc *) Tiger_Cmd,
		(ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);
}

