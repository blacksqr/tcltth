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
#include "tclout.h"
#include "tclmmap.h"
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
static Tcl_Obj *
TTH_CreateContext (
		TTH_State *statePtr
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
		Tcl_Panic("TTH context \"%s\" stomps on existing one", token);
	}

	contextPtr = (TT_CONTEXT *) ckalloc(sizeof(TT_CONTEXT));
	Tcl_SetHashValue(entryPtr, (ClientData)contextPtr);
	tt_init(contextPtr);

	return Tcl_NewStringObj(token, -1);
}


/*
 *
 */
static int
TTH_FindContext (
		Tcl_Interp    *interp,
		TTH_State     *statePtr,
		Tcl_Obj       *tokenPtr,
		Tcl_HashEntry **entryPtr
		)
{
	*entryPtr = Tcl_FindHashEntry(&statePtr->contexts,
			Tcl_GetString(tokenPtr));
	if (*entryPtr == NULL) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp, "can not find context named \"",
				Tcl_GetString(tokenPtr), "\"", NULL);
		return TCL_ERROR;
	} else
		return TCL_OK;
}


/*
 *
 */
static int
TTH_UpdateContext (
		Tcl_Interp *interp,
		TTH_State  *statePtr,
		Tcl_Obj    *tokenPtr,
		Tcl_Obj    *dataPtr 
		)
{
	Tcl_HashEntry *entryPtr;
	TT_CONTEXT    *contextPtr;
	byte          *bytesPtr;
	int           len;

	if(TTH_FindContext(interp, statePtr, tokenPtr,
				&entryPtr) != TCL_OK) { return TCL_ERROR; }

	contextPtr = (TT_CONTEXT *) Tcl_GetHashValue(entryPtr);

	bytesPtr = Tcl_GetByteArrayFromObj(dataPtr, &len);
	tt_update(contextPtr, bytesPtr, len);

	return TCL_OK;
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
static void
TTH_GetDigestFromString (
		Tcl_Obj *dataPtr,
		byte    digest[]
		)
{
	TT_CONTEXT context;
	unsigned char *bytesPtr;
	int len;

	tt_init(&context);
	bytesPtr = Tcl_GetByteArrayFromObj(dataPtr, &len);
	tt_update(&context, bytesPtr, len);
	tt_digest(&context, digest);
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

	if (TTH_FindContext(interp, statePtr, tokenPtr,
				&entryPtr) != TCL_OK) { return TCL_ERROR; }

	contextPtr = (TT_CONTEXT *) Tcl_GetHashValue(entryPtr);
	tt_digest(contextPtr, digest);

	TTH_DeleteContext(entryPtr);

	return TCL_OK;
}


/*
 *
 */
static int
TTH_GetDigestFromChan (
		Tcl_Interp   *interp,
		Tcl_Obj      *chanPtr,
		byte         digest[]
		)
{
	Tcl_Channel chan;
	int mode;
	TT_CONTEXT context;
	Tcl_Obj *chunkPtr;
	byte *dataPtr;
	int len;
	const int CHUNKSIZE = 8192;

	chan = Tcl_GetChannel(interp,
			Tcl_GetString(chanPtr), &mode);
	if (chan == NULL) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp, "can not find channel named \"",
				Tcl_GetString(chanPtr), "\"", NULL);
		return TCL_ERROR;
	}
	if ((mode & TCL_READABLE) != TCL_READABLE) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp, "channel \"", Tcl_GetString(chanPtr),
				"\" is not opened for reading", NULL);
		return TCL_ERROR;
	}

	tt_init(&context);

	chunkPtr = Tcl_NewObj();
	Tcl_IncrRefCount(chunkPtr);

	while (! Tcl_Eof(chan)) {
		len = Tcl_ReadChars(chan, chunkPtr, CHUNKSIZE, 0);
		if (len == -1) {
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp, "failed to read from channel \"",
					Tcl_GetString(chanPtr), "\"", NULL);
			return TCL_ERROR;
		}
		dataPtr = Tcl_GetByteArrayFromObj(chunkPtr, &len);
		tt_update(&context, dataPtr, len);
	}

	tt_digest(&context, digest);

	Tcl_DecrRefCount(chunkPtr);

	return TCL_OK;
}


/*
 *
 */
typedef enum {
	DM_CONTEXT,   /* -context, default */
	DM_STRING,    /* -string */
	DM_CHAN,      /* -chan */
	DM_MMAP       /* -mmap */
} DIGEST_MODE;

typedef enum {
	DO_THEX,       /* -thex, default  */
	DO_HEX,        /* -hex */
	DO_RAW         /* -raw */
} DIGEST_OUTPUT;


/*
 *
 */
static int
Cmd_ParseDigestOptions (
		Tcl_Interp     *interp,
		Tcl_Obj *const objv[],
		int            objc,
		DIGEST_MODE    *modePtr,
		DIGEST_OUTPUT  *outputPtr,
		DIGEST_BITLEN  *bitlenPtr
		)
{
	int i, op;

	static const char *options[] = { "-context", "-string", "-chan",
#ifdef USE_MMAP
		"-mmap",
#endif
		"-thex", "-hex", "-raw", "-192", "-160", "-128", NULL };
	typedef enum { OP_CONTEXT, OP_STRING, OP_CHAN,
#ifdef USE_MMAP
		OP_MMAP,
#endif
		OP_THEX, OP_HEX, OP_RAW, OP_192, OP_160, OP_128 } OPTION;

	/* Options start from index 2 and the last object is always a "value": */
	const int first = 2;
	int       last  = objc - 2;

	/* Defaults */
	*modePtr   = DM_CONTEXT;
	*outputPtr = DO_THEX;
	*bitlenPtr = 192;

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
#ifdef USE_MMAP
			case OP_MMAP:
				*modePtr = DM_MMAP;
			break;
#endif
			case OP_THEX:
				*outputPtr = DO_THEX;
			break;
			case OP_HEX:
				*outputPtr = DO_HEX;
			break;
			case OP_RAW:
				*outputPtr = DO_RAW;
			break;
			case OP_192:
				*bitlenPtr = 192;
			break;
			case OP_160:
				*bitlenPtr = 160;
			break;
			case OP_128:
				*bitlenPtr = 128;
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
	Tcl_Obj *dataPtr;
	DIGEST_MODE   dmode;
	DIGEST_OUTPUT dout;
	DIGEST_BITLEN dbitlen;
	byte digest[TIGERSIZE];

	if (objc == 1) {
		Tcl_WrongNumArgs(interp, 1, objv,
				"option ?arg ...?");
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
			Tcl_SetObjResult(interp, TTH_CreateContext(statePtr));
			return TCL_OK;
		break;

		case TTH_UPDATE:
			if (objc != 4) {
				Tcl_WrongNumArgs(interp, 2, objv,
						"tthContext sourceString");
				return TCL_ERROR;
			}
			if (TTH_UpdateContext(interp, statePtr, objv[2],
						objv[3]) != TCL_OK) { return TCL_ERROR; }
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
						&dmode, &dout, &dbitlen) != TCL_OK) { return TCL_ERROR; }
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
					if (TTH_GetDigestFromChan(interp, dataPtr,
								digest) != TCL_OK) { return TCL_ERROR; }
				break;
#ifdef USE_MMAP
				case DM_MMAP:
					if (TTH_GetDigestUsingMmap(interp, dataPtr,
								digest) != TCL_OK) { return TCL_ERROR; }
				break;
#endif
			}
			switch (dout) {
				case DO_THEX:
					Tcl_SetObjResult(interp,
							DigestToTHEX(digest, dbitlen));
				break;
				case DO_HEX:
					Tcl_SetObjResult(interp,
							DigestToHex(digest, dbitlen));
				break;
				case DO_RAW:
					Tcl_SetObjResult(interp,
							DigestToRaw(digest, dbitlen));
				break;
			}
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

