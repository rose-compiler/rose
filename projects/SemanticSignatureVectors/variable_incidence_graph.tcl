#!/bin/sh
# \
exec tclsh "$0" "$@"

set c 0
while 1 {
  set line [string trim [gets stdin]]
  if {[eof stdin]} {break}
  if {[string match "p cnf *" $line]} {continue}
  incr c
  if {$c % 100000 == 0} {puts stderr "Line $c"}
  regsub -all {[-]} $line "" line
  set line [split $line " "]
  foreach a $line {
    if {$a == 0} {continue}
    foreach b $line {
      if {$b == 0} {continue}
      if {$a <= $b} {continue}
      lappend outedges($a) $b
      lappend outedges($b) $a
      incr "edges($a -- $b)"
    }
  }
}

puts "graph variable_incidence {"
foreach p [array names edges] {puts "$p \[label=\"$edges($p)\"\]"}
puts "}"

foreach v [array names outedges] {
  puts stderr "$v has degree [llength [lsort -unique $outedges($v)]]"
}
