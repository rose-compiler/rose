#!/bin/sh
# \
exec tclsh "$0" "$@"

set data [split [read stdin] "\n"]
if {[string match "p cnf *" [lindex $data 0]]} {
  set data [lrange $data 1 end]
}

array set clauses {}
array set literalUses {}
array set units {}

proc addClause {cl} {
  global clauses literalUses units
  set cl [lsort -integer -unique $cl]
  set cl2 ""
  foreach lit $cl {
    if {[info exists units($lit)]} {
      set u $units($lit)
      if {$u} {
        return
      } else {
        continue
      }
    }
    lappend cl2 $lit
  }
  set cl $cl2
  foreach lit $cl {
    if {[lsearch -exact -integer -sorted $cl [expr {-$lit}]] != -1} {
      return ; # Tautology
    }
  }
  if {[info exists clauses($cl)]} {return}
  if {[llength $cl] == 0} {error "Unsatisfiable"}
  if {[llength $cl] == 1} {
    set ulit [lindex $cl 0]
    puts stderr "Unit literal $ulit"
    set units($ulit) 1
    set units([expr {-$ulit}]) 0
    if {[info exists literalUses($ulit)]} {
      foreach otherCl $literalUses($ulit) {
        removeClause $otherCl
      }
    }
  }
  set clauses($cl) ""
  foreach lit $cl {
    lappend literalUses($lit) $cl
  }
}

proc removeClause {cl} {
  global clauses literalUses
  set cl [lsort -integer -unique $cl]
  unset clauses($cl)
  foreach lit $cl {
    set literalUses($lit) [lsearch -all -inline -not -exact $literalUses($lit) $cl]
  }
}

array set usedVars {}
set ln 0
foreach line $data {
  if {$line == ""} {continue}
  set line [split $line " "]
  if {[lindex $line end] != 0} {error "Bad line '$line'"}
  set clause [lrange $line 0 end-1]
  addClause $clause
  foreach lit $clause {
    set usedVars([expr {abs($lit)}]) ""
  }
  incr ln
  if {$ln % 100000 == 0} {puts stderr "Line $ln"}
}

for {set pass 0} {$pass < 5} {incr pass} {
  puts stderr "Pass [expr {$pass + 1}] starting with [llength [array names clauses]] clauses"
  set pureCount 0
  set resolvedCount 0
  foreach var [lsort -integer [array names usedVars]] {
    if {$var % 10000 == 0} {puts stderr "Var $var"}
    if {![info exists literalUses($var)]} {
      set literalUses($var) ""
    }
    if {![info exists literalUses(-$var)]} {
      set literalUses(-$var) ""
    }
    set posCount [llength $literalUses($var)]
    set negCount [llength $literalUses(-$var)]
    if {$posCount == 0 || $negCount == 0} {
      # puts "Variable $var is pure"
      foreach cl $literalUses($var) {removeClause $cl}
      foreach cl $literalUses(-$var) {removeClause $cl}
      incr pureCount
    } elseif {[info exists units($var)] || ($posCount == 1 || $negCount == 1) && ($posCount + $negCount) <= 10} {
      set posClauses $literalUses($var)
      set negClauses $literalUses(-$var)
      foreach pc $posClauses {
        removeClause $pc
      }
      foreach nc $negClauses {
        removeClause $nc
      }
      foreach pc $posClauses {
        foreach nc $negClauses {
          # puts -nonewline stderr "Resolving $pc and $nc to produce "
          set pc [lsearch -all -inline -not -exact $pc $var]
          set nc [lsearch -all -inline -not -exact $nc -$var]
          # puts stderr [lsort -unique -integer [concat $pc $nc]]
          addClause [concat $pc $nc]
        }
      }
      unset usedVars($var)
      # puts "Variable $var should be resolved"
      incr resolvedCount
    } else {
      set kept 0
      set posClauses $literalUses($var)
      set negClauses $literalUses(-$var)
      if {$posCount * $negCount <= 30} {
        foreach pc $posClauses {
          foreach nc $negClauses {
            # puts -nonewline stderr "Resolving $pc and $nc to produce "
            set pc [lsearch -all -inline -not -exact $pc $var]
            set nc [lsearch -all -inline -not -exact $nc -$var]
            set newc [lsort -unique -integer [concat $pc $nc]]
            if {[llength $newc] > [llength $pc] && [llength $newc] > [llength $nc] && [llength $newc] > 3} {
              incr kept
              continue
            }
            # puts stderr [lsort -unique -integer [concat $pc $nc]]
            addClause $newc
          }
        }
        if {$kept == 0} {
          incr resolvedCount
          foreach pc $posClauses {
            removeClause $pc
          }
          foreach nc $negClauses {
            removeClause $nc
          }
        }
      }
    }
    # puts "$var $posCount $negCount"
  }
  puts stderr "Pure $pureCount resolved $resolvedCount of [llength [array names usedVars]]"
}
puts stderr "Ending with [llength [array names clauses]] clauses"

puts "p cnf [lindex [lsort -integer [array names usedVars]] end] [llength [array names clauses]]"
foreach cl [lsort -dictionary [array names clauses]] {
  puts stdout "$cl 0"
}
