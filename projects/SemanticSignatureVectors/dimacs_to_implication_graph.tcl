#!/bin/sh
# \
exec tclsh "$0" "$@"

set data [split [read stdin] "\n"]
set i 0
while {[string match "c *" [lindex $data $i]]} {
  incr i
}
set data [lrange $data $i end]
set header [split [lindex $data 0] " "]
if {[lindex $header 0] == "p"} {
  set data [lrange $data 1 end]
  set nvars [lindex $header 2]
  set nclauses [lindex $header 3]
} else {
  set nvars unknown
  set nclauses unknown
}

puts "digraph implicationgraph {"
array set usedVars {}
foreach line $data {
  if {$line == ""} {continue}
  set clause [split $line " "]
  if {[lindex $clause end] != 0} {error "Bad clause $line"}
  set clause [lrange $clause 0 end-1]
  if {[llength $clause] >= 3} {continue}
  foreach lit $clause {
    set usedVars([expr {abs($lit)}]) ""
  }
  switch -exact [llength $clause] {
    0 {error "Empty clause"}
    1 {
      set l0 [lindex $clause 0]
      puts "[expr {-$l0}] \[color=red\]"
      puts "$l0 \[color=green\]"
    }
    2 {
      set l0 [lindex $clause 0]
      set l1 [lindex $clause 1]
      puts "[expr {-$l0}] -> $l1"
      # puts "$l0 -> [expr {-$l1}] \[arrowhead=none, arrowtail=normal\]"
      puts "[expr {-$l1}] -> $l0"
    }
    default {
    }
  }
}
if {0} {
  foreach v [array names usedVars] {
    puts "$v -> [expr {-$v}] \[arrowhead=none, color=blue\]"
  }
}
puts "}"
puts stderr "Used variables: [llength [array names usedVars]] of $nvars"
