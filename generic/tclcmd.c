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
#include "tclcmd.h"

#define TCL_READ_CHUNK_SIZE 4096

typedef {
	Tcl_HashTable contexts;
	unsigned int uid;
} TTH_State;


/*
 *
 */
void
TTH_CleanupState (
		ClientData clientData
		)
{
	TTH_State      *statePtr;
	Tcl_HashEntry  *entryPtr;
	Tcl_HashSearch search;

	statePtr = (TTH_State *) clientData;

	entryPtr = Tcl_FirstHashEntry(&statePtr->contexts, &search);
	while (entryPtr != NULL) {
		ckfree((char *) Tcl_GetHashValue(entryPtr));

		entryPtr = Tcl_FirstHashEntry(&statePtr->contexts, &search);
	}

	ckfree((char *) statePtr);
}


/*
 *
 */
void
TTH_CreateContext (
		TTH_State *statePtr,
		Tcl_Obj   *tokenPtr
		)
{
	char token[3 + 10 + 1];
	Tcl_HashEntry *entryPtr;
	int new;
	TT_CONTEXT *contextPtr;

	sprintf(token, "tth%u", statePtr->uid);

	entryPtr = Tcl_CreateHashEntry(&statePtr->contexts, token, &new);
	if (new != 1) {
		Tcl_Panic("TTH context \"%s\" stomped on existing one", token);
	}

	contextPtr = (TT_CONTEXT *) ckalloc(sizeof(TT_CONTEXT));
	Tcl_SetHashValue(entryPtr, (ClientData)contextPtr);
	tt_init(contextPtr);

	Tcl_SetStringObj(tokenPtr, token, TCL_VOLATILE);
}


/*
 *
 */
Tcl_HashEntry *
TTH_FindContext (
		TTH_State *statePtr,
		Tcl_Obj   *tokenPtr
		)
{
	return Tcl_FindHashEntry(&statePtr->contexts,
			Tcl_GetString(tokenPtr));
}


/*
 *
 */
void
TTH_DeleteContext (
		Tcl_HashEntry *entryPtr
		)
{
	TT_CONTEXT *contextPtr;

	contextPtr = (TT_CONTEXT *) Tcl_GetHashValue(entryPtr);
	ckfree((char *) contextPtr);

	Tcl_DeleteHashEntry(entryPtr);
}


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
	ClientData clientData,  /* pointer to a TTH_State object */
	Tcl_Interp *interp,     /* Current interpreter */
	int objc,               /* Number of arguments */
	Tcl_Obj *const objv[]   /* Argument strings */
	)
{
	static const char *options[] = { "init", "update", "digest", NULL };
	typedef enum { TTH_INIT, TTH_UPDATE, TTH_DIGEST } TTH_Option;
	int i;

	if (objc == 1) {
		Tcl_SetResult(interp, "\
wrong # args: should be one of:\n\
tth init\n\
or\n\
tth update tthContext sourceString\n\
or\n\
tth digest ?options? tthContext\n\
tth digest ?options? -string sourceString\n\
tth digest ?options? -chan fileChannel\n\
where options are: -base32|-hex, -le|-be|-ref",
				TCL_VOLATILE);
		return TCL_ERROR;
	}

	if (Tcl_GetIndexFromObj(interp, objv[1], options, "option",
			0, &i) != TCL_OK) { return TCL_ERROR; }

	switch ((TTH_Option)i) {
		TTH_State *statePtr;
		unsigned char *parray;
		int len;

		statePtr = (TTH_State *) ClientData;

		case TTH_INIT:
			if (objc != 2) {
				Tcl_WrongNumArgs(interp, 2, objv, NULL);
				return TCL_ERROR;
			}
			TTH_CreateContext(statePtr, 
			sprintf(token, "tth%u", nctx);
			Tcl_SetResult(interp, token, TCL_VOLATILE);
			return TCL_OK;
		break;

		case TTH_UPDATE:
			if (objc != 4) {
				Tcl_WrongNumArgs(interp, 2, objv,
						"tthContext sourceString");
				return TCL_ERROR;
			}
			parray = Tcl_GetByteArrayFromObj(objv[2], &len);
			tt_update(&ttcontexts[0], parray, len);
			Tcl_ResetResult(interp);
			return TCL_OK;
		break;

		case TTH_DIGEST:
			Tcl_AppendResult(interp, "not implemented", NULL);
			return TCL_ERROR;
		break;
	}
}


/*
 *----------------------------------------------------------------------
 *
 * Tcltth_Init --
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
	TTH_State *statePtr
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

	statePtr = (TTH_State *) ckalloc(sizeof(TTH_State));
	Tcl_InitHashTable(&statePtr->contexts, TCL_STRING_KEYS);
	statePtr->uid = 0;

	Tcl_CreateObjCommand(interp, "::tth::tth",
		(Tcl_ObjCmdProc *) TTH_Cmd,
		(ClientData) statePtr, (Tcl_CmdDeleteProc *) TTH_CleanupState);

	if (Tcl_PkgProvide(interp, PACKAGE_NAME, PACKAGE_VERSION) != TCL_OK) {
		return TCL_ERROR;
	}

	return TCL_OK;
}

