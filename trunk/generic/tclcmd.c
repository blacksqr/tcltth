/*
 * tclcmd.c --
 *
 *	This file implements a Tcl interface to the Tiger Hash
 *	and Tiger Tree Hashing algorythms.
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tiger.h"
#include "tigertree.h"
#include "base32.h"
#include "tclcmd.h"

/*
 * Per-interpreter data storage accociated with the ::tth::tth Tcl command.
 */
typedef struct {
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
	++statePtr->uid;

	entryPtr = Tcl_CreateHashEntry(&statePtr->contexts, token, &new);
	if (new != 1) {
		Tcl_Panic("TTH context \"%s\" stomped on existing one", token);
	}

	contextPtr = (TT_CONTEXT *) ckalloc(sizeof(TT_CONTEXT));
	Tcl_SetHashValue(entryPtr, (ClientData)contextPtr);
	tt_init(contextPtr);

	Tcl_SetStringObj(tokenPtr, token, -1);
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
TTH_GetDigest (
		TTH_State *statePtr,
		Tcl_Obj   *tokenPtr,
		Tcl_Obj   *digestPtr
		)
{
	Tcl_HashEntry *entryPtr;
	TT_CONTEXT *contextPtr;
	char digest[TIGERSIZE];
	char TTX[BASE32_DESTLEN(TIGERSIZE)];

	entryPtr = TTH_FindContext(statePtr, tokenPtr);
	contextPtr = (TT_CONTEXT *) Tcl_GetHashValue(entryPtr);

	tt_digest(contextPtr, digest);
	to_base32(digest, TIGERSIZE, TTX);

	Tcl_SetStringObj(digestPtr, TTX, -1);
}


/*
 *
 */
void
TTH_UpdateContext (
		TTH_State *statePtr,
		Tcl_Obj   *tokenPtr,
		Tcl_Obj   *dataPtr 
		)
{
	Tcl_HashEntry *entryPtr;
	TT_CONTEXT    *contextPtr;
	CONST char    *bytesPtr;
	int           len;

	entryPtr = TTH_FindContext(statePtr, tokenPtr);
	contextPtr = (TT_CONTEXT *) Tcl_GetHashValue(entryPtr);

	bytesPtr = Tcl_GetByteArrayFromObj(dataPtr, &len);
	tt_update(contextPtr, bytesPtr, len);
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
 * TTH_Cmd --
 *
 *	Implements the new Tcl "tth" command
 *	placed in the "::tth" namespace.
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
	TTH_State *statePtr;
	Tcl_Obj *resultPtr;

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

	statePtr = (TTH_State *) clientData;

	switch ((TTH_Option)i) {
		case TTH_INIT:
			if (objc != 2) {
				Tcl_WrongNumArgs(interp, 2, objv, NULL);
				return TCL_ERROR;
			}
			resultPtr = Tcl_GetObjResult(interp);
			if (Tcl_IsShared(resultPtr)) {
				resultPtr = Tcl_DuplicateObj(resultPtr);
			}
			TTH_CreateContext(statePtr, resultPtr);
			Tcl_SetObjResult(interp, resultPtr);
			return TCL_OK;
		break;

		case TTH_UPDATE:
			if (objc != 4) {
				Tcl_WrongNumArgs(interp, 2, objv,
						"tthContext sourceString");
				return TCL_ERROR;
			}
			/* TODO check whether the given context exists.
			 * May be do so inside TTH_UpdateContext */
			TTH_UpdateContext(statePtr, objv[2], objv[3]);
			Tcl_ResetResult(interp);
			return TCL_OK;
		break;

		case TTH_DIGEST:
			if (objc != 3) {
				Tcl_WrongNumArgs(interp, 2, objv,
						"tthContext");
				return TCL_ERROR;
			}
			/* TODO check whether the given context exists.
			 * May be do so inside TTH_UpdateContext */
			resultPtr = Tcl_GetObjResult(interp);
			if (Tcl_IsShared(resultPtr)) {
				resultPtr = Tcl_DuplicateObj(resultPtr);
			}
			TTH_GetDigest(statePtr, objv[2], resultPtr);
			Tcl_SetObjResult(interp, resultPtr);
			TTH_DeleteContext(TTH_FindContext(
						statePtr, objv[2]));
			return TCL_OK;
		break;
	}

	return TCL_OK;
}


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
	TTH_State *statePtr;
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

