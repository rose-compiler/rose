#!/usr/bin/perl
use strict;

my $mode = 'semantic';

die "usage: $0 SPECIMEN_ID SPECIMEN_MD5 CSV_FILE" unless @ARGV==3;
my $specimen_id = shift;
my $specimen_md5 = shift;
print "drop index if exists cory_ld_names;\n";
if ($mode eq 'semantic') {
    print "create index cory_ld_names on semantic_functions(specimen_id, entry_va);\n";
} else {
    print "create index cory_ld_names on function_ids(file, entry_va);\n";
}
while (<ARGV>) {
    my($md5,$addr,$name) = (split /,/, $_, 4)[0,1,2];
    next unless $md5 eq $specimen_md5;
    next if $name eq "" || $name eq "None";
    $addr = hex($addr);

    if ($mode eq 'semantic') {
	print "update semantic_functions set name = '$name' where specimen_id = $specimen_id and entry_va = $addr;\n";
    } else {
	print "update function_ids set function_name = '$name' where file = '$specimen_id' and entry_va = $addr;\n";
    }

}
print "drop index cory_ld_names;\n";
