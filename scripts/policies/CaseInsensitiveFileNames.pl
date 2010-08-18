#!/usr/bin/perl
my $desc = <<EOF;
Some file systems do not distinguish between upper and lower case characters in
a file or directory name.  The following directories have files whose names
would conflict on such a filesystem:
EOF


use strict;
my $nfail=0;

sub checkdir {
  my($dir) = @_;
  opendir DIR, $dir or return;
  my @entries = readdir DIR;
  close DIR;

  # Build the index hash. Each key is the lower-case name; each value is an array of names with this key
  my %index;
  for my $entry (@entries) {
    next if $entry =~ /^\.\.?$/;
    checkdir("$dir/$entry") if -d "$dir/$entry" && ! -d "$dir/$entry/include_staging";
    my $key = lc $entry;
    $index{$key} ||= [];
    push @{$index{$key}}, $entry;
  }

  my @failures = grep {@$_>1} values %index;
  if (@failures) {
    print $desc unless $nfail++;
    print "  $dir (", join(" ", @$_), ")\n" for @failures;
  }
}

checkdir ".";
exit($nfail>0 ? 1 : 0);

