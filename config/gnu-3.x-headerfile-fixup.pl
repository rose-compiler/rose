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


@lineArray = <>;
$lineNumber = 0;

# DQ (4/11/2005): This should not be required now that we use GNU extensions within EDG and support them in ROSE
if (0) {
# Processing for all header files:
# make all processed files first include the rose-g++-headerfilefixup.h header file
# to make sure that any file which the user calls will first define rose specific
# macros and all gnu built-in functions before processing any gnu header files
# which might require them.  In this case we put the header files in include
# guards that prevent unneccessary file lookup (for better performance in header
# file processing).
print "\n";
print "// ***************** Automaticaly Generated Header File**************\n";
print "// The file included below is a ROSE specific header file which      \n";
print "// needs to be seen before any processing of the gnu 3.x compiler    \n";
print "// header files (rose-g++-headerfilefixup.h in not a generated file).\n";
print "// ***************** Automaticaly Generated Header File**************\n";
print "#ifndef _ROSE_GNU_HEADERFILEFIXUP_H\n";
print "   #include \"rose-g++-headerfilefixup.h\"\n";
print "#endif // _ROSE_GNU_HEADERFILEFIXUP_H\n";
print "\n";
print "\n";
}

# Loop over all the strings in a file
while ($lineNumber <= $#lineArray)
   {
     $line = $lineArray[$lineNumber];

   # Flag set to false and reset if line matches and is output 
   # by one of the rules below.
     $lineHasBeenOutput = 0;

if (0) { # JJW 10-29-2007 Do we really need this now?  It breaks compilers in non-standard locations
     if ($line =~ /include_next/)
        {
        # Processing of "include_next" non-standard CPP macro mechanism

        # Both limits.h and syslimits.h use the include_only non-standard directive
        # so we have to translate these (other compiler vendors make wider use of 
        # this mechanism.
          $_ = $line;
        # s/# *include_next *["<]\(.*\)[">]/#include "\/usr\/include\/\1"/g;
          s/# *include_next *["<](.*)[">]/#include "\/usr\/include\/\1"/;
          $line = $_;
          print "$line";

        # Mark as being output
          $lineHasBeenOutput = 1;
        }
}

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

   # DQ (4/9/2005): This should not be required now that we use GNU extensions within EDG and support them in ROSE
     if (0) {
     if ($line =~ /extern template/)
        {
        # Processing of "extern template" C++ gnu extension to standard C++

        # sstream.tcc uses this "gnu specific "extern template" syntax it is used in
        # many other files as well (total of 369 locations in the g++ 3.x header files).
          $_ = $line;
          s/extern *template/template/;
          $line = $_;
          print "$line";

        # Mark as being output
          $lineHasBeenOutput = 1;
        }
        }

   # DQ (4/9/2005): This should not be required now that we use GNU extensions within EDG and support them in ROSE
     if (0) {
# DQ (6/4/2005): For support of g++ 3.4.x headers we need to test for __glibcxx_function_requires
   # if ($line =~ /__glibcpp_function_requires|__glibcpp_class_requires/)
     if ($line =~ /__glibcpp_function_requires|__glibcpp_class_requires|__glibcxx_function_requires/)
        {
        # Processing of C99 macro mechanismused in GNU header files and which is 
        # non-standard in C++ front-end.

        # print "Found a specific macro! \n";
          $macroText = $line;
          if ($macroText =~ /\)$/)
             {
             # print "This is the whole macro! macro = $macroText \n";
             # Output the single line macro commented out
               print "// $macroText";
             }
            else
             {
             # print "There is another line to this macro! \n";
               $macroLineNumber = 0;
               $found = 0;
               $multiLineMacro = "";
               while (!$found)
                  {
                    $multiLineMacro .= substr($lineArray[$lineNumber + $macroLineNumber],0);
                  # print "Building up multiLineMacro = $multiLineMacro \n";

                  # Output the single line macro commented out
                    print "// $lineArray[$lineNumber + $macroLineNumber]";
                  # if ($multiLineMacro =~ /\)$/)
                    if ($multiLineMacro =~ /\)/)
                       {
                       # print "Found last line of macro! \n";
                         $found = 1;
                       }
                      else
                       {
                       # print "Keep searching ... macroLineNumber = $macroLineNumber \n";
                         $macroLineNumber++;
                       }
                  }

               $lineNumber += $macroLineNumber;
               $macroText = $multiLineMacro;
             }

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

