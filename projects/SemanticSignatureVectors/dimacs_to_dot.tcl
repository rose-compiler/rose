#!/bin/sh
# \
exec tclsh "$0" "$@"

set data [split [read stdin] "\n"]
set header [split [lindex $data 0] " "]
if {[lindex $header 0] == "p"} {
  set data [lrange $data 1 end]
  set nvars [lindex $header 2]
  set nclauses [lindex $header 3]
} else {
  set nvars unknown
  set nclauses unknown
}

puts "digraph clausegraph {"
array set usedVars {}
foreach line $data {
  if {$line == ""} {continue}
  set clause [split $line " "]
  if {[lindex $clause end] != 0} {error "Bad clause $line"}
  set clause [lrange $clause 0 end-1]
  foreach lit $clause {
    set usedVars([expr {abs($lit)}]) ""
  }
  switch -exact [llength $clause] {
    0 {error "Empty clause"}
    1 {
      set l0 [lindex $clause 0]
      if {$l0 < 0} {
        puts "[expr {-$l0}] \[color=red\]"
      } else {
        puts "$l0 \[color=green\]"
      }
    }
    2 {
      set l0 [lindex $clause 0]
      set l1 [lindex $clause 1]
      set v0 [expr {abs($l0)}]
      set v1 [expr {abs($l1)}]
      set s0 [expr {$l0 < 0 ? "-" : "+"}]
      set s1 [expr {$l1 < 0 ? "-" : "+"}]
      switch -exact -- "$s0 $s1" {
        "- -" {puts "$v0 -> $v1 \[color=red, arrowhead=none\]"}
        "+ +" {puts "$v0 -> $v1 \[color=black, arrowhead=none\]"}
        "- +" {puts "$v0 -> $v1 \[color=green\]"}
        "+ -" {puts "$v1 -> $v0 \[color=green\]"}
      }
    }
    default {
      puts "\"$clause\" \[shape=rect, label=\"\", width=.1, height=.1\]"
      foreach lit $clause {
        puts "[expr {abs($lit)}] -> \"$clause\" \[arrowhead=none, color=[expr {$lit > 0 ? "palegreen" : "pink"}]\]"
      }
    }
  }
}
puts "}"
puts stderr "Used variables: [llength [array names usedVars]] of $nvars"
