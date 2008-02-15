#!/usr/bin/perl

# DQ (2/22/2005): This script is for adding copywrite information to the 
# top of each source file within ROSE.

@lineArray = <>;
$lineNumber = 0;

# Processing for all header files:
# make all processed files first include the rose-g++-headerfilefixup.h header file
# to make sure that any file which the user calls will first define rose specific
# macros and all gnu built-in functions before processing any gnu header files
# which might require them.  In this case we put the header files in include
# guards that prevent unneccessary file lookup (for better performance in header
# file processing).
print "\n";
print "// ******************************************************************\n";
print "// *************** Put the copywrite information here ***************\n";
print "// ******************************************************************\n";
print "\n";
print "\n";

