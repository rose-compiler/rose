#!/usr/bin/perl
use strict;

my $specimen = shift;
die "usage: $0 SPECIMEN\n" unless -r $specimen;
my $top_blddir = $ENV{ROSEGIT_BLD} || $ENV{ROSE_BUILD};
die "ROSEGIT_BLD or ROSE_BUILD should be defined in your environment\n" unless -d $top_blddir;

my $disassemble = $top_blddir . "/tests/nonsmoke/functional/roseTests/binaryTests/disassemble";
die "$disassemble does not exist or is not executable\n" unless -e $disassemble;

my $dflags = "--disassemble=dp --raw=0 -rose:disassembler_search unused,deadend";
my(@call_targets, @functions);
open LISTING, "$disassemble $dflags $specimen 0 |" or die "disassembly failed\n";
while (<LISTING>) {
    if (my($va,$call_target) = /^(0x[0-9a-f]+).*\s+(?:far)?call\s+(0x[0-9a-f]+)/) {
	push @call_targets, hex $call_target;
    } elsif (my($va) = /^(0x[0-9a-f]+).*\bFunction /) {
	push @functions, hex $va;
    }
}
close LISTING or die "disassembly failed\n";
print STDERR "number of calls:     ", 0+@call_targets, "\n";
print STDERR "number of functions: ", 0+@functions, "\n";


# Cross product difference of calls and functions
my %diffs;
for my $function (@functions) {
    for my $call_target (@call_targets) {
	if ($call_target >= $function) {
	    my $diff = $call_target - $function;
	    $diffs{$diff}++;
	}
    }
}

# Print 2d points
for my $function (@functions) {
    for my $call_target (@call_targets) {
	if ($call_target >= $function) {
	    my $diff = $call_target - $function;
	    print join("\t", $function, $call_target, $diffs{$diff}), "\n";
	}
    }
}





## # Show differences sorted by number of times that difference occurred.
## my %inverted_diffs;
## for my $diff (keys %diffs) {
##     my $ntimes = $diffs{$diff};
##     $inverted_diffs{$ntimes} ||= [];
##     push @{$inverted_diffs{$ntimes}}, $diff;
## }
## for my $ntimes (sort keys %inverted_diffs) {
##     for my $diff (@{$inverted_diffs{$ntimes}}) {
## 	print $diff, "\t", $ntimes, "\n";
##     }
## }





exit 1
