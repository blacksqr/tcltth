/*
 * posix_mmap.c --
 *
 *	This file implements a Tcl procedure that calculates TTH
 *  on a given file using mmap()/munmap() system calls as defined
 *  by POSIX.
 *
 * Copyright (c) 2007 Konstantin Khomoutov <flatworm@users.sourceforge.net>
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * $Id$
 *
 */

#ifdef HAVE_MMAP

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "tclmmap.h"

/*
 *
 */
int
TTH_GetDigestUsingMmap (
		Tcl_Interp   *interp,
		Tcl_Obj      *filePtr,
		byte         digest[]
		)
{
	int fd;
	off_t offset, len;
	TT_CONTEXT context;
	byte *dataPtr;
	long pagesize;
	struct stat finfo;

	fd = open(Tcl_GetString(filePtr), O_RDONLY);
	if (fd == -1) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp, "failed to open file named \"",
				Tcl_GetString(filePtr), "\"", NULL);
		return TCL_ERROR;
	}

	tt_init(&context);

#ifdef _SC_PAGESIZE
	pagesize = sysconf(_SC_PAGESIZE);
#else
#ifdef _SC_PAGE_SIZE
	pagesize = sysconf(_SC_PAGE_SIZE);
#else
#error Neither _SC_PAGESIZE nor _SC_PAGE_SIZE is defined
#endif
#endif

	/* pagesize *= 1024; */

	if (fstat(fd, &finfo) == -1) {
		Tcl_ResetResult(interp);
		Tcl_AppendResult(interp, "failed to stat file named \"",
				Tcl_GetString(filePtr), "\"", NULL);
		close(fd);
		return TCL_ERROR;
	}

	offset = 0;
	while (offset < finfo.st_size) {
		if (finfo.st_size - offset < pagesize) {
			len = finfo.st_size - offset;
		} else {
			len = pagesize;
		}
		dataPtr = (byte *) mmap(0, len, PROT_READ, MAP_SHARED, fd, offset);
		if (dataPtr == MAP_FAILED) {
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp, "mmap() failed on file named \"",
					Tcl_GetString(filePtr), "\"", NULL);
			close(fd);
			return TCL_ERROR;
		}
		tt_update(&context, dataPtr, len);
		if (munmap(dataPtr, len) == -1) {
			Tcl_ResetResult(interp);
			Tcl_AppendResult(interp, "munmap() failed on file named \"",
					Tcl_GetString(filePtr), "\"", NULL);
			close(fd);
			return TCL_ERROR;
		}
		offset += len;
	}

	tt_digest(&context, digest);
	close(fd);

	return TCL_OK;
}

#endif /* ifdef HAVE_MMAP */

