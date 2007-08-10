# Makefile.in --
#
#	This file is a Makefile for Sample TEA Extension.  If it has the name
#	"Makefile.in" then it is a template for a Makefile;  to generate the
#	actual Makefile, run "./configure", which is a configuration script
#	generated by the "autoconf" program (constructs like "@foo@" will get
#	replaced in the actual Makefile.
#
# Copyright (c) 1999 Scriptics Corporation.
# Copyright (c) 2002-2005 ActiveState Corporation.
#
# See the file "license.terms" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
#
# RCS: @(#) $Id: Makefile.in 2 2007-08-08 00:32:26Z khomoutov $

#========================================================================
# Add additional lines to handle any additional AC_SUBST cases that
# have been added in a customized configure script.
#========================================================================

#SAMPLE_NEW_VAR	= @SAMPLE_NEW_VAR@

#========================================================================
# Nothing of the variables below this line should need to be changed.
# Please check the TARGETS section below to make sure the make targets
# are correct.
#========================================================================

#========================================================================
# The names of the source files is defined in the configure script.
# The object files are used for linking into the final library.
# This will be used when a dist target is added to the Makefile.
# It is not important to specify the directory, as long as it is the
# $(srcdir) or in the generic, win or unix subdirectory.
#========================================================================

PKG_SOURCES	=  tiger.c tigertree.c base32.c tclcmd.c
PKG_OBJECTS	=  tiger.o tigertree.o base32.o tclcmd.o

PKG_STUB_SOURCES = 
PKG_STUB_OBJECTS = 

#========================================================================
# PKG_TCL_SOURCES identifies Tcl runtime files that are associated with
# this package that need to be installed, if any.
#========================================================================

PKG_TCL_SOURCES = 

#========================================================================
# This is a list of public header files to be installed, if any.
#========================================================================

PKG_HEADERS	= 

#========================================================================
# "PKG_LIB_FILE" refers to the library (dynamic or static as per
# configuration options) composed of the named objects.
#========================================================================

PKG_LIB_FILE	= tcltth01.dll
PKG_STUB_LIB_FILE = libtcltthstub01.a

lib_BINARIES	= $(PKG_LIB_FILE)
BINARIES	= $(lib_BINARIES)

SHELL		= /bin/sh

srcdir		= .
prefix		= C:/Tcl
exec_prefix	= C:/Tcl

bindir		= ${exec_prefix}/bin
libdir		= ${exec_prefix}/lib
includedir	= ${prefix}/include
datarootdir     = ${prefix}/share
datadir		= ${datarootdir}
mandir		= ${datarootdir}/man

DESTDIR		=

PKG_DIR		= $(PACKAGE_NAME)$(PACKAGE_VERSION)
pkgdatadir	= $(datadir)/$(PKG_DIR)
pkglibdir	= $(libdir)/$(PKG_DIR)
pkgincludedir	= $(includedir)/$(PKG_DIR)

top_builddir	= .

INSTALL		= /bin/install -c
INSTALL_PROGRAM	= ${INSTALL}
INSTALL_DATA	= ${INSTALL} -m 644
INSTALL_SCRIPT	= ${INSTALL}

PACKAGE_NAME	= tcltth
PACKAGE_VERSION	= 0.1
CC		= gcc
CFLAGS_DEFAULT	= -O2 -fomit-frame-pointer
CFLAGS_WARNING	= -Wall -Wno-implicit-int
EXEEXT		= .exe
LDFLAGS_DEFAULT	= 
MAKE_LIB	= ${SHLIB_LD} -o $@ $(PKG_OBJECTS) ${SHLIB_LD_LIBS} 
MAKE_SHARED_LIB	= ${SHLIB_LD} -o $@ $(PKG_OBJECTS) ${SHLIB_LD_LIBS}
MAKE_STATIC_LIB	= ${STLIB_LD} $@ $(PKG_OBJECTS)
MAKE_STUB_LIB	= ${STLIB_LD} $@ $(PKG_STUB_OBJECTS)
OBJEXT		= o
RANLIB		= :
RANLIB_STUB	= :
SHLIB_CFLAGS	= 
SHLIB_LD	= gcc -shared
SHLIB_LD_LIBS	= ${LIBS} "/c/Tcl/lib/tclstub84.lib"
STLIB_LD	= ${AR} cr
#TCL_DEFS	=  -DHAVE_NO_LPFN_DECLS=1 -DTCL_THREADS=1 -DUSE_THREAD_ALLOC=1 
TCL_BIN_DIR	= /c/Tcl/lib
TCL_SRC_DIR	= /home/Administrator/dbn/lba/night/builds/win32-ix86/tcl
#TK_BIN_DIR	= @TK_BIN_DIR@
#TK_SRC_DIR	= @TK_SRC_DIR@

# Not used, but retained for reference of what libs Tcl required
#TCL_LIBS	= user32.lib advapi32.lib

#========================================================================
# TCLLIBPATH seeds the auto_path in Tcl's init.tcl so we can test our
# package without installing.  The other environment variables allow us
# to test against an uninstalled Tcl.  Add special env vars that you
# require for testing here (like TCLX_LIBRARY).
#========================================================================

EXTRA_PATH	= $(top_builddir):$(TCL_BIN_DIR)
#EXTRA_PATH	= $(top_builddir):$(TCL_BIN_DIR):$(TK_BIN_DIR)
TCLLIBPATH	= $(top_builddir)
TCLSH_ENV	= TCL_LIBRARY=`echo $(TCL_SRC_DIR)/library` \
		  LD_LIBRARY_PATH="$(EXTRA_PATH):$(LD_LIBRARY_PATH)" \
		  PATH="$(EXTRA_PATH):$(PATH)" \
		  TCLLIBPATH="$(TCLLIBPATH)"
#		  TK_LIBRARY=`echo $(TK_SRC_DIR)/library`

TCLSH_PROG	= /c/Tcl/bin/tclsh84.exe
TCLSH   	= $(TCLSH_ENV) $(TCLSH_PROG)

#WISH_PROG	= @WISH_PROG@
#WISH   	= $(TCLSH_ENV) $(WISH_PROG)


SHARED_BUILD	= 1

INCLUDES	=  -I"C:/Tcl/include"
#INCLUDES	=  -I"C:/Tcl/include" @TK_INCLUDES@ @TK_XINCLUDES@

PKG_CFLAGS	=  

# TCL_DEFS is not strictly need here, but if you remove it, then you
# must make sure that configure.in checks for the necessary components
# that your library may use.  TCL_DEFS can actually be a problem if
# you do not compile with a similar machine setup as the Tcl core was
# compiled with.
#DEFS		= $(TCL_DEFS) -DPACKAGE_NAME=\"tcltth\" -DPACKAGE_TARNAME=\"tcltth\" -DPACKAGE_VERSION=\"0.1\" -DPACKAGE_STRING=\"tcltth\ 0.1\" -DPACKAGE_BUGREPORT=\"\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DBUILD_sample=1 -DTCL_THREADS=1 -DTCL_WIDE_INT_TYPE=long\ long -DHAVE_LSEEK64=1 -DUSE_TCL_STUBS=1 $(PKG_CFLAGS)
DEFS		= -DPACKAGE_NAME=\"tcltth\" -DPACKAGE_TARNAME=\"tcltth\" -DPACKAGE_VERSION=\"0.1\" -DPACKAGE_STRING=\"tcltth\ 0.1\" -DPACKAGE_BUGREPORT=\"\" -DSTDC_HEADERS=1 -DHAVE_SYS_TYPES_H=1 -DHAVE_SYS_STAT_H=1 -DHAVE_STDLIB_H=1 -DHAVE_STRING_H=1 -DHAVE_MEMORY_H=1 -DHAVE_STRINGS_H=1 -DHAVE_INTTYPES_H=1 -DHAVE_STDINT_H=1 -DHAVE_UNISTD_H=1 -DBUILD_sample=1 -DTCL_THREADS=1 -DTCL_WIDE_INT_TYPE=long\ long -DHAVE_LSEEK64=1 -DUSE_TCL_STUBS=1 $(PKG_CFLAGS)

# Move pkgIndex.tcl to 'BINARIES' var if it is generated in the Makefile
CONFIG_CLEAN_FILES = Makefile pkgIndex.tcl
CLEANFILES	=  *.lib *.dll *.exp *.ilk *.pdb vc*.pch

CPPFLAGS	= 
LIBS		=  
AR		= ar
CFLAGS		=  -pipe ${CFLAGS_DEFAULT} ${CFLAGS_WARNING} ${SHLIB_CFLAGS} 
COMPILE		= $(CC) $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(AM_CFLAGS) $(CFLAGS)

#========================================================================
# Start of user-definable TARGETS section
#========================================================================

#========================================================================
# TEA TARGETS.  Please note that the "libraries:" target refers to platform
# independent files, and the "binaries:" target inclues executable programs and
# platform-dependent libraries.  Modify these targets so that they install
# the various pieces of your package.  The make and install rules
# for the BINARIES that you specified above have already been done.
#========================================================================

all: binaries libraries doc

#========================================================================
# The binaries target builds executable programs, Windows .dll's, unix
# shared/static libraries, and any other platform-dependent files.
# The list of targets to build for "binaries:" is specified at the top
# of the Makefile, in the "BINARIES" variable.
#========================================================================

binaries: $(BINARIES)

libraries:

#========================================================================
# Your doc target should differentiate from doc builds (by the developer)
# and doc installs (see install-doc), which just install the docs on the
# end user machine when building from source.
#========================================================================

doc:
	@echo "If you have documentation to create, place the commands to"
	@echo "build the docs in the 'doc:' target.  For example:"
	@echo "        xml2nroff sample.xml > sample.n"
	@echo "        xml2html sample.xml > sample.html"

install: all install-binaries install-libraries install-doc

install-binaries: binaries install-lib-binaries install-bin-binaries

#========================================================================
# This rule installs platform-independent files, such as header files.
# The list=...; for p in $$list handles the empty list case x-platform.
#========================================================================

install-libraries: libraries
	@mkdir -p $(DESTDIR)$(includedir)
	@echo "Installing header files in $(DESTDIR)$(includedir)"
	@list='$(PKG_HEADERS)'; for i in $$list; do \
	    echo "Installing $(srcdir)/$$i" ; \
	    $(INSTALL_DATA) $(srcdir)/$$i $(DESTDIR)$(includedir) ; \
	done;

#========================================================================
# Install documentation.  Unix manpages should go in the $(mandir)
# directory.
#========================================================================

install-doc: doc
	@mkdir -p $(DESTDIR)$(mandir)/mann
	@echo "Installing documentation in $(DESTDIR)$(mandir)"
	@list='$(srcdir)/doc/*.n'; for i in $$list; do \
	    echo "Installing $$i"; \
	    rm -f $(DESTDIR)$(mandir)/mann/`basename $$i`; \
	    $(INSTALL_DATA) $$i $(DESTDIR)$(mandir)/mann ; \
	done

test: binaries libraries
	$(TCLSH) `echo $(srcdir)/tests/all.tcl` $(TESTFLAGS)

shell: binaries libraries
	@$(TCLSH) $(SCRIPT)

gdb:
	$(TCLSH_ENV) gdb $(TCLSH_PROG) $(SCRIPT)

depend:

#========================================================================
# $(PKG_LIB_FILE) should be listed as part of the BINARIES variable
# mentioned above.  That will ensure that this target is built when you
# run "make binaries".
#
# The $(PKG_OBJECTS) objects are created and linked into the final
# library.  In most cases these object files will correspond to the
# source files above.
#========================================================================

$(PKG_LIB_FILE): $(PKG_OBJECTS)
	-rm -f $(PKG_LIB_FILE)
	${MAKE_LIB}
	$(RANLIB) $(PKG_LIB_FILE)

$(PKG_STUB_LIB_FILE): $(PKG_STUB_OBJECTS)
	-rm -f $(PKG_STUB_LIB_FILE)
	${MAKE_STUB_LIB}
	$(RANLIB_STUB) $(PKG_STUB_LIB_FILE)

#========================================================================
# We need to enumerate the list of .c to .o lines here.
#
# In the following lines, $(srcdir) refers to the toplevel directory
# containing your extension.  If your sources are in a subdirectory,
# you will have to modify the paths to reflect this:
#
# sample.$(OBJEXT): $(srcdir)/generic/sample.c
# 	$(COMPILE) -c `echo $(srcdir)/generic/sample.c` -o $@
#
# Setting the VPATH variable to a list of paths will cause the makefile
# to look into these paths when resolving .c to .obj dependencies.
# As necessary, add $(srcdir):$(srcdir)/compat:....
#========================================================================

VPATH = $(srcdir)/generic:$(srcdir)/unix:$(srcdir)/win

.c.o:
	$(COMPILE) -c `echo $<` -o $@

#========================================================================
# Distribution creation
# You may need to tweak this target to make it work correctly.
#========================================================================

#COMPRESS	= tar cvf $(PKG_DIR).tar $(PKG_DIR); compress $(PKG_DIR).tar
COMPRESS	= gtar zcvf $(PKG_DIR).tar.gz $(PKG_DIR)
DIST_ROOT	= /tmp/dist
DIST_DIR	= $(DIST_ROOT)/$(PKG_DIR)

dist-clean:
	rm -rf $(DIST_DIR) $(DIST_ROOT)/$(PKG_DIR).tar.*

dist: dist-clean
	mkdir -p $(DIST_DIR)
	cp -p $(srcdir)/ChangeLog $(srcdir)/README* $(srcdir)/license* \
		$(srcdir)/aclocal.m4 $(srcdir)/configure $(srcdir)/*.in \
		$(DIST_DIR)/
	chmod 664 $(DIST_DIR)/Makefile.in $(DIST_DIR)/aclocal.m4
	chmod 775 $(DIST_DIR)/configure $(DIST_DIR)/configure.in

	for i in $(srcdir)/*.[ch]; do \
	    if [ -f $$i ]; then \
		cp -p $$i $(DIST_DIR)/ ; \
	    fi; \
	done;

	mkdir $(DIST_DIR)/tclconfig
	cp $(srcdir)/tclconfig/install-sh $(srcdir)/tclconfig/tcl.m4 \
		$(DIST_DIR)/tclconfig/
	chmod 664 $(DIST_DIR)/tclconfig/tcl.m4
	chmod +x $(DIST_DIR)/tclconfig/install-sh

	list='demos doc generic library mac tests unix win'; \
	for p in $$list; do \
	    if test -d $(srcdir)/$$p ; then \
		mkdir $(DIST_DIR)/$$p; \
		cp -p $(srcdir)/$$p/*.* $(DIST_DIR)/$$p/; \
	    fi; \
	done

	(cd $(DIST_ROOT); $(COMPRESS);)

#========================================================================
# End of user-definable section
#========================================================================

#========================================================================
# Don't modify the file to clean here.  Instead, set the "CLEANFILES"
# variable in configure.in
#========================================================================

clean:  
	-test -z "$(BINARIES)" || rm -f $(BINARIES)
	-rm -f *.$(OBJEXT) core *.core
	-test -z "$(CLEANFILES)" || rm -f $(CLEANFILES)

distclean: clean
	-rm -f *.tab.c
	-rm -f $(CONFIG_CLEAN_FILES)
	-rm -f config.cache config.log config.status

#========================================================================
# Install binary object libraries.  On Windows this includes both .dll and
# .lib files.  Because the .lib files are not explicitly listed anywhere,
# we need to deduce their existence from the .dll file of the same name.
# Library files go into the lib directory.
# In addition, this will generate the pkgIndex.tcl
# file in the install location (assuming it can find a usable tclsh shell)
#
# You should not have to modify this target.
#========================================================================

install-lib-binaries: binaries
	@mkdir -p $(DESTDIR)$(pkglibdir)
	@list='$(lib_BINARIES)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo " $(INSTALL_PROGRAM) $$p $(DESTDIR)$(pkglibdir)/$$p"; \
	    $(INSTALL_PROGRAM) $$p $(DESTDIR)$(pkglibdir)/$$p; \
	    stub=`echo $$p|sed -e "s/.*\(stub\).*/\1/"`; \
	    if test "x$$stub" = "xstub"; then \
		echo " $(RANLIB_STUB) $(DESTDIR)$(pkglibdir)/$$p"; \
		$(RANLIB_STUB) $(DESTDIR)$(pkglibdir)/$$p; \
	    else \
		echo " $(RANLIB) $(DESTDIR)$(pkglibdir)/$$p"; \
		$(RANLIB) $(DESTDIR)$(pkglibdir)/$$p; \
	    fi; \
	    ext=`echo $$p|sed -e "s/.*\.//"`; \
	    if test "x$$ext" = "xdll"; then \
		lib=`basename $$p|sed -e 's/.[^.]*$$//'`.lib; \
		if test -f $$lib; then \
		    echo " $(INSTALL_DATA) $$lib $(DESTDIR)$(pkglibdir)/$$lib"; \
	            $(INSTALL_DATA) $$lib $(DESTDIR)$(pkglibdir)/$$lib; \
		fi; \
	    fi; \
	  fi; \
	done
	@list='$(PKG_TCL_SOURCES)'; for p in $$list; do \
	  if test -f $(srcdir)/$$p; then \
	    destp=`basename $$p`; \
	    echo " Install $$destp $(DESTDIR)$(pkglibdir)/$$destp"; \
	    $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(pkglibdir)/$$destp; \
	  fi; \
	done
	@if test "x$(SHARED_BUILD)" = "x1"; then \
	    echo " Install pkgIndex.tcl $(DESTDIR)$(pkglibdir)"; \
	    $(INSTALL_DATA) pkgIndex.tcl $(DESTDIR)$(pkglibdir); \
	fi

#========================================================================
# Install binary executables (e.g. .exe files and dependent .dll files)
# This is for files that must go in the bin directory (located next to
# wish and tclsh), like dependent .dll files on Windows.
#
# You should not have to modify this target, except to define bin_BINARIES
# above if necessary.
#========================================================================

install-bin-binaries: binaries
	@mkdir -p $(DESTDIR)$(bindir)
	@list='$(bin_BINARIES)'; for p in $$list; do \
	  if test -f $$p; then \
	    echo " $(INSTALL_PROGRAM) $$p $(DESTDIR)$(bindir)/$$p"; \
	    $(INSTALL_PROGRAM) $$p $(DESTDIR)$(bindir)/$$p; \
	  fi; \
	done

.SUFFIXES: .c .$(OBJEXT)

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status
	cd $(top_builddir) \
	  && CONFIG_FILES=$@ CONFIG_HEADERS= $(SHELL) ./config.status

uninstall-binaries:
	list='$(lib_BINARIES)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(pkglibdir)/$$p; \
	done
	list='$(PKG_TCL_SOURCES)'; for p in $$list; do \
	  p=`basename $$p`; \
	  rm -f $(DESTDIR)$(pkglibdir)/$$p; \
	done
	list='$(bin_BINARIES)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(bindir)/$$p; \
	done

.PHONY: all binaries clean depend distclean doc install libraries test

# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
