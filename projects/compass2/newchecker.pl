#!/usr/bin/perl
use warnings;
use strict;

# Written by Mike Roup for Compass2
# 7-2-2013
# This program creates a new checker from a template
# and automatically edits the requisite files so that
# the checker can be used without having to do any extra
# Work. If this script is moved out of the compass2 root
# the path variables below will have to be edited.
#
# Backup copies of the edited files follow the emacs format.
# e.g. The original version of compass_main.cpp is compass_main.cpp~


if (scalar @ARGV == 0) {
    print "Usage: \$ newchecker.pl checker name seperated by spaces\n";
    exit 0;
}

my $main_path     = "./bin/compass_main.cpp";
my $makefile_path = "./Makefile.am";
my $template_path = "./templates/checker_template";
my $xml_path      = "./share/xml/compass_parameters.xml.in";


if (! defined -e $main_path) {
    die "Compass main was not at $main_path\n";
}
if (! defined -e $makefile_path) {
    die "Makefile was not at $makefile_path\n";
}
if (! defined -e $template_path) {
    die "Checker template was not at $template_path\n";
}
if (! defined -e $xml_path) {
    die "Compass parameters were not at $xml_path\n";
}

##
# Check to see if a checker of the same name already exists
#
my $file_label = lc(join "_", @ARGV);

if (-e "./bin/checkers/$file_label") {
    die "This checker already exists\n";
}

open TEMPLATE, $template_path or die "Couldn't open checker_template.cpp\n";

##
# Put the new file in a subdirectory of the checkers directory
#
mkdir "./bin/checkers/$file_label", 0700;
open OUTPUT, ">","./bin/checkers/$file_label/$file_label.cpp" or die "Couldn't open new file\n";

##
# Replace generic function and namespace names with
# the name of the checker in the template
#
my $include_guard = uc $file_label;
my $camel_case;
for my $word (@ARGV) {
    $word = lc $word;
    $camel_case .= ucfirst $word
}
my $no_first_camel = lcfirst $camel_case;

while (<TEMPLATE>) {
    s/INCLUDE_GUARD/$include_guard/g;
    s/NOFIRSTCAMEL/$no_first_camel/g;
    s/CAMELCASE/$camel_case/g;
    print OUTPUT;
}

close TEMPLATE;
close OUTPUT;

##
# Modify compass_main.cpp
#
system("mv", $main_path, $main_path . "~");
open INFILE, $main_path."~" or die "Could not open compass_main.cpp~\n";
open OUTFILE, ">", $main_path or die "Could not open new file\n";

my $loopcontrol = 1;
while (<INFILE>) {
    if (/\/\/PLACEHOLDER1/ && $loopcontrol == 1) {
	print OUTFILE "extern const Compass::Checker* const ", $no_first_camel . "Checker;\n";
        $loopcontrol++;
    } elsif (/\/\/PLACEHOLDER2/ && $loopcontrol == 2) {
        print OUTFILE "            else if (\"$no_first_camel\" == checker_name)\n                traversals.push_back ($no_first_camel","Checker);\n";
        $loopcontrol++;
    }
    print OUTFILE;
}

close INFILE;
close OUTFILE;

##
# Modify compass_parameters.xml.in
#
system("mv", $xml_path, $xml_path . "~");
open INFILE, $xml_path."~" or die "Could not open compass_parameters.xml.in~\n";
open OUTFILE, ">", $xml_path or die "Could not open new file\n";

$loopcontrol = 1;
while (<INFILE>) {
    if (/<!--PLACEHOLDER-->/ && $loopcontrol == 1) {
        print OUTFILE "    <parameter name=\"enabled_checker\">$no_first_camel</parameter>\n";
        $loopcontrol++;
    }
    print OUTFILE;
}

close INFILE;
close OUTFILE;


##
# Modify Makefile.am
#

system("mv", $makefile_path, $makefile_path . "~");
open INFILE, $makefile_path."~" or die "Could not open Makefile.am\n";
open OUTFILE, ">", $makefile_path or die "Could not open new file\n";

$loopcontrol = 1;
my $seen_boostldf = 0;
my $seen_noinst = 0;
while (<INFILE>) {
    if (/BOOST_LDFLAGS/) {
        $seen_boostldf++;
    }
    if ($seen_boostldf == 2 && $loopcontrol == 1) {
        print OUTFILE "\tbin/checkers/$file_label/lib", lc(join "", @ARGV), ".la \\\n";
	$loopcontrol++;
    }
    if ($seen_noinst == 1 && $loopcontrol == 2) {
        print OUTFILE "\tbin/checkers/$file_label/lib", lc(join "", @ARGV), ".la \\\n";
	$loopcontrol++;
    }
    if (/noinst_LTLIBRARIES/) {
	$seen_noinst++;
    }
    if (/PLACEHOLDER/ && $loopcontrol == 3) {
        print OUTFILE "bin_checkers_", $file_label, "_lib", lc(join "", @ARGV), "_la_SOURCES=\\\n\tbin/checkers/$file_label/$file_label.cpp\n\n";
        $loopcontrol++;
    }
    print OUTFILE;
}

close INFILE;
close OUTFILE;
