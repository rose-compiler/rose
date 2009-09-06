#!/bin/sh
# \
exec tclsh "$0" "$@"

foreach line [split [read stdin] "\n"] {
  if {[string match {p *} $line]} {continue}
  if {[string match {c *} $line]} {
    foreach lit [lrange [split $line " "] 3 end] {
      incr intUses([expr {abs($lit)}])
    }
    continue
  }
  foreach lit [split $line " "] {
    if {$lit == 0} {continue}
    incr uses($lit)
  }
}

foreach lit [concat [array names uses] [array names intUses]] {
  set var [expr {abs($lit)}]
  set vars($var) ""
  incr uses($var) 0
  incr uses(-$var) 0
  incr intUses($var) 0
}

foreach var [lsort -integer [array names vars]] {
  puts "$var: $uses($var) $uses(-$var) $intUses($var)"
}
