# Support functions for all policies.
use strict;

# A policy checker can be disabled by adding a line containing the following text near the
# beginning (first 10 lines) of the file.
#
#    Skip policies: NAMES...
#
# where NAMES is a comma and/or white-space separated list of policy names. The policy names
# do not need the ".pl" extension. The text may be surrounded by other things, such as comment
# tokens.
sub is_disabled_in_file {
  my($filename,$policy) = @_;

  open TEXT, "<", $filename or return undef;

  local($_);
  my($linenum, $retval) = (1, undef);

  while (<TEXT>) {
    if (my($policy_list) = /\b[Ss]kip [Pp]olic(?:ies|y):\s+(.*)/) {
      for my $skip (split /[,\s]+/, $policy_list) {
	$skip =~ s/\.pl$//;
	if ($skip eq $policy) {
	  $retval = 1;
	  last;
	}
      }
    }
    last if $linenum++ >= 10;
  }
  close TEXT;
  return $retval;
}

# Policies can be controlled by a file named "PolicyControl.txt"
# Each line is one of:
#    blank (or all white space)
#    comment (first non-white character is '#', and continues to EOL)
#    VARIABLE = VALUE
#    FILENAME: disable POLICIES...
#
# Where
#    FILENAME is the name of a file in the directory (excluding directory components)
#    POLICIES is a comma and/or space separated list of policy names
#    VARIABLE = VALUE assigns a value to a variable lasting until a new value is given. The
#       variables and values are:
#         user = FULL_NAME_OF_USER # the user responsible for disabling policies
#
# The $specs is a reference to a hash whose keys are file names with path components (formed by
# joining the supplied $dir with the FILENAME) and the values are hash references, each of which
# has the following keys:
#    disabled: a reference to an array of disabled policy names
#    user: the name of the user responsible for disabling the policy
#
# Once a directory's PolicyControl.txt file is parsed (or we determine that there is no such file)
# the $specs hash will have a directory name key whose value is Boolean true or false depending on
# whether the PolicyControl.txt file existed or not.
sub parse_policy_control {
  my($dir,$specs) = @_;
  die unless 'HASH' eq ref $specs;
  my($policy_file,$linenum) = ($dir||".") . "/PolicyControl.txt";
  my(%varvals);
  $specs->{$dir} = 0;
  open PC, "<", $policy_file or return;
  while (<PC>) {
    $linenum++;
    next unless /\S/; # blank lines
    next if /^\s*#/; # comments
    if (my($variable,$value) = /^\s*([a-zA-Z][-_a-zA-Z0-9]*)\s*=\s*(.*?)\s*$/) {
	if ($value eq "") {
	    delete $varvals{$variable};
	} else {
	    $varvals{$variable} = $value;
	}
    } elsif (my($filename,$disabled_list) = /^\s*(\S+?):\s*disable\s+(.*?)\s*$/) {
      die "$policy_file:$linenum: error: file name must not include path components\n" if $filename =~ /\//;
      $filename = "$dir/$filename";
      $specs->{$filename} ||= {};
      $specs->{$filename}{$_} = $varvals{$_} for keys %varvals;
      $specs->{$filename}{disabled} ||= [];
      push @{$specs->{$filename}{disabled}}, split /[,\s]+/, $disabled_list;
    } else {
      die "$policy_file:$linenum: error: unknown policy specification\n";
    }
  }
  close PC;
  $specs->{$dir} = 1;
  return 1;
}

my %dir_policies;

# Returns a pair consisting of a boolean value indicating whether the policy is disabled in the specified file, and a reference
# to a hash of extra information (the variable=value pairs from the config file).
sub is_disabled_in_dir {
  my($filename, $policy) = @_;
  my($dir) = $filename =~ /(.*)\//;
  parse_policy_control($dir, \%dir_policies) unless exists $dir_policies{$dir};
  return unless 'HASH' eq ref $dir_policies{$filename};

  my($is_disabled) = scalar grep {$_ eq $policy} @{$dir_policies{$filename}{disabled}};
  return ($is_disabled, $dir_policies{$filename});
}

sub is_disabled {
  my($filename) = @_;
  my($policy) = $0 =~ /([^\/]+?)(\.pl)?$/;
  return 1 if is_disabled_in_file($filename, $policy);
  my($disabled, $extra) = is_disabled_in_dir($filename, $policy);
  return $disabled;
}

1;
