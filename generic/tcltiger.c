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
typedef enum {
	DO_HEX,      /* -hex, default */
	DO_RAW       /* -raw */
} DIGEST_OUTPUT;


/*
 *
 */
static int
Cmd_ParseDigestOptions (
		Tcl_Interp     *interp,
		Tcl_Obj *const objv[],
		int            objc,
		DIGEST_OUTPUT  *outputPtr,
		DIGEST_BITLEN  *bitlenPtr
		)
{
	int i, op;

	static const char *options[] = { "-hex", "-raw",
		"-192", "-160", "-128", NULL };
	typedef enum { OP_HEX, OP_RAW, OP_192, OP_160, OP_128 } OPTION;

	/* Options start from index 1 and the last object is always a "value": */
	const int first = 1;
	int       last  = objc - 2;

	/* Defaults */
	*outputPtr = DO_HEX;
	*bitlenPtr = DL_192;

	for (i = first; i <= last; ++i) {
		if (Tcl_GetIndexFromObj(interp, objv[i], options, "option",
				0, &op) != TCL_OK) { return TCL_ERROR; }
		switch (op) {
			case OP_HEX:
				*outputPtr = DO_HEX;
			break;
			case OP_RAW:
				*outputPtr = DO_RAW;
			break;
			case OP_192:
				*bitlenPtr = DL_192;
			break;
			case OP_160:
				*bitlenPtr = DL_160;
			break;
			case OP_128:
				*bitlenPtr = DL_128;
			break;
		}
	}

	return TCL_OK;
}


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
	int len;
	unsigned char *dataPtr;
	DIGEST_OUTPUT dout;
	DIGEST_BITLEN dbitlen;
	byte digest[TIGERSIZE];

	if (objc < 2) {
		Tcl_SetResult(interp,
				"wrong # args: should be tiger ?options? bitstring",
				TCL_VOLATILE);
		return TCL_ERROR;
	}

	if (Cmd_ParseDigestOptions(interp, objv, objc,
				&dout, &dbitlen) != TCL_OK) { return TCL_ERROR; }

	dataPtr = Tcl_GetByteArrayFromObj(objv[objc - 1], &len);
	tiger((word64 *) dataPtr, (word64) len, (word64 *) digest);
	tiger_to_canonical(digest);

	switch (dout) {
		case DO_RAW:
			Tcl_SetObjResult(interp,
				DigestToRaw(digest, dbitlen));
		break;
		case DO_HEX:
			Tcl_SetObjResult(interp,
				DigestToHex(digest, dbitlen));
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

