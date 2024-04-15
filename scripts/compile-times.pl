#!/usr/bin/perl -w
#
# How long does it take to compile each translation unit?
#
# This script parses the Tup output to gather information about what C files were compiled and how long it took for each one.
use strict;

sub print_dirinfo {
    if (@_) {
	my($info) = @_;
	printf("%7.2fs %5d %8.2fs %7.2fs %7.2fs %s\n",
	       $info->{average},
	       $info->{n},
	       $info->{total},
	       $info->{min},
	       $info->{max},
	       $info->{directory});
    } else {
	printf("%8s %5s %9s %8s %8s %s\n", "Average", "N", "Total", "Min", "Max", "Directory");
	print("-------- ----- --------- -------- -------- --------------------------------------------------------------\n");
    }
}

sub print_action {
    if (@_) {
	my($action) = @_;
	printf("%6.2fs %-12s %s\n", $action->{duration}, $action->{command}, $action->{filename});
    } else {
	printf("%7s %-12s %s\n", "Time", "Command", "File");
	print("------- ------------ ------------------------------------------------------------------\n");
    }
}

sub fix_filename {
    my($name) = @_;
    my(@parts) = split /\//, $name;
    my(@result);

    unshift @parts if $parts[0] eq '_build';

    for my $part (@parts) {
	if ($part eq '.') {
	    # skip
	} elsif ($part eq '..' && @result) {
	    pop @result;
	} else {
	    push @result, $part;
	}
    }
    return join '/', @result;
}
	    

####################################################################################################################################

# Parse information about build actions from the Tup output
my(@actions);
while (<>) {
    #-----------------------------------------------------step   duration       dir     cmd   file 
    if (my($duration, $directory, $command, $filename) = /\d+\) \[(\d+\.\d*)s\] (\S+?): (\S+) (\S+)/) {
	next unless $command =~ /_CXX$/;
	my($action) = {filename => fix_filename("$directory/$filename"), command => $command, duration => $duration + 0.0};
	push @actions, $action;
    }
}
print "number of build actions parsed: ", scalar(@actions), "\n";

# Group actions by directory
my(%directories);
for my $action (@actions) {
    my @path = split /\//, $action->{filename};
    pop @path;
    while (@path) {
	my($dir) = join '/', @path;
	$directories{$dir} ||= {n => 0, duration => 0.0, min => $action->{duration}, max => $action->{duration}};
	$directories{$dir}{n} += 1;
	$directories{$dir}{duration} += $action->{duration};
	$directories{$dir}{min} = $action->{duration} if $action->{duration} < $directories{$dir}{min};
	$directories{$dir}{max} = $action->{duration} if $action->{duration} > $directories{$dir}{max};
	pop @path;
    }
}

# Per-directory information
my(@dirinfo);
for my $dir (keys %directories) {
    push @dirinfo, {directory => $dir, n => $directories{$dir}{n}, total => $directories{$dir}{duration},
		    min => $directories{$dir}{min}, max => $directories{$dir}{max},
		    average => $directories{$dir}{duration} / $directories{$dir}{n}};
}


# Directories by average elapsed time per command
print "\ntime spent in each directory:\n";
print_dirinfo;
print_dirinfo $_ for sort {$a->{average} <=> $b->{average}} @dirinfo;

# Sort and show actions by duration
print "\nbuild actions by elapsed time:\n";
print_action;
print_action $_ for sort {$a->{duration} <=> $b->{duration}} @actions;

