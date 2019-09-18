#!/usr/bin/perl
# DO NOT DISABLE without first checking with a ROSE core developer
my $desc = <<EOF;
Some file systems do not distinguish between upper and lower case characters in
a file or directory name.  Having two files with equal case insensitive names
causes problems with certain Git operations (like clone and merge) because Git
thinks it has created a file with a certain checksum, but the checksums don't
match when they're rechecked later.  This checker runs in all subdirectories
including tests and projects since violations could cause Git to not work.
The following directories have files whose names would conflict on such a
filesystem:
EOF


use strict;
my $warning;
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
    next if $entry =~ /^\.git$/; # Git can handle its own problems
    next if $entry =~ /^_build/; # typical names for build directories
    next if $entry =~ /^build_tree$/; # Jenkins' build directory
    next if $entry =~ /~$/; # editor backup file
    checkdir("$dir/$entry") if -d "$dir/$entry" && ! -d "$dir/$entry/include_staging";
    my $key = lc $entry;
    $index{$key} ||= [];
    push @{$index{$key}}, $entry;
  }

  my @failures = grep {@$_>1} values %index;
  if (@failures) {
    print $desc unless $nfail++;
    print "  $dir ($warning", join(" ", @$_), ")\n" for @failures;
  }
}

checkdir ".";
exit($nfail>0 ? ($warning?128:1) : 0);

