#!/bin/sh
# \
exec tclsh "$0" "$@"

proc doLeaf {insn platform} {
  global astNode$platform
  if {[info exists astNode${platform}($insn)]} {
    return [format "insn->set_kind([string tolower $platform]_[string tolower [set astNode${platform}($insn)]]);"]
  } else {
    return [format ";"]
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
    puts $outFile "${indent}}"
  } else {
    puts $outFile "${indent}switch (mnemonic\[$offset\]) {"
    foreach c [lsort [array names firstChar]] {
      puts $outFile "${indent}  case '$c': { // '$prefix$c'"
      makeSwitch $len "$prefix$c" $platform $outFile "${indent}    "
      puts $outFile "${indent}  break;}"
    }
    puts $outFile "${indent}  default: break;"
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
  puts $outFile "  SgAsm${platform}Instruction* insn = new SgAsm${platform}Instruction(address, mnemonic, [string tolower $platform]_unknown_instruction);"
  puts $outFile "  switch (mnemonic.size()) {"
  foreach len [lsort -integer [array names byLength]] {
    puts $outFile "    case $len:"
    makeSwitch $len "" $platform $outFile "      "
    puts $outFile "    break;"
  }
  puts $outFile "    default: break;"
  puts $outFile "  }"
  puts $outFile "  return insn;"
  puts $outFile "}"
}


##BEGIN SUPPORT FOR STRING MNEMONIC NORMALIZATION
##AS(10/04/08) This is necessary for the work where we compare 
##the output of ROSE with the output of IDA Pro. 

proc doLeafNormalize {insn platform} {
  global astNode$platform
  if {[info exists astNode${platform}($insn)]} {
    return [format "mnemonic = \"[set astNode${platform}($insn)]\";"]
  } else {
    return [format ";"]
  }
}

proc makeSwitchNormalize {len prefix platform outFile indent} {
  global astNode$platform
  set offset [string length $prefix]
  if {$offset == $len} {puts $outFile "${indent}[doLeafNormalize $prefix $platform]"; return}
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
    makeSwitchNormalize $len "$prefix$c" $platform $outFile "${indent}  "
    puts $outFile "${indent}}"
  } else {
    puts $outFile "${indent}switch (mnemonic\[$offset\]) {"
    foreach c [lsort [array names firstChar]] {
      puts $outFile "${indent}  case '$c': { // '$prefix$c'"
      makeSwitchNormalize $len "$prefix$c" $platform $outFile "${indent}    "
      puts $outFile "${indent}  break;}"
    }
    puts $outFile "${indent}  default: break;"
    puts $outFile "${indent}}"
  }
}


proc makeSwitchForLanguageNormalizeToString {platform outFile} {
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

  puts $outFile "void normalize${platform}Mnemonic(std::string& mnemonic) {"
  #puts $outFile "  std::string returnMnemonic = \"unknown_instruction\";"
  puts $outFile "  switch (mnemonic.size()) {"
  foreach len [lsort -integer [array names byLength]] {
    puts $outFile "    case $len:"
    makeSwitchNormalize $len "" $platform $outFile "      "
    puts $outFile "    break;"
  }
  puts $outFile "    default: break;"
  puts $outFile "  }"
  #puts $outFile "  return returnMnemonic;"
  puts $outFile "}"
}

## END CODE FOR SUPPORTING NORMALIZATION OF MNEMONICS



set outFile [open "instructionDispatch.cpp" w]
puts $outFile "#include \"rose.h\""
puts $outFile "#include <stdint.h>"
puts $outFile "// ************************************"
puts $outFile "// THIS FILE IS AUTOMATICALLY GENERATED"
puts $outFile "// ************************************"
foreach platform {x86 Arm} {
  makeSwitchForLanguage $platform $outFile

  ##Call code for generation the function to normalize mnemnoic strings
  ##necessary for work which compares ROSE disassembler with IDA Pro.
  makeSwitchForLanguageNormalizeToString $platform $outFile

}
close $outFile
