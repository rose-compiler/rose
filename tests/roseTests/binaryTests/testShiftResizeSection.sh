#!/bin/bash
# Tests shiftResizeSection by attempting to enlarge the ".dynstr" section of an ELF executable by one byte.
#
# This seemingly simple operation is actually quite compilicated:
#
#    - The .dynstr section is immediately followed in the file by other sections and these sections probably have
#      alignment constraints that need to be satisfied.
#
#    - .dynstr is memory mapped and the mapped size needs to increase.
#
#    - Increasing .dynstr mapped size means ELF segments that are immediately right of .dynstr need to be shifted in
#      memory. However, segments that are far right of .dynstr should not be shifted if there's a sufficiently large
#      hole in the address space. E.g., if .dynstr is mapped somewhere in the 0x400000-0x4fffff and program data is
#      mapped in the 0x600000-0x6fffff range and the 0x500000-0x5fffff space is unused then anything at 0x600000 and
#      above should not be moved.
#
#    - The .dynstr is contained in ELF Load Segment 2 (in the file and in the memory mapping) which must now be extended so
#      it's last contained section doesn't slip out.
#
#    - All the section movements require that the ELF Section Table be updated. In addition, the location of the section table
#      in the file has probably changed.
#
#    - All the segment movements require that the ELF Segment Table be updated.
#
#    - The movement of the ELF Section Table requires that the ELF File Header be updated to point to the new location.
#
#    - The probable movement of the ".text" section within ELF Load Segment 2 in virtual address space requires that the
#      ELF File Header be updated to contain the new entry address.
#
#    - Movements of sections in the virtual address space necessitates that some virtual addresses in the ".dynamic"
#      section are adjusted.
#
# This test has three steps:
#    1. Parse the original executable to obtain a *.dump file
#    2. Modify the original executable to obtain a *.dump file and a new executable
#    3. Parse the new executable and compare it with the *.dump file from step #2
#
# Results:
#    At this time we must manually verify the diff output computed from the *.dump files of steps #2 and #3.
#    This output will typically contain four differences:
#      1. The "EOF" marker in the section table has a comment about the change in EOF
#      2. The .dynstr may have an internal hole that is a byte larger in #3 than in #2
#      3. The .dynstr in #2 has a 1-byte free list while the free list in #3 is empty
#      4. The internal hole in "ELF Load (segment 2)" is smaller in #2 than in #3

exe1="$1"
rose_switches=-rose:read_executable_file_format_only

#------------------------------------------------------------------------------------------------------------------------------
if [ ! -n "$exe1" ]; then echo "no executable given"; exit 1; fi
echo "testing shift/resize for $exe1" >&2
exebase=${exe1##*/}
dump1=$exebase.dump-1
dump2=$exebase.dump-2
dump3=$exebase.dump-3

rm -f $dump1 $dump2 $dump3
rm -f $exebase.dump $exebase.new
rm -f $exebase.new.dump $exebase.new.new

echo "running execFormatsTest..." >&2
./execFormatsTest $rose_switches $exe1 >&2 || exit 1
grep -v '\.data at 0x' <$exebase.dump >$dump1
rm $exebase.dump
diff $exe1 $exebase.new || exit 1

echo "running shiftResizeSection to extend .dynstr by 1 byte..." >&2
./shiftResizeSection $rose_switches $exe1 .dynstr 0 1 >&2 || exit 1
grep -v '\.data at 0x' <$exebase.dump >$dump2
rm $exebase.dump

echo "running execFormats test to read modified executable..." >&2
./execFormatsTest $rose_switches $exebase.new >&2 || exit 1
grep -v '\.data at 0x' <$exebase.new.dump >$dump3
rm -f $exebase.new.dump

diff -U10 $dump1 $dump3 | sed -n '3,$p'
exit 0 # success because previous diff requires human inspection