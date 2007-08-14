#
# Tcl package index file
#
package ifneeded tth 0.1 \
		[string map [list \$dir $dir] {
    load [file join $dir tth01.dll] tth
	namespace eval ::tth { namespace export tiger tth }
}]

