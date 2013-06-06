#!/bin/sh
# \
exec tclsh "$0" "$@"

gets stdin
while {1} {
  set line [gets stdin]
  if {[eof stdin]} {break}
  if {$line == ""} {
	gets stdin
  } else {
	set line [lindex [split $line " "] 2]
	set filename "[file dirname $line]/[file tail $line]-tsv"
	puts stderr $filename
	if {[file exists $filename]} {puts $filename}
  }
}
puts stderr done
