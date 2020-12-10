#!/usr/bin/perl
=pod

=head1 NAME

rth_run - runs a test using the ROSE test harness

=head1 SYNOPSIS

rth_run [SWITCHES...] [VAR=VALUE...] CONFIG_FILE MAKE_TARGET

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

=head1 SWITCHES

The following command-line switches are understood.  Within the ROSE project, these command-line switches are specified
with the RTH_RUN_FLAGS variable which can be set on the "make" command.  For instance, to turn on immediate printing of all
standard output and standard error invoke make as "make RTH_RUN_FLAGS=--immediate-output check".

=over

=item --cleanup

=item --no-cleanup

Enables or disables the automatic cleanup of temporary files and directories.  The default is to remove these.

=item --immediate-output

The command lines and their output are immediately echoed to this script's standard output and error as well as saving them
in the passed or failed files.

=item --quite-failure

If a test fails, rather than emitting its output, just emit a line that indicates that it failed.

=item --no-stdout

Consider any output on standard output to be an error.

=item --no-stderr

Consider any output on standard error to be an error.

=item --no-output

Consider any output to be an error. This is equivalent to saying both --no-stdout and --no-stderr.

=back

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

=item cleanup = SHELL_COMMAND

These commands all run after the "cmd" commands run, regardless of whether the "cmd" commands passed.  The exit status,
standard output, and standard error from these commands is not saved.

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
filter should read from standard input and write to standard output.  If multiple filters are specified they are run in the
order they were specified.

=item lockdir = DIRECTORY

Name of directory where lock files will be created.  The default is to use the same directory in which the may-fail file
appears. It may be necessary to set this to some other directory (like the current build directory) if the directory containing
the may-fail file is not a local file system (locking should work fine for NFS, but doesn't work for sshfs).  See the description
for the "may_fail" property.

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

=item promote = yes | no | FILE

Automatically promoting tests from "may-fail" to "must-pass" is not always desired.  For instance, if a subproject under
development has marked its tests as "promote", the developers of that subproject may want to automatically promote tests they
run, but not have tests promoted when other developers run them.  Also, a continuous integration environment testing
multiple architectures may have its own policies for deciding when a promotable test should be promoted.

This "promote" property determines when a promotable test should be promoted.  A value of "yes" (the default) indicates that a
test should be promoted to "must-pass" if the test passes, while a value of "no" indicates that promotable tests will not
promote when they pass.  Any other value is interpreted as the name of a file, which if present causes the tests to be promoted
when they pass, and if absent prevents their promotion.

Note: The file, when present, should be empty.  We reserve the possibility of adding additional instructions to the file itself
in order to control promotability in more detail.

=item require_memory = AMOUNT

Skip the test if the machine has less than AMOUNT memory installed. The AMOUNT is a number suffixed with a unit like
"16 GB". The units are "B", "kB", "MB", and "GB". If no unit, then bytes are assumed.

=item set VARIABLE = VALUE

Assign a new value to a variable.  The VARIABLE must be a variable name without enclosing curly braces or the leading dollar
sign.  White space is stripped from the beginning and end of VALUE.  Variable settings are processed in the order they occur
in the config file and are expanded in all subsequent lines. It is permissible to assign a new value to an existing variable.

=item subdir = yes | no | NAME

Run all commands in a subdirectory.  If the value is "no" or empty, then all commands run in the current working directory.
If the value is "yes" then a new temporary subdirectory is created before any commands are run, and removed during cleanup.
Any other value is the name of an existing directory which is not removed during cleanup.

=item timeout = never | LIMIT

If a test runs longer than the specified time limit then it will be aborted and assume to have failed.  The limit applies
collectively across all the individually specified commands. The LIMIT is an integer number followed by an optional unit of
measure: s (the default), sec, second, or seconds; m, min, minute, or minutes; h, hr, hour, or hours.  The default timeout is
15 minutes.

=item title = TITLE

When a test runs the test harness emits the line "TEST   xxx" where "xxx" is the makefile target sans ".passed". If the
title property has a non-empty value then the TITLE is used in place of the target name.

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

=item srcdir, top_srcdir, blddir, top_blddir, VALGRIND, etc.

These variables, and perhaps others, are passed to all invocations of the test harness when run from ROSE makefiles with the
$(RTH_RUN) variable.  The names of source and build directories are absolute even though the corresponding variable in
the makefile might be relative.  Also note that "source" and "build" are both abbreviated to three letters (makefiles
inconsistently abbreviate source, but not build).

=back

=head1 MAKEFILE ISSUES

Each test has a makefile target. If the test is named "foo" then the target is "foo.passed" and this file is created when the
test passes.  A number of configuration variables are passed to the test harness and the harness script and standard variables
together are stored in the $(RTH_RUN) makefile variable.

A simple makefile rule for a single test, depending on the configuration file stored in the source tree ("foo.conf"), and an
executable stored in the build tree ("foo"). When the base name of the target is the same as the base name of the configuration
file, the $@ argument is not necessary.

    foo.passed: foo.conf foo
            @$(RTH_RUN) $< $@

A more complex example uses a single configuration file for multiple tests and distinguishes between the tests by setting
variables on the command line.  This can only be done if the configuration file doesn't use the "may_fail=promote" property.

    EXECUTABLES=e1 e2 e3 e4 e5 e6
    TESTS=$(addsuffix .passed, $(EXECUTABLES))

    $(TESTS): %.passed: % main.conf
            @$(RTH_RUN) EXE=$< main.conf $@

=head1 TIPS AND TRICKS

If multiple tests have very similar configuration files, it's possible to write a single configuration file that's shared by
all the related tests.  Variations in the configuration can be passed as variables set on the command line. This can only be
done safely if the configuration file does not use the "may_fail=promote" property.

When sharing configuration files among many tests, one can still use the automatic promoting from "may fail" to "must pass",
but the information about what files pass and fail should be stored outside the configuration file.  This is possible by
supplying a file name for the "may_fail" property. See above.

When using the "may_fail=promote" property, one can determine which tests have been promoted from "may fail" to "must pass"
status by asking git about which configuration files have changed.

Sometimes the category of test ("may-fail" or "must-pass") depends on project configuration information, such as which version
of a library is used.  There are two ways to handle this: place the test harness configuration files in a directory whose name
depends on the project configuration information, or have one set of test harness configuration files whose may_fail properties
point to files whose names depend on the configuration information.  The latter approach reduces the number of cut-n-pasted
test harness configuration files.

Normally, the test harness will abort the test if one of the commands in the configuration file exits with a non-zero
status.  To avoid this, append "; true" to the end of the command.  It's also possible to invert the sense of the failure
by appending "; test $? -ne 0" to the command.

To re-run all tests, remove the *.passed files and run "make check".

To re-run tests that failed but whose failure was ignored (i.e., the "may-fail" or "promote" tests), remove all the *.failed
files and run "make check".  Removing these *.failed files results in the corresponding *.passed files being dangling symbolic
links, which "make" will treat as being out of date.

=head1 BUGS

No attempt is made to discover whether multiple concurrent tests try to update a single config file at the same time (when a
test is promoted from the "may-fail" to "must-pass" state).  When concurrent tests share a single config file and use this
auto-promotion feature, they must specify a separate file(s) to contain the may-fail state of the targets.  See the "may_fail"
property above.

File locking depends on being able to create multiple hard links to a file. If this is not possible then the locking mechanism
might fail to behave properly and the may_fail file could become corrupted.

Output from the test is organized in such a way that the standard output all appears before the standard error. It would be
more intuitive if the file contained output in more or less the order it was generated.

Output is left in a *.passed or *.failed file. This makes it difficult to write makefile rules for tests that don't depend on
each other's success or failure, but must depend on each other in order to achieve serial execution in an otherwise parallel
make.

=head1 AUTHOR

Robb Matzke.

Copyright Lawrence Livermore National Security

Licensed under Revised BSD License (see COPYRIGHT file at top of ROSE source code)

=cut

use strict;
use Config;

sub usage {
    print STDERR "usage: $0 [VAR=VALUE...] CONFIG TARGET\n";
    print STDERR "       $0 --help\n";
    print STDERR "was invoked with ", scalar(@ARGV), " argument", (1==@ARGV?"":"s"), (0==@ARGV?"":":"), "\n";
    print STDERR "   '$_'\n" for @ARGV;
    exit 1
}

sub help {
  local $_ = `(pod2man $0 |nroff -man) 2>/dev/null` ||
             `pod2text $0 2>/dev/null` ||
             `sed -ne '/^=pod/,/^=cut/p' $0 2>/dev/null`;
  die "$0: see source file for documentation" unless $_;
  if (open LESS, "|less -R") {
    print LESS $_;
    close LESS;
  } else {
    print $_;
  }
};

# Parse specified configuration file or die trying. Return value is a hash whose keys are the variable names and whose
# values are the values or an array of values.
sub load_config {
  my($file,$vars) = @_;
  my(%conf) = (answer=>'no', cmd=>[], cleanup=>[], diff=>'diff -u', disabled=>'no', filter=>[], lockdir=>undef,
               may_fail=>'no', promote=>'yes', require_memory=>undef, subdir=>undef, timeout=>15*60, title=>undef);
  open CONFIG, "<", $file or die "$0: $file: $!\n";
  while (<CONFIG>) {
    while (/(.*)\\\n$/s) {
      my $extra = <CONFIG>;
      last unless defined $extra;
      $_ = $1 . $extra;
    }
    s/\s*#.*//;
    s/\$\{(\w+)\}/exists $vars->{$1} ? $vars->{$1} : ''/eg;
    if (my($var,$val) = /^\s*set\s+(\w+)\s*=\s*(.*?)\s*$/) {
	$vars->{$var} = $val;
    } elsif (my($var,$val) = /^\s*(\w+)\s*=\s*(.*?)\s*$/) {
      die "$file: unknown setting: $var\n" unless exists $conf{$var};
      if (ref $conf{$var}) {
        push @{$conf{$var}}, $val;
      } else {
        $conf{$var} = $val;
      }
    } elsif (/\S/) {
      die "$file: unknown config directive: $_";
    }
  }
  close CONFIG;

  # Convert the timeout value to seconds (zero implies infinity)
  if ($conf{timeout} eq "") {
    $conf{timeout} = 15*60; # use the default
  } elsif ($conf{timeout} !~ /^\d+$/) {
    if ($conf{timeout} =~ /^(\d+)\s*(s|sec|seconds?|m|min|minutes?|hr?|hours?)$/) {
      $conf{timeout} = $1 * {s=>1, m=>60, h=>3600}->{substr $2,0,1};
    } elsif ($conf{timeout} eq 'never') {
      $conf{timeout} = 0;
    } else {
      die "$0: invalid timeout specification: $conf{timeout}\n";
    }
  }

  # Convert memory size to MB
  if (my($n,$units) = $conf{require_memory} =~ /^(\d+)\s*(\S+)?\s*$/) {
      $conf{require_memory} = megabytes($n, $units);
  } elsif ($conf{require_memory}) {
      die "$0: invalid memory limit: $conf{require_memory}\n";
  }

  return %conf;
}

# Produce a temporary name for files, directories, etc. without creating a file. We need a name that is unlikely to
# be in use or left over from a previous run that didn't clean up, so use Perl's rand() to create one. This not
# cryptographically strong, but at least we're unlikely to have conflicts.
sub tempname {
    my @chars = (qw/a b c d e f g h i j k l m n o p q r s t u v w x y z
                    A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
                    0 1 2 3 4 5 6 7 8 9 _ +/);
    my $prefix = ($ENV{TMPDIR} && -d $ENV{TMPDIR} && -w _ ? $ENV{TMPDIR} : "/tmp") . "/rose";
    while (1) {
	my $retval = $prefix;
	$retval .= $chars[int rand 64] for 0 .. 15;
	return $retval unless -e $retval;
    }
}

# Runs the specified commands one by one until they've all been run, one exits with non-zero status, or a timeout
# occurs. The command's standard output and error are redirected to the already-opened CMD_STDOUT and CMD_STDERR
# streams. This function returns the exit status and the number of bytes written by the command(s) to standard error. We
# have to do this the hard way in order to send SIGKILL to the child after a timeout (we don't want a testing host
# filling up with timed-out tests that continue to suck up resources).
sub run_command {
  my($timelimit,$subdir,@commands) = @_;
  defined (my $pid = fork) or return (255, 0);
  my($stats_file) = tempname();

  if (!$pid) {
    open STDOUT, ">&CMD_STDOUT" or exit(255);
    open STDERR, ">&CMD_STDERR" or exit(254);
    setpgrp; # so we can kill the whole group on a timeout
    my $status = 0;
    my $nerror_bytes = 0;

    for my $cmd (@commands) {
      # Open a temporary file for only this command's standard error, separate from the standard error for this
      # whole testing output because we need to be able to count how many bytes were output to standard error by just
      # the command. We'll later copy the command's standard error output into the final error output file.
      my($tmp) = tempname();
      open SINGLE_CMD_ERRORS, ">", $tmp or exit(253);
      open STDERR, ">&SINGLE_CMD_ERRORS" or exit(254);

      $cmd = "cd '$subdir'; $cmd" if $subdir;
      print CMD_STDERR "+ $cmd\n";
      $status = system $cmd;

      # Count amount of standard error from this single command and copy it to the combined error output file.
      open STDERR, ">&CMD_STDERR";
      close SINGLE_CMD_ERRORS;
      open SINGLE_CMD_ERRORS, "<", $tmp;
      while (<SINGLE_CMD_ERRORS>) {
	$nerror_bytes += length($_);
	print STDERR $_;
      }
      close SINGLE_CMD_ERRORS;
      unlink $tmp;

      # The shell usually prints messages about signals, so we'll take over that job since there's no shell.  In fact, we'll
      # also print the exit status if it's other than zero.
      if (-1 == $status) {
        print STDERR "command failed to run\n";
        last;
      } elsif ($status & 127) {
        my $signum = $status & 127;
        print STDERR ("command died with SIG", (split ' ', $Config{sig_name})[$signum],
                      ($status & 128 ? " (coredumped)" : ""), "\n");
        last;
      } elsif ($status) {
        print STDERR "command exited with value ", ($status>>8), "\n";
        last;
      }
    }

    # This is a child process. Any values that we want to return need to be stored in the filesystem and read by the parent.
    open STATS_FILE, ">", "$stats_file";
    print STATS_FILE $nerror_bytes;
    close STATS_FILE;
    exit($status!=0 ? 1 : 0);
  }

  my $status = -1;
  eval {
    local $SIG{ALRM} = sub {die "alarm\n"};
    alarm($timelimit);
    $status = $? if $pid == waitpid $pid, 0;
    alarm(0);
  };
  if ("$@" eq "alarm\n") {
    kill -1, $pid; sleep 5; # give the test a chance to clean up gracefully
    kill -9, $pid; # kill the whole process group in case the above exec used the shell.
    $status = $? if $pid == waitpid $pid, 0;
    print CMD_STDERR "\nterminated after $timelimit seconds\n";
  }

  # Read variables set by the child process
  my($nerror_bytes) = `cat $stats_file`;
  return ($status, $nerror_bytes);
}

# Returns true if a test should be promoted.  The argument is the value of the "promote" property.
sub should_promote {
  my($promote) = @_;
  return 0 if $promote eq 'no';
  return 1 if $promote eq 'yes';
  return 1 if -f $promote;
  return 0;
}

# Convert to MB according to unit.
sub megabytes {
    my($n, $units) = @_;
    $units eq "" || $units eq 'b' || $units eq 'B' and return $n / (1024.0 * 1024);
    $units =~ /^(k|kb|kB|K|KB)$/ and return $n / 1024.0;
    $units =~ /^(m|mb|M|MB)$/ and return $n;
    $units =~ /^(g|bg|G|GB)$/ and return $n * 1024;
    die "unknown memory unit: $units"
}

# Amount of installed memory in megabytes (2^20 bytes)
sub installed_memory {
    my $mb;
    local($_);
    open MEMINFO, "<", "/proc/meminfo" or die "$!: /proc/meminfo";
    while (<MEMINFO>) {
	if (my($amount,$units) = /^MemTotal:\s*(\d+)\s*(\S+)/) {
	    $mb = megabytes $amount, $units;
	    last;
	}
    }
    close MEMINFO;
    die "cannot determine amount of installed memory" unless $mb != 0;
    return $mb;
}

# Variables, augmented from the command-line
my %variables;
$variables{"TEMP_FILE_$_"} = tempname for 0 .. 9;

# Parse command-line switches and arguments
my($do_cleanup,$quiet_failure,$immediate_output,$config_file,$target,$target_pass,$target_fail,$no_stdout,$no_stderr) = (1);
while (@ARGV) {
  local($_) = shift @ARGV;
  /^--$/ and last;
  /^(-h|-\?|--help)$/ and do { help && exit 0 };
  /^--no-cleanup$/ and do {$do_cleanup=0; next};
  /^--cleanup$/ and do {$do_cleanup=1; next};
  /^--immediate-output$/ and do {$immediate_output=1; next};
  /^--no-stdout$/ and do {$no_stdout=1; next};
  /^--no-stderr$/ and do {$no_stderr=1; next};
  /^--no-output$/ and do {$no_stderr=$no_stdout=1; next};
  /^--quiet-failure$/ and do {$quiet_failure=1; next};
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
    $target = $_;
    next;
  }
  usage;
}
usage if @ARGV || !$config_file;
if ($target =~ /(.+)\.\w+$/) {
  ($target, $target_pass, $target_fail) = ($1, $target, "$1.failed");
} else {
  ($target) = $target || ($config_file =~ /([^\/]+?)(.\w+)?$/);
  ($target_pass,$target_fail) = ("$target.passed","$target.failed");
}
$variables{TARGET} = $target;
my %config = load_config $config_file, \%variables;

# Print output to indicate test is starting. Do nothing if the test
# is disabled.
my $test_title = $config{title} || $target;
if ($config{disabled} && $config{disabled} ne 'no') {
  print "  TEST     $test_title \033[31;1m(disabled: $config{disabled})\033[0m\n";
  open TARGET, ">", $target_pass or die "$0: $target_pass: $!\n";
  print TARGET "test is disabled: $config{disabled}\n";
  close TARGET;
  exit 0;
} elsif ($config{require_memory} && $config{require_memory} > installed_memory) {
  print "  TEST     $test_title \033[31;1m(disabled: this machine does not have enough memory)\033[0m\n";
  open TARGET, ">", $target_pass or die "$0: $target_pass: $!\n";
  print TARGET "test is skipped: not enough memory installed\n";
  close TARGET;
  exit 0;
}
print "  TEST     $test_title\n";

# Should everything run in a subdirectory?
my $subdir;
if ($config{subdir} eq 'yes') {
    # run in a temporary subdirectory to be created now and removed later
    $subdir = $variables{SUBDIR} = tempname;
    my $status = system "mkdir", $subdir;
    die "$0: $subdir: $!\n" if $status;
} elsif ($config{subdir} eq 'no' || $config{subdir} eq '') {
    # run in the current working directory
} else {
    # run in a user-specified subdir which we neither create nor remove
    $subdir = $variables{SUBDIR} = $config{subdir}
}

# Run the commands, capturing their output into files.
my($cmd_stdout_file,$cmd_stderr_file) = map {tempname} (qw/out err/);
unlink $cmd_stdout_file, $cmd_stderr_file;
if (!$immediate_output) {
    open CMD_STDOUT, ">", $cmd_stdout_file or die "$cmd_stdout_file: $!\n";
    open CMD_STDERR, ">", $cmd_stderr_file or die "$cmd_stderr_file: $!\n";
} else {
    open CMD_STDOUT, "|tee $cmd_stdout_file |sed 's/^/$target [out]: /'" or die "tee $cmd_stdout_file: $!\n";
    open CMD_STDERR, "|tee $cmd_stderr_file |sed 's/^/$target [err]: /' >&2" or die "tee $cmd_stderr_file: $!\n";
}
my($starttime) = time;
my($status,$nerror_bytes) = run_command($config{timeout}, $subdir, @{$config{cmd}});
open CMD_STDOUT, ">&CMD_STDERR";
if (!$status && $no_stdout && -s $cmd_stdout_file) {
    print CMD_STDERR "\nERROR: command(s) produced unexpected output on stdout\n";
    $status = 1;
}
if (!$status && $no_stderr && $nerror_bytes > 0) {
    print CMD_STDERR "\nERROR: command(s) produced unexpected output on stderr\n";
    $status = 1;
}
my($elapsed_time) = time - $starttime;
print CMD_STDERR "ELAPSED_TIME $elapsed_time\n";

# Should we compare the test's standard output with a predetermined answer?
if (!$status && $config{answer} ne 'no') {
  my($answer) = $config{answer};
  if ($answer eq 'yes') {
    ($answer = $config_file) =~ s/(\.[^\.\/]+)$//;
    $answer .= ".ans";
  }
  if (! -r $answer) {
    print CMD_STDERR "$answer: no such file\n";
    $status = 1;
  } elsif (0 != @{$config{filter}} ) {
    print CMD_STDERR "Running filters:\n";
    my($a1,$a2,$b1,$b2) = ($answer, tempname, $cmd_stdout_file, tempname);
    foreach my $filter (@{$config{filter}}) {
	$status ||= (run_command($config{timeout}, undef, "(set -x; $filter) <$a1 >$a2"))[0];
	$status ||= (run_command($config{timeout}, undef, "(set -x; $filter) <$b1 >$b2"))[0];
	$a1 = tempname if $a1 eq $answer;
	$b1 = tempname if $b1 eq $cmd_stdout_file;
	($a1,$a2) = ($a2,$a1);
	($b1,$b2) = ($b2,$b1);
	last if $status;
    }
    if (!$status) {
	print CMD_STDERR "Comparing filter output with filtered answer\n";
	$status ||= (run_command($config{timeout}, undef, "(set -x; $config{diff} $a1 $b1) >&2"))[0];
    }
    unlink $a1, $a2, $b1, $b2;
  } else {
    print CMD_STDERR "Comparing output with answer\n";
    $status ||= (run_command($config{timeout}, undef, "(set -x; $config{diff} $answer $cmd_stdout_file) >&2"))[0];
  }
}

# Run clean-up commands (their stdout is automatically redirected to stderr by now)
run_command($config{timeout}, undef, @{$config{cleanup}});

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
  } elsif (0==$status && should_promote($config{promote}) && open CONFIG, "<", $config_file) {
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
  my($lock) = $config{lockdir} eq "" ? "$file.lck" : $config{lockdir} . "/" . ($file =~ /([^\/]+)$/)[0] . ".lck";
  $lock = "" unless -w $file;

  # Obtain the lock if necessary.  If we can't write to the file then assume that nobody else is changing it either,
  # in which case we don't need exclusive access to read it.
  if ($lock) {
    my($prelock,$ntries) = ("$lock.$$", 120);
    open LOCK, ">", $prelock or die "$0: $prelock: $!\n";
    print LOCK "$$\n"; # only for debugging
    close LOCK;
    while (system "ln $prelock $lock 2>/dev/null") {
      if (--$ntries <= 0) {
        unlink $prelock;
        die "$0: cannot obtain lock: $lock\n";
      }
      select(undef, undef, undef, 0.25); # NMI machines don't have Time::HiRes
    }
    unlink $prelock;
  }

  # Read the file or die trying.
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

  # Modify the file if necessary (keeping it sorted is optional, but nice)
  if ($may_fail{$target} eq 'yes') {
    $ignored_failure = 1 if $status;
    $status = 0;
  } elsif ($may_fail{$target} eq 'promote') {
    if ($status) {
      $ignored_failure = 1 if $status;
      $status = 0;
    } elsif (!should_promote($config{promote})) {
      # Not promoted
    } elsif ($lock) {
      $may_fail{$target} = 'no';
      open MAY_FAIL, ">", $file or do {
        unlink $lock;
        die "$0: $file: $!\n";
      };
      printf MAY_FAIL "%-40s %s\n", $_, $may_fail{$_} for sort keys %may_fail;
      close MAY_FAIL;
    } else {
      # Test may-fail value is "promote" and the test passed. We'd like to promote the test to "must-pass", but we can't
      # write to the file containing the may-fail properties ($file).  Therefore, don't promote, just complain.
      print STDERR "$target: $file: read-only file, not promoting test to must-pass status.\n";
    }
  }

  # Release the lock
  unlink $lock if $lock;
}
print "$target: ignoring failure\n" if $ignored_failure;

# Create the *.failed file and populate it with the commands' stdout and stderr
close CMD_STDOUT;
close CMD_STDERR;
open TARGET, ">", $target_fail or die "$0: $target_fail: $!\n";
my %output_filename = (out => $cmd_stdout_file, err => $cmd_stderr_file);
for my $stream (qw(out err)) {
  if (open OUTPUT, "<", $output_filename{$stream}) {
    while (<OUTPUT>) {
      print TARGET $_;
      if ($status && !$immediate_output && !$quiet_failure) {
	  print "$target \[$stream]: ", $_;
      }
    }
    close OUTPUT;
    print TARGET "======== CUT ========\n";
  }
}

# Clean up. These names might actually be directories, otherwise we could have just unlinked.  If cleanup is disabled then
# append the file names to the end of the TARGET file.
if ($do_cleanup) {
    unlink $cmd_stdout_file;
    unlink $cmd_stderr_file;
    system "rm", "-rf", $subdir if $subdir && $subdir ne $config{subdir};
    system "rm", "-rf", $variables{"TEMP_FILE_$_"} for 0 .. 9;
} else {
    print TARGET "Temporary files not deleted:\n";
    print TARGET "  Working subdirectory: $subdir\n" if $subdir;
    print TARGET "  Command standard output is in $cmd_stdout_file\n";
    print TARGET "  Command standard error is in $cmd_stderr_file\n";
    for (0..9) {
	my $filename = $variables{"TEMP_FILE_$_"};
	print TARGET "  TEMP_FILE_$_: $filename\n" if -e $filename;
    }
}

# Move or link the *.failed file to *.passed as appropriate.
close TARGET;
if ($status) {
  unlink $target_pass;
  print STDERR "  FAILED $test_title; output saved in '$target_fail'\n" if $quiet_failure;
} elsif ($ignored_failure) {
  system "ln", "-sf", $target_fail, $target_pass and die "$0: $target_pass: $!\n";
} else {
  rename $target_fail, $target_pass or die "$0: $target_pass: $!\n";
}

exit($status ? 1 : 0);
