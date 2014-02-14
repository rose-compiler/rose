#!/usr/bin/perl
use strict;

###############################################################################################################################
###############################################################################################################################
# User-configurable stuff...
###############################################################################################################################
###############################################################################################################################

my $dry_run = 1;      # Set true if you only want to see what would have been done.
my $dropdb = 1;       # Set true to try to drop each database before the test runs (tests are skipped if a database exists).
my $max_pairs = 5;    # Maximum number of pairs to run, selected at random.
my $per_program = 0;  # If true, select $max_pairs on a per program basis rather than over all.
my $same_program = 1; # If true, then pairs of functions must be the same program (e.g., both "egrep")
my $symmetric = 1;    # If true, avoid generating pair (a, b) if pair (b, a) was selected.
my $dbprefix = "as_"; # Prefix to add to each database name

# Location of the training files. These must follow a naming convention described in the load_specimens function.
my $training_dir = "$ENV{HOME}/GS-CAD/ROSE/CloneDetection/training-set-targets";

# Predicate that defines how to generate pairs.  A pair is created if both $a and $b have the same program name and
# this predicate returns true.  The predicate is called with two specimen arguments. Each specimen is a hash reference
# that has the following members:
#   filename => name of the file in the file system
#   package  => name of the package, as in "grep-2.6.3"
#   program  => name of the program, as in "egrep"
#   compiler => as in "gcc", "icc", "llvm"
#   optim    => optimization level, as in "s", "0", "3", etc.
#
# See the examples below that return predicates.

# Configuration for the run-analysis.sh script
my $configuration = <<'EOF';
recreate=yes
generate_inputs_flags='--ngroups=1 --memhash arguments:redirect=memhash locals:random=1000 locals:pad=inf,0 globals:random=1000 globals:pad=inf,0 functions:random=1000 functions:pad=inf,0 integers:redirect=memhash'
add_functions_flags='--signature-components=total_for_variant,by_category,apply_log'
get_pending_tests_flags='--size=20'
run_tests_nprocs=''
run_tests_job_multiplier='1'
run_tests_flags='--follow-calls=builtin --timeout=1000000 --coverage=save --call-graph=save --path-syntactic=function --signature-components=total_for_variant,by_category,apply_log'
func_similarity_worklist_flags=''
func_similarity_flags=''
EOF

###############################################################################################################################
###############################################################################################################################
# Supporting functions
###############################################################################################################################
###############################################################################################################################

# Load all specimens that can be found in the specified directory
sub load_specimens {
    my($dir) = @_;
    my @specimens;
    my %compilers = (g=>"gcc", i=>"icc", l=>"llvm");
    open FIND, "-|", "find $dir -type f" or die "find failed";
    while (<FIND>) {
	# Names look like: .../coreutils-8.5/id/g1/id
	chomp;
	my @parts = reverse split "/";
	next unless @parts >= 4 && $parts[0] eq $parts[2];
	@parts = splice @parts, 1, 3; # g1 id coreutils-8.5
	splice @parts, 0, 1, split "", $parts[0], 2;   # g 1 id coreutils-8.5
	$parts[0] = $compilers{$parts[0]} if exists $compilers{$parts[0]}; # use full compiler name
	push @specimens, { filename=>$_, package=>$parts[3], program=>$parts[2], compiler=>$parts[0], optim=>$parts[1] };
    }
    close FIND;
    return @specimens;
}

# Show what specimens we have
sub print_specimens {
    my @specimens = @_;
    for my $specimen (@specimens) {
	printf("    %-20s %-16s %4s -O%s %s\n",
	       $specimen->{package}, $specimen->{program}, $specimen->{compiler}, $specimen->{optim}, $specimen->{filename});
    }
}

# Select all pairs of functions for which:
#  + the predicate returns true
#  + and for which the program names are the same (if $same_program is true)
# Optionally omitting pairs that are symmetric duplicates
sub select_pairs {
    my($specimens, $predicate) = @_;
    my(@pairs,%selected);
    print "generating function pairs...\n";
    for my $specimen1 (@$specimens) {
	for my $specimen2 (@$specimens) {
	    if ((!$same_program || ($specimen1->{program} eq $specimen2->{program})) &&
		&{$predicate}($specimen1, $specimen2)) {
		# Do not use this pair if we already selected its symmetric counterpart
		if ($symmetric) {
		    my $ab = "$specimen1->{filename}:$specimen2->{filename}";
		    my $ba = "$specimen2->{filename}:$specimen1->{filename}";
		    next if $selected{$ba};
		    $selected{$ab} = 1;
		}
		push @pairs, [$specimen1, $specimen2];
	    }
	}
    }
    print "generated ", 0+@pairs, " function pairs", ($symmetric?" pruned by symmetry":""), "\n";
    return @pairs;
}

# Select up to N values uniformly random. None of the returned values are duplicates (unless they were duplicated in
# the original list).
sub select_random {
    my($n, @array) = @_;
    my @retval;
    while ($n-- > 0 && @array > 0) {
	my $idx = int rand(scalar @array);
	push @retval, splice @array, $idx, 1;
    }
    return @retval;
}

# Select up to N values uniformly for each program name.  For any given pair of specimens (a,b), we use a_program as the
# program name (a_program=b_program anyway when $same_program is true).
sub select_random_per_program {
    my($n, @pairs) = @_;

    # Partition list of all pairs into lists according to $a->{program}
    my %perprog;
    for my $pair (@pairs) {
	my $a = $pair->[0];
	$perprog{$a->{program}} ||= [];
	push @{$perprog{$a->{program}}}, $pair;
    }
   
    # Select N randomly from each list
    my @retval;
    push @retval, select_random $n, @$_ for values %perprog;
    return @retval;
}

# Generate a database name for a pair of specimens.
sub database_name {
    my($a, $b) = @_;
    $dbprefix . join "_", $a->{program}, $a->{compiler}, $a->{optim}, $b->{program}, $b->{compiler}, $b->{optim};
}

# Run something, but also echo the command. Do nothing if $dry_run is set. Return true on success, false on failure
sub run {
    my(@cmd) = @_;
    print "+ ", join(" ", @cmd), "\n";
    0==system @cmd unless $dry_run;
}

my @specimens = load_specimens $training_dir;
print "loaded information for ", 0+@specimens, " specimens.\n";



###############################################################################################################################
###############################################################################################################################
##  These examples demonstrate the queries from Andreas' email on 2/6/14 and later.  Each function returns a predicate that
##  can then be passed to the select_pairs function.
###############################################################################################################################
###############################################################################################################################

sub example1 {
    print "\nexample 1: all (a,b) s.t.\n";
    print "               a_program = b_program\n";
    return sub {1}
}

sub example2 {
    print "\nexample 2: all (a,b) s.t.\n";
    print "               a_program = b_program and\n";
    print "               a_compiler in {gcc, icc} and\n";
    print "               a_optim in {s, 0} and\n";
    print "               b_compiler in {gcc, icc} and\n";
    print "               b_optim in {s, 0}\n";
    return sub {
	my($a, $b) = @_;
	my %want_C = (gcc=>1, icc=>1);
	my %want_X = ('s'=>'1', '0'=>1);
	$want_C{$a->{compiler}} && $want_C{$b->{compiler}} && $want_X{$a->{optim}} && $want_X{$b->{optim}};
    }
}

sub example3 {
    print "\nexample 3: all (a,b) s.t.\n";
    print "               a_program = b_program and\n";
    print "               a_optim in {s, 0} and\n";
    print "               b_optim in {s, 0}\n";
    return sub {
	my($a, $b) = @_;
	my %want_X = ('s'=>1, '0'=>1);
	$want_X{$a->{optim}} && $want_X{$b->{optim}}
    }
}

sub example4 {
    print "\nexample 4: all (a,b) s.t.\n";
    print "               a_program = b_program and\n";
    print "               a_compiler != b_compiler\n";
    return sub {
	my($a, $b) = @_;
	$a->{compiler} ne $b->{compiler};
    }
}

sub example5 {
    print "\nexample 5: all (a, b) s.t.\n";
    print "               a_program = b_program and\n";
    print "               a_compiler in {gcc, icc} and\n";
    print "               a_optim in {s, 0}\n";
    return sub {
	my($a, $b) = @_;
	my %want_C = (gcc=>1, icc=>1);
	my %want_X = ('s'=>1, '0'=>1);
	$want_C{$a->{compiler}} && $want_X{$a->{optim}};
    }
}

sub example6 {
    print "\nexample 6: select (a, b) s.t.\n";
    print "               a_program = b_program and\n";
    print "               (a_compiler, a_optim) in CompilerOptimSet and\n";
    print "               (b_compiler, b_optim) in CompilerOptimSet\n";
    print "               where CompilerOptimSet is a set of ordered pairs\n";
    print "               where each pair is a compiler and optimization level\n";
    my $npairs = 3;		# number of (compiler,otpim) pairs to randomly select
    my %distinct;		# keys are the distinct (compiler,optim) pairs
    $distinct{"$_->{compiler} $_->{optim}"} = 1 for @specimens;
    my %coset = map {$_=>1} select_random $npairs, keys %distinct;
    print "selecting these combinations of compiler and optimization level:\n";
    print "   $_\n" for sort keys %coset;
    return sub {
	my($a, $b) = @_;
	$coset{"$a->{compiler} $a->{optim}"} && $coset{"$b->{compiler} $b->{optim}"};
    }
}

###############################################################################################################################
###############################################################################################################################
# The main program
###############################################################################################################################
###############################################################################################################################

# Generate a list of pairs over which to run
my @pairs = select_pairs \@specimens, example6;
if (defined $max_pairs) {
    if ($per_program) {
	@pairs = select_random_per_program $max_pairs, @pairs;
	print "selected up to $max_pairs pairs for each program; ", 0+@pairs, " pairs in total\n";
    } else {
	@pairs = select_random $max_pairs, @pairs;
	print "selected ", 0+@pairs, " pairs at random\n";
    }
}

# Run the analysis for each pair
for my $pair (@pairs) {
    my $a = $pair->[0];
    my $b = $pair->[1];

    print "=" x 80, "\nrunning this pair:\n";
    print_specimens $pair->[0], $pair->[1];

    # If the database exists either skip the test or delete the database first
    my $dbname = database_name($a, $b);
    run "dropdb", $dbname if $dropdb;
    run "createdb", $dbname or next;

    # File names for the specimens.  If both specimens have the same file name then we need to copy one of them to
    # a new name otherwise the system gets confused (because it identifies specimens by their file names).  We cannot
    # remove this name because the database will be referencing it.
    my($a_file, $b_file) = ($a->{filename}, $b->{filename});
    run "ln", $b_file, ($a_file.=".2") if $a_file eq $b_file;

    # Run the analysis
    my $config_file = `tempfile`; chomp $config_file;
    open CONFIG, ">", $config_file or die "$config_file: $!\n";
    print CONFIG "dbname='postgresql:///$dbname\n", $configuration;
    close CONFIG;
    print STDERR "# Configuration file ($config_file):\n", `sed 's/^/    /' $config_file`;
    my($mydir) = $0 =~ /(.*)\//; $mydir ||= ".";
    run "$mydir/run-analysis.sh", "--batch", "--config=$config_file", $a_file, $b_file;
    unlink $config_file;
}
