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
	  return "$offset [read_operand [lindex $sib 0]] [read_operand [lindex $sib 1]] add add"
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

proc letterToSize {letter} {
  switch -exact $letter {
    b {return 1}
    w {return 2}
    l {return 4}
    q {return 8}
    default {return 0}
  }
}

proc convert_insn {next_addr insn} {
  regsub -all {^(rep[^ ]*) } $insn {\1-} insn
  regsub -all {^lock } $insn {lock-} insn
  regsub -all { *#.*} $insn "" insn
  regsub -all { <} $insn {`<} insn
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
    set size [letterToSize [string index $opcode end]]
    switch -glob $opcode {
      mov? {puts -nonewline "\"mov\" $op1 size$size $op0 size$size 2 insn"}
      movzbw {puts -nonewline "\"movzx\" $op1 size2 $op0 size1 2 insn"}
      movzbl {puts -nonewline "\"movzx\" $op1 size4 $op0 size1 2 insn"}
      movzwl {puts -nonewline "\"movzx\" $op1 size4 $op0 size2 2 insn"}
      movzbq {puts -nonewline "\"movzx\" $op1 size8 $op0 size1 2 insn"}
      movzwq {puts -nonewline "\"movzx\" $op1 size8 $op0 size2 2 insn"}
      movzlq {puts -nonewline "\"movzx\" $op1 size8 $op0 size4 2 insn"}
      movsbw {puts -nonewline "\"movsx\" $op1 size2 $op0 size1 2 insn"}
      movsbl {puts -nonewline "\"movsx\" $op1 size4 $op0 size1 2 insn"}
      movswl {puts -nonewline "\"movsx\" $op1 size4 $op0 size2 2 insn"}
      movsbq {puts -nonewline "\"movsx\" $op1 size8 $op0 size1 2 insn"}
      movswq {puts -nonewline "\"movsx\" $op1 size8 $op0 size2 2 insn"}
      movslq {puts -nonewline "\"movsx\" $op1 size8 $op0 size4 2 insn"}
      bswap {puts -nonewline "\"bswap\" $op0 size4 1 insn"}
      xchg? {puts -nonewline "\"xchg\" $op1 size$size $op0 size$size 2 insn"}
      cmpxchg? {puts -nonewline "\"cmpxchg\" $op1 size$size $op0 size$size 2 insn"}
      cwtl {puts -nonewline "\"cwde\" 0 insn"}
      cltd {puts -nonewline "\"cdq\" 0 insn"}
      cltq {puts -nonewline "\"cdqe\" 0 insn"}
      cqto {puts -nonewline "\"cqo\" 0 insn"}
      inc? {puts -nonewline "\"inc\" $op0 size$size 1 insn"}
      dec? {puts -nonewline "\"dec\" $op0 size$size 1 insn"}
      add? {puts -nonewline  "\"add\"   $op1 size$size $op0 size$size 2 insn"}
      adc? {puts -nonewline  "\"adc\"   $op1 size$size $op0 size$size 2 insn"}
      sub? {puts -nonewline  "\"sub\"   $op1 size$size $op0 size$size 2 insn"}
      sbb? {puts -nonewline  "\"sbb\"   $op1 size$size $op0 size$size 2 insn"}
      and? {puts -nonewline  "\"and\"   $op1 size$size $op0 size$size 2 insn"}
      or? {puts -nonewline   "\"or\"    $op1 size$size $op0 size$size 2 insn"}
      xor? {puts -nonewline  "\"xor\"   $op1 size$size $op0 size$size 2 insn"}
      cmp? {puts -nonewline  "\"cmp\"   $op1 size$size $op0 size$size 2 insn"}
      test? {puts -nonewline "\"test\"  $op1 size$size $op0 size$size 2 insn"}
      neg? {puts -nonewline "\"neg\" $op0 size$size 1 insn"}
      not? {puts -nonewline "\"not\" $op0 size$size 1 insn"}
      mul? {puts -nonewline "\"mul\" $op0 size$size 1 insn"}
      imul? {
	switch -exact [llength $operands] {
	  1 {puts -nonewline "\"imul\" $op0 size$size 1 insn"}
	  2 {puts -nonewline "\"imul\" $op1 size$size $op0 size$size 2 insn"}
	  3 {puts -nonewline "\"imul\" $op2 size$size $op0 size$size $op1 size$size 3 insn"}
	}
      }
      div? {puts -nonewline "\"div\" $op0 size$size 1 insn"}
      idiv? {puts -nonewline "\"idiv\" $op0 size$size 1 insn"}
      shl? {
	if {![info exists op1]} {
	  puts -nonewline "\"shl\" $op0 size$size 1 insn"
	} else {
	  puts -nonewline "\"shl\" $op1 size$size $op0 size1 2 insn"
	}
      }
      shldl {
	puts -nonewline "\"shld\" $op2 size$size $op0 size$size $op1 size$size 3 insn"
      }
      shr? {
	if {![info exists op1]} {
	  puts -nonewline "\"shr\" $op0 size$size 1 insn"
	} else {
	  puts -nonewline "\"shr\" $op1 size$size $op0 size1 2 insn"
	}
      }
      sar? {
	if {![info exists op1]} {
	  puts -nonewline "\"sar\" $op0 size$size 1 insn"
	} else {
	  puts -nonewline "\"sar\" $op1 size$size $op0 size1 2 insn"
	}
      }
      shrdl {
	puts -nonewline "\"shrd\" $op2 size$size $op0 size$size $op1 size$size 3 insn"
      }
      rol? {
	if {![info exists op1]} {
	  puts -nonewline "\"rol\" $op0 size$size 1 insn"
	} else {
	  puts -nonewline "\"rol\" $op1 size$size $op0 size1 2 insn"
	}
      }
      ror? {
	if {![info exists op1]} {
	  puts -nonewline "\"ror\" $op0 size$size 1 insn"
	} else {
	  puts -nonewline "\"ror\" $op1 size$size $op0 size1 2 insn"
	}
      }
      rcl? {
	if {![info exists op1]} {
	  puts -nonewline "\"rcl\" $op0 size$size 1 insn"
	} else {
	  puts -nonewline "\"rcl\" $op1 size$size $op0 size1 2 insn"
	}
      }
      rcr? {
	if {![info exists op1]} {
	  puts -nonewline "\"rcr\" $op0 size$size 1 insn"
	} else {
	  puts -nonewline "\"rcr\" $op1 size$size $op0 size1 2 insn"
	}
      }
      lea? {puts -nonewline "\"lea\" $op1 size$size $op0 size$size 2 insn"}
      push? {puts -nonewline "\"push\" $op0 size$size 1 insn"}
      pushfw {puts -nonewline "\"pushf\" 0 insn"}
      pushfl {puts -nonewline "\"pushfd\" 0 insn"}
      pushfq {puts -nonewline "\"pushfq\" 0 insn"}
      pushaw {puts -nonewline "\"pusha\" 0 insn"}
      pushal {puts -nonewline "\"pushad\" 0 insn"}
      pop? {puts -nonewline "\"pop\" $op0 size$size 1 insn"}
      popfw {puts -nonewline "\"popf\" 0 insn"}
      popfl {puts -nonewline "\"popfd\" 0 insn"}
      popfq {puts -nonewline "\"popfq\" 0 insn"}
      popaw {puts -nonewline "\"popa\" 0 insn"}
      popal {puts -nonewline "\"popad\" 0 insn"}
      jmp -
      jmpq -
      jmpl -
      jmpw {
	if {[string index $op0_raw 0] == "*"} {
	  puts -nonewline "\"jmp\" [read_operand [string range $op0_raw 1 end]] size$size 1 insn"
	} elseif {[string range $op0_raw 0 1] == "0x"} {
	  puts -nonewline "\"jmp\" [read_operand \$$op0_raw] 1 insn"; get_insn $op0_raw
	} else {
	  puts -nonewline "\"jmp\" [read_operand \$0x$op0_raw] 1 insn"; get_insn 0x$op0_raw
	}
      }
      jcxz {puts -nonewline "\"jcxz\" [read_operand \$0x$op0_raw] size4 1 insn"; get_insn $op0}
      jecxz {puts -nonewline "\"jecxz\" [read_operand \$0x$op0_raw] size4 1 insn"; get_insn $op0}
      jrcxz {puts -nonewline "\"jrcxz\" [read_operand \$0x$op0_raw] size4 1 insn"; get_insn $op0}
      loop* {puts -nonewline "\"$opcode\" [read_operand \$0x$op0_raw] size$size 1 insn"; get_insn $op0}
      j* {puts -nonewline "\"$opcode\" [read_operand \$0x$op0_raw] size$size 1 insn"; get_insn $op0}
      set* {puts -nonewline "\"$opcode\" $op0 size1 1 insn"}
      cmov* {puts -nonewline "\"$opcode\" $op1 $op0 2 insn"}
      cld {puts -nonewline "\"cld\" 0 insn"}
      std {puts -nonewline "\"std\" 0 insn"}
      sahf {puts -nonewline "\"sahf\" 0 insn"}
      bsr? {puts -nonewline "\"bsr\" $op1 size$size $op0 size$size 2 insn"}
      bsf? {puts -nonewline "\"bsf\" $op1 size$size $op0 size$size 2 insn"}
      bt? -
      bts? -
      btc? -
      btr? {
	puts -nonewline "\"$opcode\" $op0 size$size $op1 size$size 2 insn"
      }
      repz-* {convert_insn $next_addr [string range $insn 5 end]; puts -nonewline " repe"}
      repnz-* {convert_insn $next_addr [string range $insn 6 end]; puts -nonewline " repne"}
      scasl {puts -nonewline "\"scasd\" $op1 size$size $op0 size$size 2 insn"}
      scas? {puts -nonewline "\"$opcode\" $op1 size$size $op0 size$size 2 insn"}
      cmpsl {puts -nonewline "\"cmpsd\" $op1 size$size $op0 size$size 2 insn"}
      cmps? {puts -nonewline "\"$opcode\" $op1 size$size $op0 size$size 2 insn"}
      movsl {puts -nonewline "\"movsd\" $op1 size$size $op0 size$size 2 insn"}
      movs? {puts -nonewline "\"$opcode\" $op1 size$size $op0 size$size 2 insn"}
      stosl {puts -nonewline "\"stosd\" $op1 size$size $op0 size$size 2 insn"}
      stos? {puts -nonewline "\"$opcode\" $op1 size$size $op0 size$size 2 insn"}
      lodsl {puts -nonewline "\"lodsd\" $op1 size$size $op0 size$size 2 insn"}
      lods? {puts -nonewline "\"$opcode\" $op1 size$size $op0 size$size 2 insn"}
      insl {puts -nonewline "\"insd\" $op1 size$size $op0 size$size 2 insn"}
      ins? {puts -nonewline "\"$opcode\" $op1 size$size $op0 size$size 2 insn"}
      outsl {puts -nonewline "\"outsd\" $op1 size$size $op0 size$size 2 insn"}
      outs? {puts -nonewline "\"$opcode\" $op1 size$size $op0 size$size 2 insn"}
      call -
      callq -
      calll -
      callw {
	if {[string index $op0_raw 0] == "*"} {
	  puts -nonewline "\"call\" [read_operand [string range $op0_raw 1 end]] size$size 1 insn"
	} elseif {[string range $op0_raw 0 1] == "0x"} {
	  puts -nonewline "\"call\" [read_operand \$$op0_raw] 1 insn"; get_insn $op0_raw
	} else {
	  puts -nonewline "\"call\" [read_operand \$0x$op0_raw] 1 insn"; get_insn 0x$op0_raw
	}
      }
      retw -
      retl -
      retq {if {[info exists op0]} {puts -nonewline "\"ret\"$op0 1 insn"} else {puts -nonewline "\"ret\" 0 insn"}}
      leavel -
      leaveq {puts -nonewline "\"leave\" 0 insn"}
      int {puts -nonewline "\"int\" $op0 size1 1 insn"}
      rdtsc {puts -nonewline "\"rdtsc\" 0 insn"}
      hlt {puts -nonewline "\"hlt\" 0 insn"}
      lock-* {convert_insn $next_addr [string range $insn 5 end]; puts -nonewline " lock"}
      nop {puts -nonewline "\"nop\" 0 insn"}
      .byte {puts -nonewline "\".byte\" 0 insn"}
      data16 {puts -nonewline "\"data16\" 0 insn"}

      default {
	set result "\"$opcode\" "
	for {set i 0} {[info exists op$i]} {incr i} {}
	set opcount $i
	for {incr i -1} {$i >= 0} {incr i -1} {
	  append result \ [set op$i]
	}
	puts -nonewline "$result $opcount insn"
      }
    }
  } else {
    error "Bad insn format '$insn'"
  }
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
set data [readfile "|${objdump} -d -w -z $objdumpFlags --show-raw-insn [lindex $argv 0]"]

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
  global disassembly_cache argv
  set addr [format %d $addr]
  if {[info exists disassembly_cache($addr)]} {return $disassembly_cache($addr)}
  # puts stderr "Running disassembly for 0x[format %x $addr]"
  set data [readfile [list |objdump -d -w -z -Msuffix --show-raw-insn --start-addr=$addr --stop-addr=[expr {$addr + 0x1000}] [lindex $argv 0]]]
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
  global basic_block_starts get_insn_running get_insn_worklist sections data
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
	  puts -nonewline "[format %u ${addr}] [format %u ${next_addr}] \"$insn\" \"$bytes\" "
	  convert_insn $next_addr $insn
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
