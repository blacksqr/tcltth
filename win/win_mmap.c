/*
 * win_mmap.c --
 *
 *	This file implements a Tcl command that calculates TTH
 *	on a file using Win32 API calls.
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
#define WINDOWS_LEAN_AND_MEAN
#include <windows.h>

#include "tiger.h"
#include "tigertree.h"

int
TTH_GetDigestUsingMmap (
		Tcl_Interp   *interp,
		Tcl_Obj      *filePtr,
		byte         digest[]
		)
{
	SYSTEM_INFO sysinfo;
	TT_CONTEXT context;
	CONST TCHAR *nativeName;
	HANDLE hFile, hMap;
	DWORD fsizeLow, fsizeHigh;
	word64 fsize, offset;
	DWORD pagesize, len;
	byte *dataPtr;

	nativeName = (TCHAR*) Tcl_FSGetNativePath(filePtr);
	if (nativeName == NULL) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp, "failed to get native name for file named \"",
				Tcl_GetString(filePtr), "\"", NULL);
		return TCL_ERROR;
	}

	hFile = CreateFile(nativeName, GENERIC_READ, FILE_SHARE_READ, NULL,
			OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp, "failed to open file named \"",
				Tcl_GetString(filePtr), "\"", NULL);
		/* TODO provide error message */
		return TCL_ERROR;
	}

	fsizeLow = GetFileSize(hFile, &fsizeHigh);
	if (fsizeLow == 0xFFFFFFFF && GetLastError() != NO_ERROR) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp, "failed to get file size", NULL);
		CloseHandle(hFile);
		return TCL_ERROR;
	}
	fsize = ((word64)fsizeHigh << 32) | (word64)fsizeLow;

	hMap = CreateFileMapping(hFile, NULL, PAGE_READONLY,
			fsizeLow, fsizeHigh, NULL);
	if (hMap == NULL) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp, "failed to create file mapping", NULL);
		CloseHandle(hFile);
		return TCL_ERROR;
	}

	GetSystemInfo(&sysinfo);
	pagesize = sysinfo.dwPageSize * 1024;

	tt_init(&context);

	offset = 0;
	while (offset < fsize) {
		if (fsize - offset < pagesize) {
			len = fsize - offset;
		} else {
			len = pagesize;
		}
		dataPtr = MapViewOfFile(hMap, FILE_MAP_READ, 
				(offset >> 32), offset & 0xFFFFFFFF, len);
		if (dataPtr == NULL) {
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp, "failed to map view of file", NULL);
			CloseHandle(hMap);
			CloseHandle(hFile);
			return TCL_ERROR;
		}
		tt_update(&context, dataPtr, len);
		if (UnmapViewOfFile(dataPtr) == 0) {
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp, "failed to unmap view of file", NULL);
			CloseHandle(hMap);
			CloseHandle(hFile);
			return TCL_ERROR;
		}
		offset += len;
	}

	tt_digest(&context, digest);

	CloseHandle(hMap);
	CloseHandle(hFile);

	return TCL_OK;
}

