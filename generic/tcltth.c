/*
 * tcltth.c --
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

#include <tcl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tigertree.h"
#include "base32.h"
#include "tcltth.h"

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
static void
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
static void
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
static Tcl_HashEntry *
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
static void
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
static void
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
 *
 */
static int
TTH_GetDigestFromContext (
		Tcl_Interp   *interp,
		TTH_State    *statePtr,
		Tcl_Obj      *tokenPtr,
		byte         digest[]
		)
{
	Tcl_HashEntry *entryPtr;
	TT_CONTEXT *contextPtr;
	char error[256];

	entryPtr = TTH_FindContext(statePtr, tokenPtr);
	if (entryPtr == NULL) {
		Tcl_ResetResult(interp);
		sprintf(error, "can not find context named \"%s\"",
				Tcl_GetString(tokenPtr));
		Tcl_SetResult(interp, error, TCL_VOLATILE);
		return TCL_ERROR;
	}

	contextPtr = (TT_CONTEXT *) Tcl_GetHashValue(entryPtr);
	tt_digest(contextPtr, digest);

	TTH_DeleteContext(entryPtr);

	return TCL_OK;
}


/*
 *
 */
static void
TTH_GetDigestFromString (
		Tcl_Obj *dataPtr,
		byte    digest[]
		)
{
	unsigned char *bytesPtr;
	int len;

	bytesPtr = Tcl_GetByteArrayFromObj(dataPtr, &len);
	tiger((word64 *) bytesPtr, (word64) len, (word64 *) digest);
}


/*
 *
 */
static void
TTH_DigestToBase32 (
		byte    digest[],
		Tcl_Obj *resultPtr
		)
{
	char TTX[BASE32_DESTLEN(TIGERSIZE)];

	to_base32(digest, TIGERSIZE, TTX);
	Tcl_SetStringObj(resultPtr, TTX, -1);
}


/*
 *
 */
static void
TTH_DigestToRaw (
		byte    digest[],
		Tcl_Obj *resultPtr
		)
{
	unsigned char *bytesPtr;

	bytesPtr = Tcl_SetByteArrayLength(resultPtr, TIGERSIZE);
	memcpy(bytesPtr, digest, TIGERSIZE);
}


/*
 *
 */
typedef enum {
	DM_CONTEXT,   /* -context, default */
	DM_STRING,    /* -string */
	DM_CHAN       /* -chan */
} DIGEST_MODE;

typedef enum {
	DO_BASE32,     /* -base32 or -ttx, default  */
	DO_RAW         /* -raw */
} DIGEST_OUTPUT;


/*
 *
 */
static int
Cmd_ParseDigestOptions (
		Tcl_Interp    *interp,
		Tcl_Obj       *objv[],
		int           objc,
		DIGEST_MODE   *modePtr,
		DIGEST_OUTPUT *outputPtr
		)
{
	int i, op;

	static const char *options[] = { "-context", "-string", "-chan",
		"-base32", "-ttx", "-raw", NULL };
	typedef enum { OP_CONTEXT, OP_STRING, OP_CHAN,
		OP_BASE32, OP_TTX, OP_RAW } OPTION;

	/* Options start from index 2 and the last object is always a "value": */
	const int first = 2;
	int       last  = objc - 2;

	/* Defaults */
	*modePtr   = DM_CONTEXT;
	*outputPtr = DO_BASE32;

	for (i = first; i <= last; ++i) {
		if (Tcl_GetIndexFromObj(interp, objv[i], options, "option",
				0, &op) != TCL_OK) { return TCL_ERROR; }
		switch (op) {
			case OP_CONTEXT:
				*modePtr = DM_CONTEXT;
			break;
			case OP_STRING:
				*modePtr = DM_STRING;
			break;
			case OP_CHAN:
				*modePtr = DM_CHAN;
			break;
			case OP_BASE32:
			case OP_TTX:
				*outputPtr = DO_BASE32;
			break;
			case OP_RAW:
				*outputPtr = DO_RAW;
			break;
		}
	}

	return TCL_OK;
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

int
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
	Tcl_Obj *dataPtr, *resultPtr;
	DIGEST_MODE   dmode;
	DIGEST_OUTPUT dout;
	byte digest[TIGERSIZE];

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
				resultPtr = Tcl_NewObj();
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
			if (objc < 3) {
				Tcl_WrongNumArgs(interp, 2, objv,
						"?options? source");
				return TCL_ERROR;
			}
			if (Cmd_ParseDigestOptions(interp, objv, objc,
						&dmode, &dout) != TCL_OK) { return TCL_ERROR; }
			dataPtr = objv[objc - 1];
			switch (dmode) {
				case DM_CONTEXT:
					if (TTH_GetDigestFromContext(interp, statePtr, dataPtr,
								digest) != TCL_OK) { return TCL_ERROR; }
				break;
				case DM_STRING:
					TTH_GetDigestFromString(dataPtr, digest);
				break;
				case DM_CHAN:
					Tcl_SetResult(interp, "not implemented", TCL_VOLATILE);
					return TCL_ERROR;
				break;
			}
			resultPtr = Tcl_GetObjResult(interp);
			if (Tcl_IsShared(resultPtr)) {
				resultPtr = Tcl_NewObj();
			}
			switch (dout) {
				case DO_BASE32:
					TTH_DigestToBase32(digest, resultPtr);
				break;
				case DO_RAW:
					TTH_DigestToRaw(digest, resultPtr);
				break;
			}
			/* Tcl_SetIntObj(resultPtr, (dmode << 4 | dout) & 0xFF); */
			Tcl_SetObjResult(interp, resultPtr);
			return TCL_OK;
		break;
	}

	return TCL_OK;
}


/*
 *
 */
Tcl_Command
TTH_CreateCmd (
		Tcl_Interp *interp
		)
{
	TTH_State *statePtr;

	statePtr = (TTH_State *) ckalloc(sizeof(TTH_State));
	Tcl_InitHashTable(&statePtr->contexts, TCL_STRING_KEYS);
	statePtr->uid = 0;

	return Tcl_CreateObjCommand(interp, "::tth::tth",
		(Tcl_ObjCmdProc *) TTH_Cmd,
		(ClientData) statePtr, (Tcl_CmdDeleteProc *) TTH_CleanupState);
}

