set oldNodesFile [lindex $argv 0]
set tempDotFile [lindex $argv 1]
set outputFile [lindex $argv 2]

proc readFile {filename} {
  set f [open $filename r]
  set data [read $f]
  close $f
  return $data
}

proc isNodeLine {line} {
  return [expr {[regexp {".*"} $line] && ![regexp "^digraph" $line] && ![regexp " -> " $line]}]
}

set ctr 0

proc writeClusterSpacing {output} {
  global ctr
  for {set i 0} {$i < 20} {incr i} {
    puts $output "subgraph cluster_space$ctr \{ style=invis"
    incr ctr;
  }
}

proc writeClusterSpacingEnd {output} {
  for {set i 0} {$i < 20} {incr i} {
    puts $output "\}"
  }
}

set oldNodes [split [lindex [split [readFile $oldNodesFile] "\n"] 0] " "]

array set oldNodeArray {}
foreach n $oldNodes {
  set hexVersion [format "0x%x" $n]
  set oldNodeArray($hexVersion) ""
}

set dotFile [split [readFile $tempDotFile] "\n"]

set output [open $outputFile w]

puts $output "digraph copyGraph {"
writeClusterSpacing $output
puts $output "subgraph clusterOld {"
puts $output "label=\"Old nodes\"; fillcolor=lightgreen; style=filled; fontsize=36;"
foreach line $dotFile {
  if {[isNodeLine $line]} {
    regexp {^"([^_"]*)".*} $line _ nodename
    if {[info exists oldNodeArray($nodename)]} {
      puts $output $line
    }
  }
}
puts $output "}"
writeClusterSpacingEnd $output
writeClusterSpacing $output
puts $output "subgraph clusterNew {"
puts $output "label=\"New nodes\"; fillcolor=lightyellow; style=filled; fontsize=36;"
foreach line $dotFile {
  if {[isNodeLine $line]} {
    regexp {^"([^_"]*)".*} $line _ nodename
    if {![info exists oldNodeArray($nodename)]} {
      puts $output $line
    }
  }
}
puts $output "}"
writeClusterSpacingEnd $output
foreach line $dotFile {
  if {[regexp " -> " $line]} {
    puts $output $line
  }
}
puts $output "}"

close $output
