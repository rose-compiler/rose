#!/bin/sh
# \
exec tclsh "$0" "$@"

foreach file $argv {
  puts stderr $file
  set f [open $file r]
  catch {unset vects}
  set vector_count 0
  while {1} {
    set line [gets $f]
    if {[eof $f] && $line == ""} {break}
    if {[string match #* $line]} {continue}
    if {![info exists vects($line)]} {set vects($line) 0}
    incr vects($line)
    incr vector_count
  }
  close $f
  set num_buckets 0
  set num_elts_in_buckets 0
  foreach v [array names vects] {
    if {$vects($v) >= 2} {
      incr num_buckets
      incr num_elts_in_buckets $vects($v)
    }
  }
  puts "clones-${file}:$num_buckets buckets, $num_elts_in_buckets points (out of ${vector_count}, [format "%1.2f" [expr {$num_elts_in_buckets * 100.0 / $vector_count}]] %) in them"
}
