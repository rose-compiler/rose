#!/bin/sh
# \
exec tclsh "$0" "$@"

proc readfile {name} {
  set f [open $name r]
  fconfigure $f -encoding binary -translation binary
  set data [read $f]
  close $f
  return $data
}

proc in_code_segment {addr} {
  global code_sections
  foreach {type virtaddr off size flg} $code_sections {
    if {$addr >= $virtaddr && $addr < $virtaddr + $size} {
      return 1
    }
  }
  return 0
}

proc split_on_unwrapped_comma {str} {
  set ls ""
  set count 0
  foreach c [split $str {}] {
    switch -exact -- $c {
      "(" {append ls $c; incr count}
      ")" {append ls $c; incr count -1}
      "," {if {$count == 0} {append ls " "} else {append ls $c}}
      default {append ls $c}
    }
  }
  return [split $ls " "]
}

proc get_register_size {reg} {
  if {[string match %* $reg]} {set reg [string range $reg 1 end]}
  switch -regexp -- $reg {
    {^[cdefgs]s$} {return sizeWord}
    {^e.*$} {return sizeDWord}
    {^r.*b$} {return sizeByte}
    {^r.*w$} {return sizeWord}
    {^r.*d$} {return sizeDWord}
    {^r.*$} {return sizeQWord}
    {^[abcd]h$} {return sizeByte}
    {^.*l$} {return sizeByte}
    default {return sizeWord}
  }
}

proc read_operand {op} {
  # puts stderr "read_operand $op"
  # if {[string match *:* $op]} { puts stderr "read_operand $op"}
  switch -regexp -- $op {
    {^%([er]..)$} {return "\"[string range $op 1 end]\" reg"}
    {^%(r[8-9])$} {return "\"[string range $op 1 end]\" reg"}
    {^%(r[8-9][dwb])$} {return "\"[string range $op 1 end]\" reg"}
    {^%(r1[0-5][dwb])$} {return "\"[string range $op 1 end]\" reg"}
    {^%.*flags$} {return "\"[string range $op 1 end]\" reg"}
    {^%([cdefgs]s)$} {return "\"[string range $op 1 end]\" reg"}
    {^%[abcd]l$} {return "\"[string range $op 1 end]\" reg"}
    {^%[abcd]h$} {return "\"[string range $op 1 end]\" reg"}
    {^%[sd]il$} {return "\"[string range $op 1 end]\" reg"}
    {^%[bs]pl$} {return "\"[string range $op 1 end]\" reg"}
    {^%[abcd]x$} {return "\"[string range $op 1 end]\" reg"}
    {^%[sd]i$} {return "\"[string range $op 1 end]\" reg"}
    {^%[bs]p$} {return "\"[string range $op 1 end]\" reg"}
    {^%st$} {return "\"st0\" reg"}
    {^%st\([0-7]\)$} {return "\"st[string index $op 4]\" reg"}
    {^%mm[0-7]$} {return "\"[string range $op 1 end]\" reg"}
    {^%xmm([0-9]+)$} {return "\"[string range $op 1 end]\" reg"}
    {^\$0x0x.*$} {return [read_operand "\$[string range $op 3 end]"]}
    {^\$0x([0-9a-fA-F]*)$} {
      if {[in_code_segment [string range $op 1 end]]} {get_insn [string range $op 1 end]}
      return "[format %u 0x[string range $op 3 end]] constant"
    }
    {^\$0x([^`]*)`.*$} {
      regexp {^\$(0x[^`]*)`<(.*)>$} $op _ val annot
      if {[in_code_segment $val]} {get_insn $val}
      return "[format %u $val] constant \"$annot\" annotate"
    }
    {^\$([-0-9a-f]*) *$} {
      if {[in_code_segment 0x[string range $op 1 end]]} {get_insn 0x[string range $op 1 end]}
      return "[format %u 0x[string range $op 1 end]] constant"
    }
    {^%[cdesfg]s:.*$} {return "\"[string range $op 1 2]\" reg [read_operand [string range $op 4 end]] segment"}
    default {return "[read_operand_addr $op] mem_read"}
  }
}

proc read_operand_addr {op} {
  # puts stderr "read_operand_addr $op"
  switch -regexp -- $op {
    {^[-0-9A-Fa-f]+`.*$} {
      regexp {^([0-9A-Fa-f]+)`<(.*)>$} $op _ op annot
      if {[in_code_segment 0x$op]} {get_insn 0x$op}
      return "[format %u 0x${op}] constant \"$annot\" annotate"
    }
    {^[-0-9A-Fa-f]+$} {
      if {[in_code_segment 0x$op]} {get_insn 0x$op}
      return "[format %u 0x${op}] constant"
    }
    {^0x[0-9A-Fa-f]+$} {
      if {[in_code_segment $op]} {get_insn $op}
      return "[format %u 0x[string range ${op} 2 end]] constant"
    }
    {^[-x0-9A-Fa-f]*\([^)]*\)$} {
      if {[regexp {^([-x0-9A-Fa-f]*)\(([^)]*)\)$} $op _ offset sib]} {
	if {$offset == ""} {
          set offset "0 constant"
        } elseif {[string range $offset 0 1] == "0x"} {
          set offset "[format %u 0x[string range $offset 2 end]] constant"
        } else {
          set offset "[format %u $offset] constant"
        }
	set sib [split_on_unwrapped_comma $sib]
	if {[llength $sib] == 3} {
	  if {[lindex $sib 0] == ""} {
	    if {$offset == "0 constant"} {
	      return "[read_operand [lindex $sib 1]] [lindex $sib 2] constant mul"
	    } else {
	      return "$offset [read_operand [lindex $sib 1]] [lindex $sib 2] constant mul add"
	    }
	  } else {
	    if {$offset == "0 constant"} {
	      return "[read_operand [lindex $sib 0]] [read_operand [lindex $sib 1]] [lindex $sib 2] constant mul add"
	    } else {
	      return "$offset [read_operand [lindex $sib 0]] [read_operand [lindex $sib 1]] [lindex $sib 2] constant mul add add"
	    }
	  }
	} elseif {[llength $sib] == 2} {
	  if {[lindex $sib 0] == ""} {
            return "$offset [read_operand [lindex $sib 1]] add"
          } else {
	    return "$offset [read_operand [lindex $sib 0]] [read_operand [lindex $sib 1]] add add"
          }
	} elseif {[llength $sib] == 1} {
	  if {$offset == "0 constant"} {
	    return "[read_operand [lindex $sib 0]]"
	  } else {
	    return "$offset [read_operand [lindex $sib 0]] add"
	  }
	} else {
	  error "Bad sib $sib"
	}
      } else {
	error "Bad sib-like address $op"
      }
    }
    {^\*.*} {return [read_operand [string range $op 1 end]]}
    default {error "Bad operand to read address '$op'"}
  }
}

proc letterToSizeInt {letter} {
  switch -exact $letter {
    b {return sizeByte}
    w {return sizeWord}
    l {return sizeDWord}
    q {return sizeQWord}
    default {return sizeUnknown}
  }
}

proc letterToSizeFloat {letter} {
  switch -exact $letter {
    s {return sizeFloat}
    l {return sizeDouble}
    default {return sizeLongDouble}
  }
}

proc convert_insn {next_addr insn} {
  regsub -all {^(rep[^ ]*) } $insn {\1-} insn
  regsub -all {^lock } $insn {lock-} insn
  regsub -all { *#.*} $insn "" insn
  regsub -all { <} $insn {`<} insn
  set result ""
  if {[regexp {^([^ ]*) *(.*)$} $insn _ opcode operands]} {
    set operands [string trim $operands]
    regsub -all ",.*" $opcode "" opcode; # Remove branch prediction hints
    set operands [split_on_unwrapped_comma $operands]
    set i 0
    foreach op $operands {
      set op${i}_raw $op
      set op$i "<invalid operand>"
      catch {set op$i [read_operand $op]}
      incr i
    }
    set sizeInt [letterToSizeInt [string index $opcode end]]
    set sizeFloat [letterToSizeFloat [string index $opcode end]]
    switch -glob $opcode {
      mov? {append result "\"mov\" $op1 $sizeInt $op0 $sizeInt 2 insn"}
      movzbw {append result "\"movzx\" $op1 sizeWord $op0 sizeByte 2 insn"}
      movzbl {append result "\"movzx\" $op1 sizeDWord $op0 sizeByte 2 insn"}
      movzwl {append result "\"movzx\" $op1 sizeDWord $op0 sizeWord 2 insn"}
      movzbq {append result "\"movzx\" $op1 sizeQWord $op0 sizeByte 2 insn"}
      movzwq {append result "\"movzx\" $op1 sizeQWord $op0 sizeWord 2 insn"}
      movzlq {append result "\"movzx\" $op1 sizeQWord $op0 sizeDWord 2 insn"}
      movsbw {append result "\"movsx\" $op1 sizeWord $op0 sizeByte 2 insn"}
      movsbl {append result "\"movsx\" $op1 sizeDWord $op0 sizeByte 2 insn"}
      movswl {append result "\"movsx\" $op1 sizeDWord $op0 sizeWord 2 insn"}
      movsbq {append result "\"movsx\" $op1 sizeQWord $op0 sizeByte 2 insn"}
      movswq {append result "\"movsx\" $op1 sizeQWord $op0 sizeWord 2 insn"}
      movslq {append result "\"movsx\" $op1 sizeQWord $op0 sizeDWord 2 insn"}
      bswap {append result "\"bswap\" $op0 sizeDWord 1 insn"}
      xchg? {append result "\"xchg\" $op1 $sizeInt $op0 $sizeInt 2 insn"}
      cmpxchg? {append result "\"cmpxchg\" $op1 $sizeInt $op0 $sizeInt 2 insn"}
      cbtw {append result "\"cbw\" 0 insn"}
      cwtl {append result "\"cwde\" 0 insn"}
      cltd {append result "\"cdq\" 0 insn"}
      cltq {append result "\"cdqe\" 0 insn"}
      cqto {append result "\"cqo\" 0 insn"}
      inc? {append result "\"inc\" $op0 $sizeInt 1 insn"}
      dec? {append result "\"dec\" $op0 $sizeInt 1 insn"}
      add? {append result  "\"add\"   $op1 $sizeInt $op0 $sizeInt 2 insn"}
      adc? {append result  "\"adc\"   $op1 $sizeInt $op0 $sizeInt 2 insn"}
      sub? {append result  "\"sub\"   $op1 $sizeInt $op0 $sizeInt 2 insn"}
      sbb? {append result  "\"sbb\"   $op1 $sizeInt $op0 $sizeInt 2 insn"}
      and? {append result  "\"and\"   $op1 $sizeInt $op0 $sizeInt 2 insn"}
      or? {append result   "\"or\"    $op1 $sizeInt $op0 $sizeInt 2 insn"}
      xor? {append result  "\"xor\"   $op1 $sizeInt $op0 $sizeInt 2 insn"}
      cmp? {append result  "\"cmp\"   $op1 $sizeInt $op0 $sizeInt 2 insn"}
      test? {append result "\"test\"  $op1 $sizeInt $op0 $sizeInt 2 insn"}
      neg? {append result "\"neg\" $op0 $sizeInt 1 insn"}
      not? {append result "\"not\" $op0 $sizeInt 1 insn"}
      mul? {append result "\"mul\" $op0 $sizeInt 1 insn"}
      imul? {
	switch -exact [llength $operands] {
	  1 {append result "\"imul\" $op0 $sizeInt 1 insn"}
	  2 {append result "\"imul\" $op1 $sizeInt $op0 $sizeInt 2 insn"}
	  3 {append result "\"imul\" $op2 $sizeInt $op0 $sizeInt $op1 $sizeInt 3 insn"}
	}
      }
      div? {append result "\"div\" $op0 $sizeInt 1 insn"}
      idiv? {append result "\"idiv\" $op0 $sizeInt 1 insn"}
      shl? {
	if {![info exists op1]} {
	  append result "\"shl\" $op0 $sizeInt 1 insn"
	} else {
	  append result "\"shl\" $op1 $sizeInt $op0 sizeByte 2 insn"
	}
      }
      shldl {
	append result "\"shld\" $op2 $sizeInt $op0 $sizeInt $op1 $sizeInt 3 insn"
      }
      shr? {
	if {![info exists op1]} {
	  append result "\"shr\" $op0 $sizeInt 1 insn"
	} else {
	  append result "\"shr\" $op1 $sizeInt $op0 sizeByte 2 insn"
	}
      }
      sar? {
	if {![info exists op1]} {
	  append result "\"sar\" $op0 $sizeInt 1 insn"
	} else {
	  append result "\"sar\" $op1 $sizeInt $op0 sizeByte 2 insn"
	}
      }
      shrdl {
	append result "\"shrd\" $op2 $sizeInt $op0 $sizeInt $op1 $sizeInt 3 insn"
      }
      rol? {
	if {![info exists op1]} {
	  append result "\"rol\" $op0 $sizeInt 1 insn"
	} else {
	  append result "\"rol\" $op1 $sizeInt $op0 sizeByte 2 insn"
	}
      }
      ror? {
	if {![info exists op1]} {
	  append result "\"ror\" $op0 $sizeInt 1 insn"
	} else {
	  append result "\"ror\" $op1 $sizeInt $op0 sizeByte 2 insn"
	}
      }
      rcl? {
	if {![info exists op1]} {
	  append result "\"rcl\" $op0 $sizeInt 1 insn"
	} else {
	  append result "\"rcl\" $op1 $sizeInt $op0 sizeByte 2 insn"
	}
      }
      rcr? {
	if {![info exists op1]} {
	  append result "\"rcr\" $op0 $sizeInt 1 insn"
	} else {
	  append result "\"rcr\" $op1 $sizeInt $op0 sizeByte 2 insn"
	}
      }
      lea? {append result "\"lea\" $op1 $sizeInt $op0 $sizeInt 2 insn"}
      lds? {append result  "\"lds\" $op1 $sizeInt $op0 $sizeInt 2 insn"}
      push? {append result "\"push\" $op0 $sizeInt 1 insn"}
      pushfw {append result "\"pushf\" 0 insn"}
      pushfl {append result "\"pushfd\" 0 insn"}
      pushfq {append result "\"pushfq\" 0 insn"}
      pushaw {append result "\"pusha\" 0 insn"}
      pushal {append result "\"pushad\" 0 insn"}
      pop? {append result "\"pop\" $op0 $sizeInt 1 insn"}
      popfw {append result "\"popf\" 0 insn"}
      popfl {append result "\"popfd\" 0 insn"}
      popfq {append result "\"popfq\" 0 insn"}
      popaw {append result "\"popa\" 0 insn"}
      popal {append result "\"popad\" 0 insn"}
      jmp -
      jmpq -
      jmpl -
      jmpw {
	if {[string index $op0_raw 0] == "*"} {
	  append result "\"jmp\" [read_operand [string range $op0_raw 1 end]] $sizeInt 1 insn"
	} elseif {[string range $op0_raw 0 1] == "0x"} {
	  append result "\"jmp\" [read_operand \$$op0_raw] 1 insn"; get_insn $op0_raw
	} else {
	  append result "\"jmp\" [read_operand \$0x$op0_raw] 1 insn"; get_insn 0x$op0_raw
	}
      }
      jcxz {append result "\"jcxz\" [read_operand \$0x$op0_raw] sizeDWord 1 insn"; get_insn $op0}
      jecxz {append result "\"jecxz\" [read_operand \$0x$op0_raw] sizeDWord 1 insn"; get_insn $op0}
      jrcxz {append result "\"jrcxz\" [read_operand \$0x$op0_raw] sizeDWord 1 insn"; get_insn $op0}
      loop* {append result "\"$opcode\" [read_operand \$0x$op0_raw] $sizeInt 1 insn"; get_insn $op0}
      j* {append result "\"$opcode\" [read_operand \$0x$op0_raw] $sizeInt 1 insn"; get_insn $op0}
      set* {append result "\"$opcode\" $op0 sizeByte 1 insn"}
      cmov* {append result "\"$opcode\" $op1 [get_register_size $op1] $op0 [get_register_size $op1] 2 insn"}
      cld {append result "\"cld\" 0 insn"}
      std {append result "\"std\" 0 insn"}
      sahf {append result "\"sahf\" 0 insn"}
      bsr? {append result "\"bsr\" $op1 $sizeInt $op0 $sizeInt 2 insn"}
      bsf? {append result "\"bsf\" $op1 $sizeInt $op0 $sizeInt 2 insn"}
      bt? -
      bts? -
      btc? -
      btr? {
	append result "\"$opcode\" $op0 $sizeInt $op1 $sizeInt 2 insn"
      }
      repz-* {convert_insn $next_addr [string range $insn 5 end]; append result " repe"}
      repnz-* {convert_insn $next_addr [string range $insn 6 end]; append result " repne"}
      scasl {append result "\"scasd\" $op1 $sizeInt $op0 $sizeInt 2 insn"}
      scas? {append result "\"$opcode\" $op1 $sizeInt $op0 $sizeInt 2 insn"}
      cmpsl {append result "\"cmpsd\" $op1 $sizeInt $op0 $sizeInt 2 insn"}
      cmps? {append result "\"$opcode\" $op1 $sizeInt $op0 $sizeInt 2 insn"}
      movsl {append result "\"movsd\" $op1 $sizeInt $op0 $sizeInt 2 insn"}
      movs? {append result "\"$opcode\" $op1 $sizeInt $op0 $sizeInt 2 insn"}
      stosl {append result "\"stosd\" $op1 $sizeInt $op0 $sizeInt 2 insn"}
      stos? {append result "\"$opcode\" $op1 $sizeInt $op0 $sizeInt 2 insn"}
      lodsl {append result "\"lodsd\" $op1 $sizeInt $op0 $sizeInt 2 insn"}
      lods? {append result "\"$opcode\" $op1 $sizeInt $op0 $sizeInt 2 insn"}
      insl {append result "\"insd\" $op1 $sizeInt $op0 $sizeInt 2 insn"}
      ins? {append result "\"$opcode\" $op1 $sizeInt $op0 $sizeInt 2 insn"}
      outsl {append result "\"outsd\" $op1 $sizeInt $op0 $sizeInt 2 insn"}
      outs? {append result "\"$opcode\" $op1 $sizeInt $op0 $sizeInt 2 insn"}
      call -
      callq -
      calll -
      callw {
	if {[string index $op0_raw 0] == "*"} {
	  append result "\"call\" [read_operand [string range $op0_raw 1 end]] $sizeInt 1 insn"
	} elseif {[string range $op0_raw 0 1] == "0x"} {
	  append result "\"call\" [read_operand \$$op0_raw] 1 insn"; get_insn $op0_raw
	} else {
	  append result "\"call\" [read_operand \$0x$op0_raw] 1 insn"; get_insn 0x$op0_raw
	}
      }
      retw -
      retl -
      retq {if {[info exists op0]} {append result "\"ret\"$op0 1 insn"} else {append result "\"ret\" 0 insn"}}
      leavel -
      leaveq {append result "\"leave\" 0 insn"}
      int {append result "\"int\" $op0 sizeByte 1 insn"}
      rdtsc {append result "\"rdtsc\" 0 insn"}
      hlt {append result "\"hlt\" 0 insn"}
      lock-* {convert_insn $next_addr [string range $insn 5 end]; append result " lock"}
      nop {append result "\"nop\" 0 insn"}
      .byte {append result "\".byte\" 0 insn"}
      data16 {append result "\"data16\" 0 insn"}

      stmxcsr {append result "\"stmxcsr\" $op0 sizeDWord 1 insn"}
      ldmxcsr {append result "\"ldmxcsr\" $op0 sizeDWord 1 insn"}

      fist {
        append result "\"$opcode\" $op0 $sizeInt 1 insn"
      }

      fi* {
	set result "\"$opcode\" "
	for {set i 0} {[info exists op$i]} {incr i} {}
	set opcount $i
	for {incr i -1} {$i >= 0} {incr i -1} {
	  append result " [set op$i]"
          if {$i != $opcount - 1} {
            append result " $sizeInt"
          } else {
            append result " $sizeFloat"
          }
	}
	append result "$result $opcount insn"
      }

      f* {
	set result "\"$opcode\" "
	for {set i 0} {[info exists op$i]} {incr i} {}
	set opcount $i
	for {incr i -1} {$i >= 0} {incr i -1} {
	  append result " [set op$i] $sizeFloat"
	}
	append result "$result $opcount insn"
      }

      default {
	set result "\"$opcode\" "
	for {set i 0} {[info exists op$i]} {incr i} {}
	set opcount $i
	for {incr i -1} {$i >= 0} {incr i -1} {
	  append result " [set op$i] $sizeInt"
	}
	append result "$result $opcount insn"
      }
    }
  } else {
    error "Bad insn format '$insn'"
  }
  # puts stderr "Emitting result '$result'"
  puts -nonewline $result
}

array set instructions {}
array set basic_block_starts {}
array set instructions_leading_to_point {}

set executable [readfile [lindex $argv 0]]
if {![binary scan $executable @0icc magic class dataformat]} {
  error "Invalid format"
}
if {$magic != 0x464C457F} {error "Not an ELF binary"}
switch -exact $class {
  1 {set bits 32}
  2 {set bits 64}
  default {error "Bad binary class $class"}
}
switch -exact $dataformat {
  1 {set endian little}
  2 {set endian big}
  default {error "Bad binary format $dataformat"}
}

if {$bits == 64 && 1000000000 + 2000000000 < 0} {
  error "Cannot analyze 64-bit binaries on a 32-bit system"
}

switch -exact "$bits-$endian" {
  32-little {set headerformat @18s@24i@32i@46ss}
  32-big {set headerformat @18S@24I@32I@46SS}
  64-little {set headerformat @18s@24w@40w@58ss}
  64-big {set headerformat @18S@24W@40W@58SS}
}

if {![binary scan $executable $headerformat archNumber start_addr shoff shentsize shnum]} {
  error "Invalid format"
}

switch -exact $archNumber {
  3 {set arch i386}
  40 {set arch arm}
  62 {set arch x86-64}
  default {error "Bad architecture number $archNumber"}
}

if {$arch != "i386" && $arch != "x86-64"} {error "This asmToRoseAst.tcl does not support architecture $arch"}

switch -exact -- $arch {
  i386 -
  x86-64 {
    set objdump "objdump"
    set objdumpFlags "-Msuffix"
  }
  arm {
    set objdump arm-unknown-linux-gnu-objdump
    set objdumpFlags "-Mreg-names-raw"
  }
  default {
    error "Bad architecture $arch"
  }
}
# set data [readfile "|${objdump} -d -w -z $objdumpFlags --show-raw-insn [lindex $argv 0]"]

if {$shentsize != 40 && $bits == 32 || $shentsize != 64 && $bits == 64} {error "Bad section header size"}

set code_sections ""
set sections ""

switch -exact "$bits-$endian" {
  32-little {set sectionformat x4iiiii}
  32-big {set sectionformat x4IIIII}
  64-little {set sectionformat x4iwwww}
  64-big {set sectionformat x4IWWWW}
}

for {set i 0} {$i < $shnum} {incr i} {
  if {![binary scan $executable @[expr {$shoff + $i * $shentsize}]$sectionformat type flags addr offset size]} {error "Bad section $i"}
  lappend sections $type $addr $offset $size $flags
  # puts stderr "Got section $type [format %x $addr] $offset [format %x $size] $flags"
  if {($flags & 6) == 6} { # Executable and alloc
    # puts stderr "Code section"
    lappend code_sections $type $addr $offset $size $flags
  }
}

array set disassembly_cache {}

proc get_disassembly_of_instruction {addr} {
  global disassembly_cache argv code_sections
  set addr [format %d $addr]
  if {[info exists disassembly_cache($addr)]} {return $disassembly_cache($addr)}
  set sectsize 1
  set sectpos 0
  foreach {type start offset size flags} $code_sections {
    if {$addr >= $start && $addr < $start + $size} {
      set sectsize $size
      set sectpos [expr {$addr - $start}]
    }
  }
  puts stderr "Running disassembly for 0x[format %x $addr] (byte $sectpos / $sectsize in segment)"
  set data [readfile [list |objdump -d -w -z -Msuffix --show-raw-insn --start-addr=$addr --stop-addr=[expr {$addr + 0x4000}] [lindex $argv 0]]]
  set lines [split $data \n]
  foreach line $lines {
    if {[regexp {^ *([0-9a-fA-F]+):(.*)$} $line _ addrhex rest]} {
      set disassembly_cache([format %d 0x$addrhex]) $rest
    }
  }
  if {[info exists disassembly_cache($addr)]} {return $disassembly_cache($addr)}
  return ""
}

set get_insn_worklist {}
set get_insn_running 0

# puts stderr $data

proc get_insn {addr} {
  global basic_block_starts get_insn_running get_insn_worklist sections
  # puts stderr "get_insn start $addr"
  regsub -all {[ `].*} $addr "" addr ; # Used for annotations on jumps
  if {![scan $addr %d _]} {return}; # Remove indirect jumps and similar things (by removing all non-numbers)
  if {![in_code_segment $addr]} {return}; # Bogus pointers (call 0 and such)
  set addr 0x[format %x $addr]
  set basic_block_starts($addr) 1; # This might be needed even if the instruction has already been converted, but is not done for later steps in the loop below (which are just handling fallthrough cases)
  # puts stderr "get_insn outer $addr"
  lappend get_insn_worklist $addr
  if {$get_insn_running} {
    return
  } else {
    set get_insn_running 1
    while {[llength $get_insn_worklist] != 0} {
      set addr [lindex $get_insn_worklist 0]
      set get_insn_worklist [lrange $get_insn_worklist 1 end]
      while {$addr != ""} { # Avoid recursion in simple cases
	set addr 0x[format %x $addr]
	global argv instructions disassembly_cache
	if {[info exists instructions([expr {$addr + 0}])]} {break}
	set instructions([expr {$addr + 0}]) ""; # Reserve it so this argument will not be used again
	# puts stderr "get_insn inner $addr"
	set disassembly [get_disassembly_of_instruction $addr]
	set re "^\\t(\[ 0-9A-Fa-f\]*)\\t(\[^\\n\]*)\$"
        # puts stderr "Got disassembly '$disassembly'"
	# puts stderr "re = '$re'"
	# puts stderr "Output data is $data"
	if {[regexp -lineanchor $re $disassembly _ bytes insn]} {
	  set bytes [string trim $bytes]
	  # puts stderr "Got insn at 0x[format %X $addr]: '$insn' = $bytes"
	  regsub -all " " $bytes "" bytes
	  set next_addr [expr {$addr + [string length $bytes] / 2}]
	  # puts stderr "next is 0x[format %X $next_addr]"
	  set instructions([expr {0 + $addr}]) [list $insn $next_addr]
	  if {$next_addr == ""} {set next_addr 0}
	  set x "[format %u ${addr}] [format %u ${next_addr}] \"$insn\" \"$bytes\" "
          # puts stderr "Emitting '$x'"
	  puts -nonewline $x
	  convert_insn $next_addr $insn
          # puts stderr "Emitting ''"
	  puts ""
	  if {![info exists instructions_leading_to_point($next_addr)]} {set instructions_leading_to_point($next_addr) 0}
	  incr instructions_leading_to_point($next_addr)
	  if {$instructions_leading_to_point($next_addr) >= 2} {set basic_block_starts($next_addr) 1}
	  #if {[string match j* $insn] || 
	  #    [string match call* $insn] || 
          #    [string match ret* $insn] || 
          #    [string match hlt* $insn] || 
	  #    [string match loop* $insn]} {set basic_block_starts($next_addr) 1}

	  if {$next_addr != 0} {
	    set addr $next_addr
	  }
	} else {
	  break
	}
      }
    }
    set get_insn_running 0
  }
}

switch -exact $bits-$endian {
  32-little {set pointerformat i}
  32-big {set pointerformat I}
  64-little {set pointerformat w}
  64-big {set pointerformat W}
}

get_insn $start_addr
foreach {type virtaddr off size flag} $sections {
  if {($flag & 6) == 2 && $type == 1} { ; # Allocated, not executable, PROGBITS
    # puts stderr "Scanning section at 0x[format %x $virtaddr] for code pointers"
    for {set i 0} {$i < $size} {incr i [expr {$bits / 8}]} {
      binary scan $executable @${off}x${i}$pointerformat value
      if {[in_code_segment $value]} {get_insn $value}
    }
  }
}
foreach addr [array names basic_block_starts] {
  puts "[format %u $addr] basic_block_start"
}
puts "done"
