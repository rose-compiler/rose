#!/usr/bin/perl
use strict;

# Returns the warning message (part after "warning:") if a line is a compiler warning
sub is_warning {
    my($line) = @_;

    $line =~ /(.*?):\d+:\d+: warning:\s*(.*)/ and return $2; 	  # GCC and LLVM warnings with line and column numbers
    $line =~ /(.*?):\d+: warning:\s*(.*)/ and return $2;      	  # GCC and LLVM warnings with only line numbers
    $line =~ /(.*?)\(\d+\): warning #\d+:\s*(.*)/ and return $3;  # Intel warnings
    return undef;
}

# Cleans up warning messages, e.g., by removing things in quotes
sub clean {
    local($_) = @_;

    s/'[^']+'/'...'/g;
    return $_;
}

# Counts types of compiler warnings
my %histogram;
while (<>) {
    if (my $mesg = is_warning $_) {
	++$histogram{clean $mesg};
	++$histogram{total};
    }
}

print $histogram{$_}, "\t", $_, "\n" for sort keys %histogram;
exit 0
