# $Id$
# Hashes a directory, with timings. Non-recursive.

proc hashem dir {
  set out [list]
  foreach f [glob -dir $dir *] {
    set fd [open $f]
    fconfigure $fd -translation binary
    set start [clock seconds]
    set hash [tth::tth digest -chan $fd]
    lappend out $f $hash [expr {[file size $f]/([clock seconds] - $start)/1024.0/1024.0}]
    close $fd
  }
  set out
}

