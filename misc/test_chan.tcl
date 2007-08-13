# $Id$
# Simple proc for applying "tth -chan" to a file

proc test_chan fname {
  set fd [open $fname]
  fconfigure $fd -translation binary
  set start [clock seconds]
  set hash [tth::tth digest -ttx -chan $fd]
  set intvl [expr {[clock seconds] - $start}]
  close $fd
  if {$intvl > 0} {
    puts "Rate: [expr {[file size $fname]/$intvl/1024.0/1024.0}] Mb/s"
  } else {
    puts "Instant"
  }
  set hash
}

