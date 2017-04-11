#!/usr/bin/perl
use strict;

# Create a histogram showing how many warnings come from each directory.
# Usage: Pipe the standard error of build commands to this file.
# Bourne-shell example: make 2>&1 |this_script
#
# The default output is sorted by directory/file name. To get output sorted by number of warnings instead,
# run it like this: make 2>&1 |this_script |sort -nrs

########################################################################################################################
# Returns a tuple which is a file name and the warning message if the line looks like a warning.

sub is_warning {
    my($line) = @_;

    $line =~ /(.+?):\d+:\d+: warning: (.*)/ and return ($1,$2);	    	# GCC and LLVM warnings with line and column numbers
    $line =~ /(.+?):\d+: warning: (.*)/ and return ($1,$2);         	# GCC and LLVM warnings with only line numbers
    $line =~ /(.+?)\(\d+\): warning #\d+: (.*)/ and return ($1,$2); 	# Intel warnings
    $line =~ /(.+?):\d+-(\d+\.)?\d+: warning: (.*)/ and return ($1,$2); # yacc location range: L1.C1-C2 or L1.C1-L2.C2
    $line =~ /(.+\.yy): warning: (.*)/ and return ($1,$2);              # yacc file name w/out location

    # Some tool-specific warnings that lack location information
    $line =~ /^clang: warning: (.*)/ and return ("unknown","clang $1");
    $line =~ /^libtool: warning: (.*)/ and return ("unknown","libtool $1");

    return ();
}

########################################################################################################################
# Given the file name field of a warning message, return a list of suitable keys for the location histogram.

sub location_keys {
    my($file_name) = @_;
    my @retval;

    my @parts = grep {$_ ne ".." && $_ ne ""} split /\/+/, $file_name;
    while (@parts) {
	push @retval, join "/", @parts;
	pop @parts;
    }

    return @retval;
}

########################################################################################################################
# Emit info about where the errors are located.

sub count_locations {
    my %histogram;
    while (<>) {
	if (my($file,$warning) = is_warning $_) {
	    ++$histogram{$_} for location_keys $file;
	    ++$histogram{total};
	}
    }

    print $histogram{$_}, "\t", $_, "\n" for sort keys %histogram;
}

########################################################################################################################
# Given a warning message, clean it up so it can be categorized.

sub clean_message {
    local($_) = @_;

    s/'[^']+'/'...'/g;		# remove single-quoted content
    return $_;
}

########################################################################################################################
# Emit info about the types of errors.

sub count_types {
    my %histogram;
    while (<>) {
	if (my($file,$mesg) = is_warning $_) {
	    ++$histogram{clean_message $mesg};
	    ++$histogram{total};
	}
    }

    print $histogram{$_}, "\t", $_, "\n" for sort keys %histogram;
}

########################################################################################################################

# Parse arguments
my $action = \&count_locations;
while (@ARGV) {
    if ($ARGV[0] eq '--') {
	shift @ARGV;
	last;
    } elsif ($ARGV[0] =~ /^-?-locations?$/) {
	$action = \&count_locations;
	shift @ARGV;
    } elsif ($ARGV[0] =~ /^-?-types?$/) {
	$action = \&count_types;
	shift @ARGV;
    } elsif ($ARGV[0] =~ /^-/) {
	die "$0: invalid switch: $ARGV[0]\n";
    } else {
	last;
    }
}

&$action;
exit 0;
