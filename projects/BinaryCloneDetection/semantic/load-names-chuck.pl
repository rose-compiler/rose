#!/usr/bin/perl
# Input file contains four tab-separated columns:
#    function entry address (hexadecimal)
#    is_a_thunk ("True" or "False")
#    mangled name
#    non-mangled name as reported by IDA or "None"
#    
use strict;

die "usage: $0 SPECIMEN_ID INPUT_FILE" unless @ARGV==2;
my $specimen_id = shift;
print "drop index if exists semantic_functions_spec_ent;\n";
print "create index semantic_functions_spec_ent on semantic_functions(specimen_id, entry_va);\n";

while (<ARGV>) {
    my($addr,$name) = (split /\t/, $_, 4)[0,2];
    next if $name eq "" || $name eq "None";
    $addr = hex($addr);

    print "update semantic_functions set name = '$name' where specimen_id = $specimen_id and entry_va = $addr;\n";
}
print "drop index semantic_functions_spec_ent;\n";
