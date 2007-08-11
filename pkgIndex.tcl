#
# Tcl package index file
#
package ifneeded tcltth 0.1 \
		[string map [list \$dir $dir] {
    load [file join $dir libtcltth0.1.so] tcltth
	namespace eval ::tth { namespace export tth }
}]

