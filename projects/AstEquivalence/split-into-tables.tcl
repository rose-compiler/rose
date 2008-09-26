#!/bin/sh
# \
exec tclsh "$0" "$@"

proc getSqlFields {text} {
  # FIXME: does not handled escaped single quotes in fields
  set result {}
  while {[string trim $text] != ""} {
    if {[regexp {^ *([^',]+)(?:,(.*))?$} $text _ firstField rest]} {
      lappend result $firstField
      set text $rest
    } elseif {[regexp {^ *'((?:[^']|'')*)'(?:,(.*))?$} $text _ firstField rest]} {
      regsub -all "''" $firstField "'" firstField
      lappend result $firstField
      set text $rest
    } else {
      error "Bad parameter list format \"\"\"$text\"\"\""
    }
  }
  return $result
}

array set fileStreams {}

proc getStreamForTableFile {filename} {
  global fileStreams newdir
  if {[info exists fileStreams($filename)]} {
    return $fileStreams($filename)
  } else {
    set stream [open "$newdir/$filename.tsv" w]
    set fileStreams($filename) $stream
    return $stream
  }
}

proc closeAllTableFiles {} {
  global fileStreams
  foreach {filename} [array names fileStreams] {
    close $fileStreams($filename)
  }
  unset fileStreams
  array set fileStreams {}
}

# metainformation not used

proc {process__modules(name, md5, sha1, comment)} {name md5 sha1 comment} { # FIXME: Import time field is in the wrong format
  global modules_counter
  if {![info exists modules_counter]} {set modules_counter 1}
  puts [getStreamForTableFile modules] [join [list $modules_counter $name $md5 $sha1 $comment [clock format [clock seconds]]] "\t"]
  incr modules_counter
}

proc {process__functions_1(address, name, function_type)} {address name function_type} {
  puts [getStreamForTableFile functions_1] [join [list $address $name $function_type NULL] "\t"]
}

proc {process__functions_1(address, name, function_type, name_md5)} {address name function_type name_md5} {
  puts [getStreamForTableFile functions_1] [join [list $address $name $function_type $name_md5] "\t"]
}

proc {process__basic_blocks_1(id, address, parent_function)} {id address parent_function} {
  puts [getStreamForTableFile basic_blocks_1] [join [list $id $parent_function $address] "\t"]
}

proc {process__instructions_1(address, mnemonic, basic_block_id, sequence, data)} {address mnemonic basic_block_id sequence data} {
  puts [getStreamForTableFile instructions_1] [join [list $address $basic_block_id $mnemonic $sequence $data] "\t"]
}

proc {process__callgraph_1(src, src_basic_block_id, dst, src_address)} {src src_basic_block_id dst src_address} {
  global callgraph__counter
  if {![info exists callgraph__counter]} {set callgraph__counter 1}
  puts [getStreamForTableFile callgraph_1] [join [list $callgraph__counter $src $src_basic_block_id $src_address $dst] "\t"]
  incr callgraph__counter
}

proc {process__control_flow_graph_1(parent_function, src, dst, kind)} {parent_function src dst kind} {
  global control_flow_graph__counter
  if {![info exists control_flow_graph__counter]} {set control_flow_graph__counter 1}
  puts [getStreamForTableFile control_flow_graph_1] [join [list $control_flow_graph__counter $parent_function $src $dst $kind] "\t"]
  incr control_flow_graph__counter
}

proc {process__operand_strings_1(str)} {str} {
  global operand_strings__counter
  if {![info exists operand_strings__counter]} {set operand_strings__counter 1}
  puts [getStreamForTableFile operand_strings_1] [join [list $operand_strings__counter $str] "\t"]
  incr operand_strings__counter
}

proc {process__expression_tree_1(parent_id, position, expr_type, symbol)} {parent_id position expr_type symbol} {
  global expression_tree__counter
  if {![info exists expression_tree__counter]} {set expression_tree__counter 1}
  puts [getStreamForTableFile expression_tree_1] [join [list $expression_tree__counter $expr_type $symbol NULL $position $parent_id] "\t"]
  incr expression_tree__counter
}

proc {process__expression_tree_1(parent_id, position, expr_type, immediate)} {parent_id position expr_type immediate} {
  global expression_tree__counter
  if {![info exists expression_tree__counter]} {set expression_tree__counter 1}
  puts [getStreamForTableFile expression_tree_1] [join [list $expression_tree__counter $expr_type NULL $immediate $position $parent_id] "\t"]
  incr expression_tree__counter
}

proc {process__operand_tuples_1(address, operand_id, position)} {address operand_id position} {
  puts [getStreamForTableFile operand_tuples_1] [join [list $address $operand_id $position] "\t"]
}

proc {process__expression_substitutions_1(address, operand_id, expr_id, replacement)} {address operand_id expr_id replacement} {
  global expression_substitutions__counter
  if {![info exists expression_substitutions__counter]} {set expression_substitutions__counter 1}
  puts [getStreamForTableFile expression_substitutions_1] [join [list $expression_substitutions__counter $address $operand_id $expr_id $replacement] "\t"]
  incr expression_substitutions__counter
}

proc {process__operand_expressions_1(operand_id, expr_id)} {operand_id expr_id} {
  puts [getStreamForTableFile operand_expressions_1] [join [list $operand_id $expr_id] "\t"]
}

proc {process__address_references_1(address, target, kind)} {address target kind} {
  puts [getStreamForTableFile address_references_1] [join [list $address $target $kind] "\t"]
}

# data_1 not used
# sections_1 not used

proc {process__address_comments_1(address, comment)} {address comment} {
  puts [getStreamForTableFile address_comments_1] [join [list $address $comment] "\t"]
}

if {[llength $argv] == 0} {
  error "Usage: $argv0 *.sql"
}
foreach filename $argv {
  puts $filename
  set root [file rootname $filename]
  set newdir "./${root}-tsv"
  file mkdir $newdir
  set f [open $filename r]
  set lineNumber 0
  while {1} {
    incr lineNumber
    set line [gets $f]
    if {[eof $f]} {break}
    if {![string match "INSERT INTO *" $line]} {continue}
    while {![regexp {^INSERT INTO (.*) values\((.*)\);$} $line] && ![eof $f]} {
      append line " [gets $f]"; # Handle continuation lines
    }
    if {![regexp {^INSERT INTO (.*) values\((.*)\);$} $line _ tableinfo vals]} {
      global lineNumber
      error "Bad line '$line' at line $lineNumber of $filename"
    }
    set vals [getSqlFields $vals]
    eval [concat [list "process__$tableinfo"] $vals]
  }
  close $f
  closeAllTableFiles
}
