#!/bin/sh
# \
exec tclsh "$0" "$@"

set digit [lindex $argv 0]

array set counts {}

set linecount 0
while {1} {
  set line [gets stdin]
  if {[eof stdin]} {break}
  incr linecount
  if {$linecount % 100000 == 0} {puts stderr $linecount; flush stderr}
  # if {![string match *$digit $line]} {continue}
  if {![info exists counts($line)]} {set counts($line) 0}
  incr counts($line)
}
foreach l [array names counts] {
  set c $counts($l)
  if {$c >= 2} {puts $c}
}
