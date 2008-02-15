#!/usr/bin/perl

# DQ (2/22/2005): This script is for updating names in old ROSE files.

@lineArray = <>;
$lineNumber = 0;

# Loop over all the strings in a file
while ($lineNumber <= $#lineArray)
   {
     $line = $lineArray[$lineNumber];

   # Flag set to false and reset if line matches and is output 
   # by one of the rules below.
     $lineHasBeenOutput = 0;

     if ($line =~ /include_next/)
        {
          $_ = $line;
#         s/# *include_next *["<](.*)[">]/#include "\/usr\/include\/\1"/;
          s/# *include_next *["<](.*)[">]/#include "\/usr\/include\/\1"/g;
          $line = $_;
          print "$line";

        # Mark as being output
          $lineHasBeenOutput = 1;
        }

   # If not previously output by the above rules then output the unmodified line 
   # from the processed file.
     if (!$lineHasBeenOutput)
        {
        # Output an unmodified line of the header file
          print "$line";
        }

     $lineNumber++;
   }

