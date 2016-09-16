#!/usr/bin/perl
use strict;
while (<ARGV>) {
    # Ignore if-def stuff for now
    s/defs=\{(0x[0-9a-f]{8}(,0x[0-9a-f]{8})*)?\}/defs={X}/g;

    # separate defs from expr with a comma
    s/(defs=\{.*?\}) expr=/$1, expr=/;

    # split memory cells into multiple lines
    s/^(\s+)addr=(.*?) value=(.*)/$1address = $2\n$1  value = $3/;

    # remove memory "flags" field and "init mem" line
    next if /^\s*flags =/;
    next if /^init mem:/;

    # Remove extra level of braces
    s/\{ \{/{/g;
    s/\} \}/}/g;
    s/{\s+/{/g;
    s/\s+\}/}/g;

    # Replace variable names with something generic
    s/\bv\d+/VAR/g;
    s/\bm\d+/MEM/g;

    print
}
