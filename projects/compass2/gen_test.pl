#!/usr/bin/perl
use warnings;
use strict;

# Michael Roup
# July 26, 2013

if (scalar @ARGV == 0) {
    print "Usage: \$ ./gen_test.pl checker name with spaces\n";
    exit 0;
}

my $makefile_path = "./tests/checkers/Makefile.am";
my $makefile_template = "./templates/makefile_template";
my $parameters_template = "./templates/parameters_template";
my $support_path = "../../config/support-rose.m4";
my $file_label = lc(join "_", @ARGV);
my $test_source = $file_label . "_test_1.cpp";

if (! defined -e $makefile_path) {
    die "Checkers' test makefile was not at $makefile_path\n";
}
if (! defined -e $makefile_template) {
    die "Makefile template was not at $makefile_template\n";
}
if (! defined -e $parameters_template) {
    die "Compass parameters template was not at $parameters_template\n";
}
if (! defined -e $support_path) {
    die "support-rose.m4 was not at $support_path\n";
}

#make sure the checker exists
if (! defined -e "./bin/checkers/$file_label") {
    die "This checker doesn't exist\n";
}

#make sure the tests don't exist
if (-e "./tests/checkers/$file_label") {
    die "This checker already has tests\n";
}



#get the short description from the checker
open INFILE, "./bin/checkers/$file_label/$file_label.cpp" or die "Couldn't open the checker source file\n";
my $short_description;

while (<INFILE>) {
    if (/const string short_description = "(.*)"/) {
	$short_description = $1;
	last;
    }
}

close INFILE;

#edit the makefile in tests/checkers
system("mv", $makefile_path, $makefile_path . "~");
open INFILE, $makefile_path."~" or die "Could not open Makefile.am~\n";
open OUTFILE, ">", $makefile_path or die "Could not open new file\n";

while (<INFILE>) {
    print OUTFILE;
    if (/^SUBDIRS/) {
	print OUTFILE "\t$file_label \\\n";
    }
    
}

close INFILE;
close OUTFILE;

mkdir "./tests/checkers/$file_label", 0700;

open INFILE, $makefile_template or die "Could not open makefile_template\n";
open OUTFILE, ">", "./tests/checkers/$file_label/Makefile.am" or die "Could not open new file\n";

while (<INFILE>) {
    s/PLACEHOLDER/$test_source/;
    s/editToMatchShortDescription/$short_description/;
    print OUTFILE;
}

close INFILE;
close OUTFILE;

open INFILE, $parameters_template or die "Could not open parameters_template\n";
open OUTFILE, ">", "./tests/checkers/$file_label/compass_parameters.xml.in" or die "Could not open new file\n";

my $camel_case;
for my $word (@ARGV) {
    $word = lc $word;
    $camel_case .= ucfirst $word;
}
my $no_first_camel = lcfirst $camel_case;
while (<INFILE>) {
    s/PLACEHOLDER/$no_first_camel/;
    print OUTFILE;
}

close INFILE;
close OUTFILE;

system("touch", "./tests/checkers/$file_label/$test_source");

system("mv", $support_path, $support_path . "~");

open INFILE, $support_path."~" or die "Could not open support-rose.m4\n";
open OUTFILE, ">", $support_path or die "Cou;d not open new file\n";

while (<INFILE>) {
    print OUTFILE;
    if (/^projects\/compass2\/tests\/checkers\/Makefile/) {
	print OUTFILE "projects/compass2/tests/checkers/$file_label/Makefile\n";
	print OUTFILE "projects/compass2/tests/checkers/$file_label/compass_parameters.xml\n";
    }
}

close INFILE;
close OUTFILE;
