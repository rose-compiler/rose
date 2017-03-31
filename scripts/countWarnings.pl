#!/usr/bin/perl
use strict;

# Create a histogram showing how many warnings come from each directory.
# Usage: Pipe the standard error of build commands to this file.
# Bourne-shell example: make 2>&1 |this_script
#
# The default output is sorted by directory/file name. To get output sorted by number of warnings instead,
# run it like this: make 2>&1 |this_script |sort -nrs

########################################################################################################################
# Returns the file name if this line looks like a warning message.

sub is_warning {
    my($line) = @_;

    $line =~ /(.*?):\d+:\d+: warning:/ and return $1; 	  # GCC and LLVM warnings with line and column numbers
    $line =~ /(.*?):\d+: warning:/ and return $1;      	  # GCC and LLVM warnings with only line numbers
    $line =~ /(.*?)\(\d+\): warning #\d+:/ and return $1; # Intel warnings
    return undef;
}

########################################################################################################################
# Given the file name field of a warning message, return a list of suitable keys.

sub histogram_keys {
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

my %histogram;
while (<>) {
    if (my $file = is_warning $_) {
	++$histogram{$_} for histogram_keys $file;
	++$histogram{total};
    }
}

print $histogram{$_}, "\t", $_, "\n" for sort keys %histogram;
exit 0;
