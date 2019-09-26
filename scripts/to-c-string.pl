#!/usr/bin/perl
# Reads text from standard input and prints it as a C string to standard output.
use strict;

sub escape {
    my($char) = @_;
    return "\\\\" if $char eq "\\";
    return "\\\"" if $char eq '"';
    return "\\t" if $char eq "\t";
    return "\\n" if $char eq "\n";
}

while (<STDIN>) {
    s/([\\\t\n"])/escape($1)/ge;
    print "\"$_\"\n";
}
