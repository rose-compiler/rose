#!/usr/bin/perl
my $desc = <<"EOF";
ASCII TAB characters appearing in source code are interpreted to mean "indent
to the next column which is a multiple of eight."  Editor choice and indentation
tend to be religious matters for many programmers, yet editors have differing
capabilities when it comes to TAB characters, tabstops, and the keyboard button
labeled "Tab" [see http://www.jwz.org/doc/tabs-vs-spaces.html].  Therefore,
if a TAB character appears in a source file, it must be possible to interpret
its meaning consistently. The following files appear (heuristically) to violate
this policy.  You may rerun this script as "@{[$0=~/([^\/]+)$/]} --verbose"
to see a list of lines that appear to violate this policy.  To fix a line,
replace the TAB characters with the appropriate number of SPC characters based
on the amount of line indentation desired. The files in possible volation are:
EOF

BEGIN {push @INC, $1 if $0 =~ /(.*)\//}
use strict;
use FileLister;
use Text::Tabs;

my $verbose;
while ($ARGV[0] =~ /^-/) {
  if ($ARGV[0] eq "--") {
    shift @ARGV;
    last;
  } elsif ($ARGV[0] eq "--verbose") {
    $verbose = 1;
    shift @ARGV;
  } else {
    die "$0: unknown switch: $ARGV[0]\n";
  }
}

# Return the total counts represented by the delta hash
sub total {
  my($deltas) = @_;
  my $total = 0;
  $total += $_ for values %$deltas;
  return $total;
}

# Show a histogram
sub show {
  my($deltas) = @_;
  my $total = total($deltas);
  for my $delta (sort {$a <=> $b} keys %$deltas) {
    my $pct = 100 * $deltas->{$delta} / $total;
    printf "  %3d %4d (%3d%%) %1s\n", $delta, $deltas->{$delta}, $pct, '-' x int($pct);
  }
}

# Basic tab width is the lowest SPC delta with at least 40% occurrence
sub base_indentation {
  my($deltas) = @_;
  my $total = total($deltas);
  for my $delta (sort {$a <=> $b} keys %$deltas) {
    return $delta if $deltas->{$delta} / $total > 0.4;
  }
  return 1;
}

my $nfail=0;
my $files = FileLister->new(@ARGV);
while (my $filename = $files->next_file) {
  if ($filename =~ /\.(h|hh|c|cpp|C)$/ && open FILE, "<", $filename) {

    # Determine indentation delta distributions for space-only indentation vs. tab indentation
    my($indent,$nspc,$ntab,%spc_deltas,%tab_deltas);
    while (<FILE>) {
      my $line = expand($_); # uses 8-column tab-stops
      my $actual = length $1 if $line =~ /^(\s*)\S/;
      my $delta = abs $indent-$actual;
      $indent = $actual;
      if ($delta) {
	if (/^\s*?\t/) {
	  $nspc++;
	  $tab_deltas{$delta}++;
	} else {
	  $ntab++;
	  $spc_deltas{$delta}++;
	}
      }
    }
    close FILE;

    # Base indentation is computed from spaces since spaces are a known width and generally more common than tabs.
    # However, if less than 5% of the lines are SPC indented then abandon the policy checker--there is probably not
    # enough data to make the determination.
    next if $ntab==0;
    next if $nspc / ($nspc+$ntab) < 0.05;
    my $base = base_indentation \%spc_deltas;
    if (0) {
      print $filename, ":\n";
      print "  base indentation = ", $base, "\n";
      show \%spc_deltas if $base < 4;
      #show \%tab_deltas;
    }

    # Does file violate this policy?
    my $nviolations=0;
    for my $delta (sort {$a <=> $b} keys %tab_deltas) {
      $nviolations += $tab_deltas{$delta} if $delta % $base;
    }
    if (!$verbose && $nviolations>0) {
      print $desc unless $nfail++;
      print "  $filename (base=$base, violations=$nviolations)\n";
    }


    # Look for TAB indentation that is not a multiple of the base indentation.
    if ($verbose && $nviolations) {
      open FILE, "<", $filename or die;
      $indent = 0;
      my $linenum=0;
      while (<FILE>) {
	$linenum++;
	my $line = expand($_);
	my $actual = length $1 if $line =~ /^(\s*)\S/;
	my $delta = abs $indent-$actual;
	$indent = $actual;
	if ($delta && /^\s*?\t/ && $delta % $base) {
	  if ($verbose) {
	    $nfail++;
	    print "$filename:$linenum: TAB-based indentation is not a multiple of $base\n";
	  } else {
	    print $desc unless $nfail++;
	    print "  $filename\n";
	    last;
	  }
	}
      }
      close FILE;
    }
  }
}
exit($nfail>0 ? 1 : 0);
