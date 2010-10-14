#!/usr/bin/perl

# DQ (12/9/2009): This script is for counting the number of lines of output per directory for the "make check" rule.
# To run use: "make check | tee file; grep -n "Making check in " | makeCheckLineCount.pl"

@lineArray = <>;
$lineNumber = 0;

@newlineArray;

$previousValue = 0;
$previousCount = 0;
$previousLine = "START";

# Loop over all the strings in a file
while ($lineNumber <= $#lineArray)
   {
     $line = $lineArray[$lineNumber];

   # Flag set to false and reset if line matches and is output 
   # by one of the rules below.
     $lineHasBeenOutput = 0;

     $value = int $line;

     $count = ($value - $previousValue) - 1;

     $_ = $previousLine;

   # Build the target string to search for
     $targetStringMake      = "$previousValue:Making all in ";
     $targetStringMakeCheck = "$previousValue:Making check in ";

   # Remove the "#:Making all in " substring (for compiling using make).
     s/$targetStringMake//g;

   # Remove the "#:Making check in " substring (for tests using the make check rule).
     s/$targetStringMakeCheck//g;

   # Remove the end of line character
     s/\n//g;
     $previousLine = $_;
     $newline = "$count\t: $previousLine \n";
   # print $newline; 

     $previousValue = $value;
     $previousCount = $count;
     $previousLine  = $line;

   # Copy into an array
   # $newlineArray[++$#newlineArray] = $newline;
     if ($count > 0)
        { $newlineArray[$lineNumber] = $newline; }

     $lineNumber++;
   }

# Define how the sort should be done internally (integer based instead of character based)
sub numerically { int $a <=> int $b }
# @sortedList = reverse sort numerically @newlineArray;
@sortedList = sort numerically @newlineArray;

print "\n\n\n";
print "*********************************************************** \n";
print "***********  Least Parallel Build Directories  ************ \n";
print "**** (values represent the number of files to compile) **** \n";
print "****  (small values thus represent least parallelism)  **** \n";
print "*********************************************************** \n";

# Output the sorted list
$lineNumber = 0;
while ($lineNumber <= $#sortedList)
   {
     print$sortedList[$lineNumber];
     $lineNumber++;
   }
print "*****************************************";
