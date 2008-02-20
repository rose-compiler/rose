#!/bin/sh
# \
exec tclsh "$0" "$@"

while {1} {
  set line [split [gets stdin] " "]
  if {[eof stdin]} {break}
  if {[lindex $line end-1] != "->"} {error "Incorrect line format: should be mnemonic ... -> AstNodeName" line}
  foreach mnemonic [lrange $line 0 end-2] {
    set astNode($mnemonic) [lindex $line end]
  }
}

proc doLeaf {insn} {
  global astNode
  if {[info exists astNode($insn)]} {
    return [format "{instruction = new SgAsmx86$astNode($insn)(address,  mnemonic, \"\"); goto done;}"]
  } else {
    return [format "{instruction = new SgAsmx86Instruction(address,  mnemonic, \"\"); goto done;}"]
  }
}

proc makeSwitch {len prefix indent} {
  global astNode
  set offset [string length $prefix]
  if {$offset == $len} {puts "${indent}[doLeaf $prefix]"; return}
  foreach str [array names astNode] {
    if {[string range $str 0 [expr {$offset - 1}]] != $prefix} {continue}
    if {[string length $str] != $len} {continue}
    set c [string index $str $offset]
    if {$c == ""} {continue}
    lappend firstChar($c) $str
  }
  if {[array size firstChar] == 0} {
    error "Bad prefix '$prefix' for length $len"
  } elseif {[array size firstChar] == 1} {
    puts "${indent}if (mnemonic\[$offset\] == '$c') { // '$prefix$c'"
    makeSwitch $len "$prefix$c" "${indent}  "
    puts "${indent}} else {"
    puts "${indent}  instruction = new SgAsmx86Instruction(address,  mnemonic, \"\"); goto done;"
    puts "${indent}}"
  } else {
    puts "${indent}switch (mnemonic\[$offset\]) {"
    foreach c [lsort [array names firstChar]] {
      puts "${indent}  case '$c': { // '$prefix$c'"
      makeSwitch $len "$prefix$c" "${indent}    "
      puts "${indent}  }"
    }
    puts "${indent}  default: {std::cerr << \"ERROR ::: Found bad instruction name '\" << mnemonic << \"'\\n\"; /*abort();*/}"
    puts "${indent}}"
  }
}

foreach mnemonic [array names astNode] {
  set byLength([string length $mnemonic]) x
}

puts "switch (mnemonic.size()) {"
foreach len [lsort -integer [array names byLength]] {
  puts "  case $len:"
  makeSwitch $len "" "    "
}
puts "  default: instruction = new SgAsmx86Instruction(address,  mnemonic, \"\");"
puts "}"
puts "done: ;"
