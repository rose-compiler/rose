#!/bin/bash

# Filter the output from IDA by reading standard input and writing to standard output.  The input is one record per line, having
# the following comma-separated fields:
#
#  FILE_HASH: output from md5sum run on the specimen
#  FUNCTION_VA: function entry address in hexadecimal w/out leading "0x"
#  FUNCTION_NAME: name of function (which must not include a comma or linefeed)
#  HASH: hash of the BYTES field
#  PICHASH: hash of the PICBYTES field
#  BYTES: bytes as a hexadecimal string w/out leading "0x"
#  PICBYTES: bytes as a hexadecimal string w/out leading "0x"
#  FLAGS: one of the words 'None', 'library', or 'thunk'
#
# The output will be one record per line, each having the following comma-separated fields
#
#  FUNCTION_VA, FUNCTION_SIZE, 'c', FUNCTION_VA
#
# where FUNCTION_VA is prefixed by "0x" both times, and FUNCTION_SIZE is decimal.

filehash="$1"

grep "^$1" |perl -ne '@r = map {lc} split(","); $r[7] ne "None" and print "0x$r[1], @{[length($r[5])/2]}, c, 0x$r[1]\n"'
