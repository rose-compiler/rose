#!/usr/local/bin/perl
#
# Usage: depend.pl <depend directory> <include directory> <.C files>
#

#
# Set up global directory information and output depend file
#

$DIR     = shift(@ARGV);
$INCLUDE = shift(@ARGV);
$DEPEND  = "$DIR/Makefile.depend.tmp";
@FILES   = @ARGV;
@INCPATH = ($DIR, $INCLUDE);

$TABLEN  = 8;
$LINLEN  = 72;

#
# For each of the specified files, get dependency information and write to file
#

open(OUTFILE, ">$DEPEND") || die "Cannot open output file $DEPEND...";

$EMPTY="";

print OUTFILE "##\n";
print OUTFILE "## File:\tMakefile.depend\n";
print OUTFILE "## Package:\tA++P++\n";
print OUTFILE "## Copyright:\t(c) 1997-1999 The Regents of the University of California\n";
print OUTFILE "## Release:\t\$N${EMPTY}ame:  \$\n";
print OUTFILE "## Revision:\t\$R${EMPTY}evision: \$\n";
print OUTFILE "## Modified:\t\$D${EMPTY}ate: \$\n";
print OUTFILE "## Description:\tmakefile dependencies\n";
print OUTFILE "##\n\n";

# Setup the DEPEND_INCLUDE path for finding the header files in the A++ or P++ include dir!
print OUTFILE "DEPEND_INCLUDE = ../../include \n";

print OUTFILE "##\n\n";

for (@FILES) {
   my $FILE = getFullPath($_);
#  print "In Main: $FILE ";
   @DEPS = getDependencies($FILE, ($FILE));
   printDependencies($FILE, sort(@DEPS));
}
close(OUTFILE);

#
# Recursively generate dependencies using the default search directories
#

sub getDependencies {
   my $FILE = shift(@_);
   my @DEPS = @_;
   my @TODO = ();

   $_ = $FILE;
   if (!/.*\.f$/) {
      if (open(DEPFILE, $FILE)) {
         while (<DEPFILE>) {
#           print "$1 ";
            if (/^#[\t ]*include[\t ]*\"([^\"]+)\"/) {
               @TODO = (@TODO, $1);
            }
         }
         close(DEPFILE);
      }

      for (@TODO) {
         my $DEPFILE = getFullPath($_);
         if ((length($DEPFILE) > 0) && (!inSet($DEPFILE, @DEPS))) {
            @DEPS = getDependencies($DEPFILE, ($DEPFILE, @DEPS));
         }
      }
   }

   return(@DEPS);
}

#
# Look up the full name of the include file from the default or include paths
#

sub getFullPath {
   my $FILE    = shift(@_);

   for (@INCPATH) {
      my $FULLFILE = "$_/$FILE";
      if (-r $FULLFILE) {
         return($FULLFILE);
      }
   }

   return("");
}

#
# Search the set and check whether the first argument appears in the set
#

sub inSet {
   my $SEARCH = shift(@_);
   for (@_) {
      if ($_ eq $SEARCH) {
#        print "In inset: $SEARCH ";
         return(1);
      }
   }
   return 0;
}

#
# Print out data dependencies in a pleasing manner
#

sub printDependencies {
   my $FILE = shift(@_);
   my @DEPS = @_;

   my $LIBLINE = printName($FILE);
   $LIBLINE =~ s/^(.*)\.[Ccf]/$1.o:/;
   print OUTFILE "$LIBLINE";
   $NTAB = ($LINLEN-length($LIBLINE))/$TABLEN;
   for ($i = 0; $i < $NTAB; $i++) {
      print OUTFILE "\t";
   }
   print OUTFILE "\\\n";

   print OUTFILE "\t";
   $CURLEN = $TABLEN;
   for (@DEPS) {
      $DEP = printName($_);
      if (length($DEP)+$CURLEN >= $LINLEN) {
         $NTAB = ($LINLEN-$CURLEN)/$TABLEN;
         for ($i = 0; $i < $NTAB; $i++) {
            print OUTFILE "\t";
         }
         print OUTFILE "\\\n";
         print OUTFILE "\t";
         $CURLEN = $TABLEN;
      }
      if ($CURLEN == $TABLEN) {
         print OUTFILE "$DEP";
         $CURLEN += length($DEP);
      } else {
         print OUTFILE " $DEP";
         $CURLEN += length($DEP)+1;
      }
   }
   print OUTFILE "\n";
}

#
# Convert the filename to a print name - remove include path or ./ prefix
#

# INCLUDE changed to DEPEND_INCLUDE to avoid confusion with Automake variable INCLUDES 
# DEPEND_INCLUDE must be setup in the Makefile
sub printName {
   $_ = shift(@_);
   if (/include\/([^\/]*)$/) {
      return("\$(DEPEND_INCLUDE)/$1");
   } elsif (/\/([^\/]*)$/) {
      return($1);
   } else {
      return($_);
   }
}
