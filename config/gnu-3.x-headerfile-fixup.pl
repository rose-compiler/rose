#!/usr/bin/perl

# DQ (8/27/2004):
# This script is part of the support for the use of newer 3.x gnu header 
# files within ROSE when using the gnu g++ version 3.x backend compiler.
# This is an improvement over the previous work which processed the gnu
# 2.96 header files for use with ROSE when using the g++ version 3.x 
# compilers. And of course it is finally done in Perl which I always
# thought might be the best way to do it.

# This Perl script processes a gnu 3.x header file and comments out a 
# collection of macros which can span several lines. The macros 
# follow a C(( macro syntax not supported by any C++ compiler
# or any older (non-c99) C compiler.  The specific macros are
# __glibcpp_function_requires and __glibcpp_class_requires
# and implements a "concepts" mechanism which provides
# type properties information to the compiler ans which
# will be likely supported in future versions of C++.  
# For more info on C++ "Concepts" consult some of Bjarne's
# technical papers.

#### JJW 7/18/2008: This script does not do anything (all nontrivial parts are
#### in an if (0) block), so it is not called anymore

@lineArray = <>;
$lineNumber = 0;

# Loop over all the strings in a file
while ($lineNumber <= $#lineArray)
   {
     $line = $lineArray[$lineNumber];

   # Flag set to false and reset if line matches and is output 
   # by one of the rules below.
     $lineHasBeenOutput = 0;

   # JJW 6-11-2008: This is not likely to be necessary anymore
   if (0) {
   # DQ (7/24/2006): Modified macros to avoid overflow.  These macros causes warning which were annoying (EDG complained, but gnu did not)
   # This works for gnu 3.3.2 header files, but should be portable
     if ($line =~ /  \(__glibcpp_signed \(T\) \? \(T\)1 \<\< __glibcpp_digits \(T\) \: \(T\)0\)/)
        {
        # print "Found target macro!!!\n";
        # exit 1;

          $_ = $line;
          s/\(__glibcpp_signed \(T\) \? \(T\)1 \<\< __glibcpp_digits \(T\) \: \(T\)0\)/\(__glibcpp_signed \(T\) \? \(T\)\(1 \<\< __glibcpp_digits \(T\)\) \: \(T\)0\)/;
          $line = $_;
          print "$line";

        # Mark as being output
          $lineHasBeenOutput = 1;
        }

   # DQ (7/24/2006): Modified macros to avoid overflow.  These macros causes warning which were annoying (EDG complained, but gnu did not)
   # This works for gnu 3.3.2 header files, but should be portable
     if ($line =~ /  \(__glibcpp_signed \(T\) \? \(\(T\)1 \<\< __glibcpp_digits \(T\)\) - 1 \: \~\(T\)0\)/)
        {
        # print "Found target macro!!!\n";
        # exit 1;

          $_ = $line;
          s/\(__glibcpp_signed \(T\) \? \(\(T\)1 \<\< __glibcpp_digits \(T\)\) - 1 \: \~\(T\)0\)/\(__glibcpp_signed \(T\) \? \(T\)\(\(1LL \<\< __glibcpp_digits \(T\)\) - 1\) \: \~\(T\)0\)/;
          $line = $_;
          print "$line";

        # Mark as being output
          $lineHasBeenOutput = 1;
        }
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

