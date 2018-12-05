#!/usr/bin/perl
use strict;

my $show_individual_files = 0;
my $show_zero_warnings = 1;

# Number of warnings per file
sub count_warnings {
    my %warnings;
    while (<ARGV>) {
        if (my($file) = /^(\S+?):\d+(:\d+)?:\s+warning:/) {
            $warnings{$file}++;
        }
    }
    return %warnings;
}

# Find the top of the ROSE source tree. This won't work if there are no warnings from ROSE!
sub rose_source_tree {
    my($warnings) = @_;
    my(%srcdirs,$maxwarns,$maxfile);
    for my $file (keys %{$warnings}) {
        if ($file =~ /\/src\//) {
            $srcdirs{$file}++;
        }
    }
    for my $file (keys %srcdirs) {
        if ($srcdirs{$file} > $maxwarns) {
            ($maxwarns, $maxfile) = ($srcdirs{$file}, $file);
        }
    }
    die "no warnings" unless defined $maxwarns;
    $maxfile =~ /(.*?)\/src\//;
    return "$1/";
}

# Count the number of lines of source files in each directory
sub count_source_lines {
    my($rose_source_tree) = @_;
    my(%nlines);
    open LINES, "-|", "find $rose_source_tree \\( -name '*.[Ch]' -o -name '*.[ch]pp' \\) -print0 |xargs -0 wc -l"
        or die "cannot run find command";
    while (<LINES>) {
        chomp;
        my($nlines, $file) = /^\s*(\d+)\s*(.*)/;
        $nlines{$file} = $nlines;
    }
    close LINES;
    return %nlines;
}

# Remove common prefix from all directory names
sub remove_prefix {
    my($counts, $prefix) = @_;
    my(%result);
    for my $key (keys %{$counts}) {
        if (substr($key,0,length $prefix) eq $prefix) {
            $result{substr($key,length $prefix)} += $counts->{$key};
        } else {
            $result{$key} += $counts->{$key};
        }
    }
    %{$counts} = %result;
}

sub accumulate {
    my($counts) = @_;
    my(%result);
    for my $file (keys %{$counts}) {
        my @comps = split "/", $file;
        pop @comps unless $show_individual_files;
        while (@comps) {
            my($dir) = join "/", @comps;
            $result{$dir} += $counts->{$file};
            pop @comps;
        }
    }
    return %result;
}

my %warnings = count_warnings;
my $rose_srcdir = rose_source_tree \%warnings;
remove_prefix \%warnings, $rose_srcdir;
my %lines = count_source_lines $rose_srcdir;
remove_prefix \%lines, $rose_srcdir;
my %aw = accumulate \%warnings;
my %al = accumulate \%lines;

my %all_entities;
for my $entity (keys %aw) { $all_entities{$entity} = 1 }
for my $entity (keys %al) { $all_entities{$entity} = 1 }

my(@totals,$maxratio);
for my $dir (keys %all_entities) {
    my $ratio;
    if ($al{$dir} > 0) {
        $ratio = $aw{$dir} / $al{$dir};
        $maxratio = $ratio if $ratio > $maxratio;
    }
    push @totals, { level => $dir =~ tr(/)(/),
                    nwarns => $aw{$dir},
                    nlines => $al{$dir},
                    ratio  => $ratio,
                    name   => $dir };
}
print STDERR "maxratio=$maxratio\n";

sub sort_by_name {
    return $a->{name} cmp $b->{name};
}

sub sort_by_ratio {
    return $b->{ratio} <=> $a->{ratio};
}

sub sort_for_zeros {
    return $a->{nwarns} <=> $b->{nwarns} if $a->{nwarns} != $b->{nwarns};
    return $b->{nlines} <=> $a->{nlines} if $a->{nlines} != $b->{nlines};
    return $a->{ratio} <=> $b->{ratio} if $a->{ratio} != $b->{ratio};
    return $a->{level} <=> $b->{level} if $a->{level} != $b->{level};
}

printf "%5s\t%8s\t%8s\t%-15s\t%s\n", "NWarn", "LOC", "per 1000", " BarChart", "Directory";
for my $total (sort sort_for_zeros @totals) {
    next unless $show_zero_warnings || $total->{nwarns};
    my $barlen = int(13 * $total->{ratio} / $maxratio + 0.5);
    die if $barlen > 13;
    my $bar = '=' x $barlen;
    printf "%5d\t%8d\t%8.3f\t|%-13s|\t%s\n", $total->{nwarns}, $total->{nlines}, 1000*$total->{ratio}, $bar, $total->{name};
}
