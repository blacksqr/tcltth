# $Id$
# Simple proc for applying "tth -mmap" to a file

proc test_mmap fname {
  set start [clock seconds]
  set hash [tth::tth digest -thex -mmap $fname]
  set intvl [expr {[clock seconds] - $start}]
  if {$intvl > 0} {
    puts "Rate: [expr {[file size $fname]/$intvl/1024.0/1024.0}] Mb/s"
  } else {
    puts "Instant"
  }
  set hash
}

