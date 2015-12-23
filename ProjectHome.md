# Synopsis #

A C extension for Tcl implementing Tiger Tree Hash (TTH).

# Target #

Provide Tcl extension which is able to:
  * Calculate Tiger hash on a given string of bytes;
  * Calculate TTH on a Tcl channel (a file, for example);
  * Calculate TTH (digest) sequentially on a series of data chunks (to be used with async I/O);
  * Provide an ability to present the calculated hash/digest using some "armoured" form (base32 basically as required by THEX).

The immediate target is to provide a way for calculating and presenting TTH on files as used in DirectConnect P2P applications (namely, DC++).

# Status #

Fully functional. Used in [tcldc](http://tcldc.googlecode.com).

# Missing #

  * "Stable" source release (documentation must be completed first).
  * Binary release for win32 platforms.

# Misc info #

Utilizes code from http://tigertree.sf.net