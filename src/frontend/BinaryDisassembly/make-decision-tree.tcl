#!/bin/sh
# \
exec tclsh "$0" "$@"

proc doLeaf {insn platform} {
  global astNode$platform
  if {[info exists astNode${platform}($insn)]} {
    return [format "{return new SgAsm${platform}[set astNode${platform}($insn)](address,  mnemonic, \"\");}"]
  } else {
    return [format "{return new SgAsm${platform}Instruction(address,  mnemonic, \"\");}"]
  }
}

proc makeSwitch {len prefix platform outFile indent} {
  global astNode$platform
  set offset [string length $prefix]
  if {$offset == $len} {puts $outFile "${indent}[doLeaf $prefix $platform]"; return}
  foreach str [array names astNode$platform] {
    if {[string range $str 0 [expr {$offset - 1}]] != $prefix} {continue}
    if {[string length $str] != $len} {continue}
    set c [string index $str $offset]
    if {$c == ""} {continue}
    lappend firstChar($c) $str
  }
  if {[array size firstChar] == 0} {
    error "Bad prefix '$prefix' for length $len"
  } elseif {[array size firstChar] == 1} {
    puts $outFile "${indent}if (mnemonic\[$offset\] == '$c') { // '$prefix$c'"
    makeSwitch $len "$prefix$c" $platform $outFile "${indent}  "
    puts $outFile "${indent}} else {"
    puts $outFile "${indent}  return new SgAsm${platform}Instruction(address,  mnemonic, \"\");"
    puts $outFile "${indent}}"
  } else {
    puts $outFile "${indent}switch (mnemonic\[$offset\]) {"
    foreach c [lsort [array names firstChar]] {
      puts $outFile "${indent}  case '$c': { // '$prefix$c'"
      makeSwitch $len "$prefix$c" $platform $outFile "${indent}    "
      puts $outFile "${indent}  }"
    }
    puts $outFile "${indent}  default: return new SgAsm${platform}Instruction(address,  mnemonic, \"\");"
    puts $outFile "${indent}}"
  }
}

proc makeSwitchForLanguage {platform outFile} {
  global astNode$platform
  set instructionTable [open "RoseBin_instruction_table_[string tolower $platform]" r]
  while {1} {
    set line [split [gets $instructionTable] " "]
    if {[eof $instructionTable]} {break}
    if {[lindex $line end-1] != "->"} {error "Incorrect line format: should be mnemonic ... -> AstNodeName" line}
    foreach mnemonic [lrange $line 0 end-2] {
      set astNode${platform}($mnemonic) [lindex $line end]
    }
  }
  close $instructionTable

  foreach mnemonic [array names astNode$platform] {
    set byLength([string length $mnemonic]) x
  }

  puts $outFile "SgAsm${platform}Instruction* create${platform}Instruction(uint64_t address, const std::string& mnemonic) {"
  puts $outFile "switch (mnemonic.size()) {"
  foreach len [lsort -integer [array names byLength]] {
    puts $outFile "  case $len:"
    makeSwitch $len "" $platform $outFile "    "
  }
  puts $outFile "  default: return new SgAsm${platform}Instruction(address,  mnemonic, \"\");"
  puts $outFile "}"
  puts $outFile "}"
}

set outFile [open "instructionDispatch.cpp" w]
puts $outFile "#include \"rose.h\""
puts $outFile "#include <stdint.h>"
foreach platform {x86 Arm} {
  makeSwitchForLanguage $platform $outFile
}
close $outFile
