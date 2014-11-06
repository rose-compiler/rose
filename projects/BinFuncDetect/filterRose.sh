#!/bin/bash

# Filter the output from $ROSE/projects/BinaryAnalysisTools/recursiveDisassemble --list-instruction-addresses by reading
# standard input and writing to standard output.  The input will be one line per instruction having the format:
#
#  INSN_VA+INSN_SIZE BBLOCK_VA FUNCTION_VA
#
# The *_VA fields are hexadecimal; INSN_SIZE is decimal. The FUNCTION_VA field is the string "nil" when no function
# owns the instruction and such lines are ignored.
#
# The output will be records, one per line, where each record contains four comma-separated fields:
#
#  INSN_VA, INSN_SIZE, 'c', FUNCTION_VA
#

#            INSN_ADDR   INSN_SZ   BBLOCK_VA     FUNCTION_VA
#          vvvvvvvvvvvvv  vvvvv   vvvvvvvvvvv   vvvvvvvvvvvvv
perl -ne '/(0x[0-9a-f]+)\+(\d+)\s+0x[0-9a-f]+\s+(0x[0-9a-f]+)/ && print "$1, $2, c, $3\n"'