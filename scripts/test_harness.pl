#!/usr/bin/perl
=pod

=head1 NAME

test_harness - harness for running ROSE tests from a makefile

=head1 SYNOPSIS

test_harness [SWITCHES...] [VAR=VALUE...] CONFIG_FILE MAKE_TARGET

=head1 FEATURES

=over

=item *

The existing build system is used as much as possible. This means, for instance, the the makefiles will control the parallelism
of the tests.  However, some of the details can be moved into separate configuration files which allow the configuration to be
adjusted without necessitating calls to the "build" or "configure" scripts.

=item *

Tests can be allowed to temporarily fail. A developer can mark a test as "may-fail" in order to progress with other tests in
possibly other directories. A "may-fail" test can be automatically promoted to a "must-pass" test once it begins to pass.
Tests can also be completely disabled, in which case the test harness doesn't even attempt to run the test, but emits a
reminder that the test has been disabled.

=item *

Tests may share configuration files, including the ability to mark individual tests as "may-fail" (and allowing them to promote
back to the "must-pass" state).  Test-specific parameters can be passed into the configuration file by specifying them on the
test harness command line.  This allows directories with large numbers of very similar tests to be configured with just a
couple files.

=item *

Flexibility in specifying commands that must run for the test. The commands are specified as a list in the configuration
file. Since variables are expanded in the file, it's possible to even specify the command in the makefile itself.  Commands are
generally required to exit with zero status, but this requirement can be suppressed or inverted.

=item *

Features for comparing output with pregenerated answers.  The configuration file can specify where the pregenerated answer is
located, how to compare the output with the answer, and whether any filters are required before the comparison is made.

=item *

Limited output. When tests succeed, the test harness emits only their names. When a test fails, its output is prefixed with the
name of the test so that output from parallel testing is more understandable.  In either case, the output is also saved in a
file (the "target" file with a suffix of ".passed" or ".failed").

=back

=head1 DETAILS

Each test run by a makefile has its own target, a file with the extension ".passed", which will be created only if the test
passes (it is removed if the test fails, and failing output is saved in a file with a ".failed" extension).  The target name is
specified to this test harness via its second, optional argument.  If the second argument is not present, then the result files
are created in the current working directory having the same base name as the first argument. The first argument is the name of
a configuration file. The configuration file lists the commands that must be run, and how to determine whether the test passed
or failed.  Configuration files generally have a ".conf" extension.

=head1 CONFIGURATION

Configuration files contain blank lines, comment lines, or property settings.  Comments begin with a hash character ("#") and
continue to the end of the line.  Settings consist of a property name, followed by an equal sign, followed by the property
value. White space may appear before the property name, on either side of the equal sign, and after the value. Some properties
are allowed to appear more than once and are treated as arrays.

Properties may contain variables of the form ${NAME}, which are expanded to the value defined on the command line.

The following properties are possible:

=over

=item answer = no | yes | FILE

If specified, then the standard output from the command must match the content stored in the specified file. The match is
determined by running the command specified by the "diff" property.  The default, "no", means that the success or failure of
the test rests entirely on the exit status of the test commands.

If the value is the word "yes" then the name of the answer file is the same as the configuration file except with the extension
".ans".

The standard output and predefined answers can be filtered before the comparison (see the "filter" property) and the default
comparison can be modified (see the "diff" property).

=item cmd = SHELL_COMMAND

The shell commands to run for the test.  This is an array property--commands are run in the order specified and all commands
must exit with a zero status.  The standard output and standard error of the commands are both captured and the standard output
can be compared with a predetermined answer if so desired (see the "answer" property).

When multiple commands are specified on the same line they will be run in a single command shell. Commands on different lines
of the config file will be run in separate shells.  In other words, a variable setting or change of directories applies only to
a single line.

=item diff = DIFF_COMMAND

This property specifies a program (and options) to use when comparing the standard output of a test with a predetermined
answer.  The program will be invoked with two arguments: the name of the file containing the predetermined answer (see the
"answer" property), and the name of the file containing the test's standard output. The diff program should exit with with zero
status if the files are identical, non-zero if they differ. Any output produced by this command will be emitted to standard
output and placed in the failure target file.  The default is "diff -u".

=item disabled = no | COMMENT

A test may be temporarily disabled by setting this property to a value other than the word "no". When the test runs, a comment
will be produced to indicate that the test is disabled and COMMENT will appear as part of that text. This is useful in
conjunction with the may_fail property below because it allows conditional parts of a makefile to disable the test without
affecting whether the test is a failing or passing test.

=item filter = no | COMMAND

Before output is compared with a predetermined answer, both the output and the answer are fed through an optional filter. The
filter should read from standard input and write to standard output.

=item may_fail =  no | yes | promote | FILE[:DEFAULT]

Specifies whether the test is allowed to fail.  If the value is "promote" then the test is allowed to fail, but once it
succeeds the test harness will change this property setting to "no" in the configuration file if possible. The default is "no".

If a file is specified, the file should contain the may_fail values for multiple test names. Each line will be a test name
followed by white space, followed by the word "no", "yes", or "promote".  The test harness protects access to this file with a
mutex so that multiple tests can run concurrently.  If the target is not found in the file then its value is assumed to be
either "no" or the supplied default.

If a "may-fail" test fails, the output will be stored in the *.failed target (same as other failing tests), and a symbolic link
will be created to the "*.passed" target.  This allows these tests to be found easily after make is finished.  Since these
tests are expected to fail, their output is not emitted when they do so (this is like passing tests). However, the test harness
does emit a single line message to indicate that the failure was ignored.

=back

=head1 VARIABLES

Variables can be specified on the command-line and are expanded on all lines of the configuration file.  A variable is
specified on the command line as "VAR=VALUE" and is referenced in the configuration file as ${VAR}.  Additionally, the
following variables are supplied by the test harness:

=over

=item TEMP_FILE_0, TEMP_FILE_1, ... TEMP_FILE_9

Names of temporary files available to the script and which will be deleted at the end of the test.

=item CONFDIR

Name of the directory containing the configuration file.

=item CONFIG

File name of the configuration file without directory components.

=item TARGET

The name of the target without directory components and without the ".passed" or ".failed" extension.

=item srcdir, top_srcdir, VALGRIND, BINARY_SAMPLES, ...

These variables, and perhaps others, are passed to all invocations of the test harness when run from ROSE makefiles with the
$(TH) variable.

=back

=head1 MAKEFILE ISSUES

Each test has a makefile target. If the test is named "foo" then the target is "foo.passed" and this file is created when the
test passes.  A number of configuration variables are passed to the test harness and the harness script and standard variables
together are stored in the $(TH) makefile variable.

A simple makefile rule for a single test, depending on the configuration file stored in the source tree ("foo.conf"), and an
executable stored in the build tree ("foo"). When the base name of the target is the same as the base name of the configuration
file, the $@ argument is not necessary.

    foo.passed: foo.conf foo
            @$(TH) $< $@

A more complex example uses a single configuration file for multiple tests and distinguishes between the tests by setting
variables on the command line.  This can only be done if the configuration file doesn't use the "may_fail=promote" property.

    EXECUTABLES=e1 e2 e3 e4 e5 e6
    TESTS=$(addsuffix .passed, $(EXECUTABLES))

    $(TESTS): %.passed: % main.conf
            @$(TH) EXE=$< main.conf $@

=head1 TIPS AND TRICKS

If multiple tests have very similar configuration files, it's possible to write a single configuration file that's shared by
all the related tests.  Variations in the configuration can be passed as variables set on the command line. This can only be
done safely if the configuration file does not use the "may_fail=promote" property.

When sharing configuration files among many tests, one can still use the automatic promoting from "may fail" to "must pass",
but the information about what files pass and fail should be stored outside the configuration file.  This is possible by
supplying a file name for the "may_fail" property. See above.

When using the "may_fail=promote" property, one can determine which tests have been promoted from "may fail" to "must pass"
status by asking git about which configuration files have changed.

Sometimes the category of test ("may-fail" or "must-pass") depends on configuration information, such as which version of a
library is used.  There are two ways to handle this: place the configuration files in a directory whose name depends on the
configuration information, or have one set of configuration files whose may_fail properties point to a file whose name depends
on the configuration information.  The latter approach reduces cut-n-paste in the configuration files, but can result in more
contention for the may_fail properties file in parallel runs.

Normally, the test harness will abort the test if one of the commands in the configuration file exits with a non-zero
status.  To avoid this, append "; true" to the end of the command.  It's also possible to invert the sense of the failure
by appending "; test $? -ne 0" to the command.

To re-run all tests, remove the *.passed files and run "make check".

To re-run tests that failed but whose failure was ignored (i.e., the "may-fail" or "promote" tests), remove all the *.failed
files and run "make check".  Removing these *.failed files results in the corresponding *.passed files being dangling symbolic
links, which "make" will treat as being out of date.

=head1 BUGS

No attempt is made to discover whether multiple concurrent tests try to update a single config file at the same time (when a
test is promoted from "may-fail" to "must-pass").  When concurrent tests share a single config file and use this auto-promotion
feature, they must specify a separate file to contain the state of each target. See the "may_fail" property above.

File locking depends on being able to created multiple hard links to a file. If this is not possible then the locking mechanism
might fail to behave properly.

Output from the test is organized in such a way that the standard output all appears before the standard error. It would be
more intuitive if the file contained output in more or less the order it was generated.

Output is left in a *.passed or *.failed file. This makes it difficult to write makefile rules for tests that don't depend on
each other's success or failure, but must depend on each other in order to achieve serial execution.

=head1 AUTHOR

Robb Matzke.

Copyright Lawrence Livermore National Security

Licensed under Revised BSD License (see COPYRIGHT file at top of ROSE source code)

=cut

use Time::HiRes 'usleep';
use strict;

my $usage = <<EOF;
usage: $0 [VAR=VALUE...] CONFIG TARGET
       $0 --help
EOF

sub help {
  local $_ = `(pod2man $0 |nroff -man) 2>/dev/null` ||
	     `pod2text $0 2>/dev/null` ||
	     `sed -ne '/^=pod/,/^=cut/p' $0 2>/dev/null`;
  print $_;
  die "$0: see source file for documentation" unless $_;
};


# Parse specified configuration file or die trying. Return value is a hash whose keys are the variable names and whose
# values are the values or an array of values.
sub load_config {
  my($file,%vars) = @_;
  my(%conf) = (answer=>'no', cmd=>[], diff=>'diff -u', disabled=>'no', filter=>'no', may_fail=>'no');
  open CONFIG, "<", $file or die "$0: $file: $!\n";
  while (<CONFIG>) {
    s/\s*#.*//;
    s/\$\{(\w+)\}/exists $vars{$1} ? $vars{$1} : $1/eg;
    if (my($var,$val) = /^\s*(\w+)\s*=\s*(.*?)\s*$/) {
      die "$file: unknown setting: $var\n" unless exists $conf{$var};
      if (ref $conf{$var}) {
	push @{$conf{$var}}, $val;
      } else {
	$conf{$var} = $val;
      }
    } elsif (/\S/) {
      die "$0: unknown config directive: $_";
    }
  }
  close CONFIG;
  return %conf;
}

my $tempname = "AAA";
sub tempname {
  my $name = sprintf "/tmp/th-%05d-%1s", $$, $tempname++;
  unlink $name;
  return $name;
}

# Variables, augmented from the command-line
my %variables;
$variables{"TEMP_FILE_$_"} = tempname for 0 .. 9;

# Parse command-line switches and arguments
my($config_file,$target,$target_pass,$target_fail);
while (@ARGV) {
  local($_) = shift @ARGV;
  /^--$/ and last;
  /^(-h|-\?|--help)$/ and do { help && exit 0 };
  /^-/ and die "$0: unknown command line switch: $_\n";
  /^(\w+)=(.*)/ and do {$variables{$1} = $2; next};
  /=/ and die "$0: malformed variable definition: $_\n";
  unless ($config_file) {
    $config_file=$_;
    $variables{CONFIG}  = ($config_file =~ /([^\/]+)$/)[0];
    $variables{CONFDIR} = ($config_file =~ /(.*)\//)[0] || ".";
    next;
  }
  unless ($target) {
    $target = $variables{TARGET} = $_;
    next;
  }
  die $usage;
}
die $usage if @ARGV || !$config_file;
my %config = load_config $config_file, %variables;
if ($target =~ /(.+)\.\w+$/) {
  ($target, $target_pass, $target_fail) = ($1, $target, "$1.failed");
} else {
  ($target) = $target || ($config_file =~ /([^\/]+?)(.\w+)?$/);
  ($target_pass,$target_fail) = ("$target.passed","$target.failed");
}

# Print output to indicate test is starting. Do nothing if the test
# is disabled.
if ($config{disabled} ne 'no') {
  print "  TESTING $target (disabled: $config{disabled})\n";
  open TARGET, ">", $target_pass or die "$0: $target_pass: $!\n";
  print TARGET "test is disabled: $config{disabled}\n";
  close TARGET;
  exit 0;
}
print "  TESTING $target\n";

# Run the commands, capturing their output into files.
my($cmd_stdout,$cmd_stderr) = map {"$target.$_"} qw/out err/;
unlink $cmd_stdout, $cmd_stderr;
my($status) = 0;
for my $cmd (@{$config{cmd}}) {
  $status = system "($cmd) >>$cmd_stdout 2>>$cmd_stderr";
  last if $status;
}

# Should we compare the test's standard output with a predetermined answer?
if (!$status && $config{answer} ne 'no') {
  my($answer) = $config{answer};
  $answer = "$1.ans" if $answer eq 'yes' && $config_file =~ /(.+?)(\.[^\.]+)/;
  if (! -r $answer) {
    system "echo '$answer: no such file' >>$cmd_stderr";
    $status = 1;
  } elsif ($config{filter} ne 'no') {
    my($a,$b) = (tempname, tempname);
    $status ||= system "($config{filter}) <$answer >$a 2>>$cmd_stderr";
    $status ||= system "($config{filter}) <$cmd_stdout >$b 2>>$cmd_stderr";
    $status ||= system "($config{diff} $a $b) >>$cmd_stderr 2>&1";
    unlink $a, $b;
  } else {
    $status = system "($config{diff} $answer $cmd_stdout) >>$cmd_stderr 2>&1";
  }
}

# If the test failed and the "may_fail" property is "yes" then pretend the test was successfull.
# If the test passed and the "may_fail" property is "promote" then we need to change the property
# to "no".  The property value is either stored in a configuration file or a separate file. A separate
# file must be used when a single configuration file might be used by more than one test concurrently.
my($ignored_failure);
if ($config{may_fail} eq 'yes') {
  $ignored_failure = 1 if $status;
  $status = 0;
} elsif ($config{may_fail} eq 'promote') {
  # Rewrite the configuration file if necessary and possible
  if ($status) {
    $ignored_failure = 1 if $status;
    $status = 0;
  } elsif (0==$status && open CONFIG, "<", $config_file) {
    my($s) = join "", <CONFIG>;
    close CONFIG;
    $s =~ s/^(\s*may_fail\s*=\s*)promote(\s*(#.*))?$/$1never$2/mg;
    system "cp", $config_file, "$config_file~";
    if (open CONFIG, ">", $config_file) {
      print CONFIG $s;
      close CONFIG;
    }
  }
} elsif ($config{may_fail} ne 'no') {
  my($file, $default) = split /:/, $config{may_fail};
  $default ||= 'no';
  die "$0: $file: no such file\n" unless -e $file;
  die "$0: $file: not readable\n" unless -r $file;
  my $lock = -w $file ? "$file.lck" : "";

  # Obtain the lock if necessary.
  if ($lock) {
    my($prelock,$ntries) = ("$lock.$$", 60);
    open LOCK, ">", $prelock or die "$0: $prelock: $!\n";
    print LOCK "$$\n"; # only for debugging
    close LOCK;
    while (system "ln $prelock $lock 2>/dev/null") {
      if (--$ntries <= 0) {
	unlink $prelock;
	die "$0: cannot obtain lock: $lock\n";
      }
      usleep 500000;
    }
    unlink $prelock;
  }

  # Read the file
  my %may_fail = ($target => $default);
  open MAY_FAIL, "<", $file or do {
    unlink $lock;
    die "$0: $file: $!\n";
  };
  while (<MAY_FAIL>) {
    my($name,$val) = /^\s*(\S+)\s+(\S+)/;
    $may_fail{$name} = $val if defined $val;
  }
  close MAY_FAIL;

  # Modify the file if necesssary (keeping it sorted is optional, but nice)
  if ($may_fail{$target} eq 'yes') {
    $ignored_failure = 1 if $status;
    $status = 0;
  } elsif ($may_fail{$target} eq 'promote') {
    if ($status) {
      $ignored_failure = 1 if $status;
      $status = 0;
    } else {
      $may_fail{$target} = 'no';
      open MAY_FAIL, ">", $file or do {
	unlink $lock;
	die "$0: $file: $!\n";
      };
      print MAY_FAIL $_, " ", $may_fail{$_}, "\n" for sort keys %may_fail;
      close MAY_FAIL;
    }
  }

  # Release the lock
  unlink $lock;
}
print "$target: ignoring failure\n" if $ignored_failure;

# Produce output for failing tests, overwriting previous target. We should have only
# one target file: either $target_pass or $target_fail.
open TARGET, ">", $target_fail or die "$0: $target_fail: $!\n";
for my $output ($cmd_stdout, $cmd_stderr) {
  if (open OUTPUT, "<", $output) {
    while (<OUTPUT>) {
      print TARGET $_;
      print "$target: ", $_ if $status;
    }
    close OUTPUT;
    unlink $output;
  }
}
close TARGET;
if ($status) {
  unlink $target_pass;
} elsif ($ignored_failure) {
  system "ln", "-sf", $target_fail, $target_pass and die "$0: $target_pass: $!\n";
} else {
  rename $target_fail, $target_pass or die "$0: $target_pass: $!\n";
}

# Clean up. These names might actually be directories, otherwise we could have just unlinked.
system "rm", "-rf", $variables{"TEMP_FILE_$_"} for 0 .. 9;

exit($status ? 1 : 0);
