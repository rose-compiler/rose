#!/usr/local/bin/perl
#
# Usage: cmp.pl <file1> <file2>
#

$ANAME = shift(@ARGV);
$BNAME = shift(@ARGV);

open(AFILE, "$ANAME") || die "Cannot open input file $ANAME...";
open(BFILE, "$BNAME") || die "Cannot open input file $BNAME...";

while (!eof(AFILE) && !eof(BFILE)) {
   $ALINE = <AFILE>;
   $BLINE = <BFILE>;
   $_ = $ALINE;

   if (!/^(\/\/|c|C|#|##)[ ]*(Release:[\t ]*\$Name|Revision:[\t ]*\$Revision|Modified:[\t ]*\$Date):[^\$]*\$/) {
      if ($ALINE ne $BLINE) {
         exit 1;
      }
   }
}

exit 0 if (eof(AFILE) && eof(BFILE));
exit 1;
