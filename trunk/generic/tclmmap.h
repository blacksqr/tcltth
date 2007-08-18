/*
 * tclmmap.h --
 *
 *	This file contains generic interface to various
 *	(platform-dependent) implementations of calculating TTH
 *	on a file using memory mapping.
 *
 * Copyright (c) 2007 Konstantin Khomoutov <flatworm@users.sourceforge.net>
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * $Id$
 *
 */

#ifndef __TCLMMAP_H
#define __TCLMMAP_H

#if defined(_WIN32) || defined(HAVE_MMAP)

#define USE_MMAP 1

#include <tcl.h>

int
TTH_GetDigestUsingMmap (
		Tcl_Interp   *interp,
		Tcl_Obj      *filePtr,
		byte         digest[]
		);

#else

#undef USE_MMAP

#endif /* defined(WINDOWS) || HAVE_MMAP */

#endif /* ifdef __TCLMMAP_H */
