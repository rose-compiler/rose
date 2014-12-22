#!/usr/bin/perl
use strict;

###############################################################################################################################
###############################################################################################################################
# User-configurable stuff...
###############################################################################################################################
###############################################################################################################################

my $dry_run = 0;        # Set true if you only want to see what would have been done.
my $dropdb = 1;         # Set true to try to drop each database before the test runs (tests are skipped if a database exists).
<<<<<<< HEAD
my $max_pairs = 20;     # Maximum number of specimen pairs pairs to run, selected at random.
my $per_program = 1;    # If true, select $max_pairs on a per program basis rather than over all.
my $same_program = 1;   # If true, then pairs of specimens must be the same program (e.g., both "egrep")
my $symmetric = 1;      # If true, avoid generating pair (a, b) if pair (b, a) was selected.
my $dbprefix = "example10_";   # Prefix to add to each database name
=======
my $max_pairs = undef;  # Maximum number of specimen pairs pairs to run, selected at random (or all of them if undefined)
my $per_program = 0;    # If true, select $max_pairs on a per program basis rather than over all.
my $same_program = 1;   # If true, then pairs of specimens must be the same program (e.g., both "egrep")
my $symmetric = 1;      # If true, avoid generating pair (a, b) if pair (b, a) was selected.
my $dbprefix = "as";    # Prefix to add to each database name
>>>>>>> b2197eaa90e31bc057b41c066b118716b0b65d87

# Location of the training files. These must follow a naming convention described in the load_specimens function.
my $training_dir = "$ENV{HOME}/binary-runs/suspects-and-victims-subset/test-set/";

# Predicate that defines how to generate pairs.  A pair is created if both $a and $b have the same program name and
# this predicate returns true.  The predicate is called with two specimen arguments. Each specimen is a hash reference
# that has the following members:
#   filename => name of the file in the file system
#   package  => name of the package, as in "grep-2.6.3"
#   program  => name of the program, as in "egrep"
#   compiler => as in "gcc", "icc", "llvm", "stunnix"
#   comp     => as in "g", "i", "l", "s";
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

my $api_configuration = <<'EOF';
api_similarity_worklist_flags=''
api_similarity_flags=''
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
    my %compilers = (g=>"gcc", i=>"icc", l=>"llvm", "s"=>"stunnix");
    open FIND, "-|", "find $dir -type f" or die "find failed";
    while (<FIND>) {
        # Names look like: .../coreutils-8.5/id/g1/id
        chomp;
        my @parts = reverse split "/";
        next unless @parts >= 4 && $parts[0] eq $parts[2];
        @parts = splice @parts, 1, 3; # g1 id coreutils-8.5
        splice @parts, 0, 1, split "", $parts[0], 2;   # g 1 id coreutils-8.5
	push @parts, $compilers{$parts[0]} || $parts[0]; # g 1 id coreutils-8.5 gcc
        push @specimens, { filename=>$_, package=>$parts[3], program=>$parts[2], comp=>$parts[0], optim=>$parts[1],
			   compiler=>$parts[4] };
    }
    close FIND;
    return @specimens;
}

# Show what specimens we have
sub print_specimens {
    my @specimens = @_;
    for my $specimen (@specimens) {
        printf("    %-20s %-16s %7s -O%s %s\n",
               $specimen->{package}, $specimen->{program}, $specimen->{compiler}, $specimen->{optim}, $specimen->{filename});
    }
}

# Select all pairs of specimens for which:
#  + the predicate returns true
#  + and for which the program names are the same (if $same_program is true)
# Optionally omitting pairs that are symmetric duplicates
sub select_pairs {
    my($specimens, $predicate) = @_;
    my(@pairs,%selected);
    print "generating specimen pairs...\n";
    for my $specimen1 (@$specimens) {
        for my $specimen2 (@$specimens) {
            if ((!$same_program ||
		 ($specimen1->{package} eq $specimen2->{package} && ($specimen1->{program} eq $specimen2->{program}))) &&
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
    print "required a_program = b_program\n" if $same_program;
    print "eliminated symmetric specimen pair duplicates: (a,b) if (b,a)\n" if $symmetric;
    print "generated ", 0+@pairs, " specimen pairs\n";
    return @pairs;
}

# Select pairs of specimens (a, b) such that the ordered tuple created from certain properties of a and b exists
# in the set @tuples. For example, if $keys = ['compiler', 'optim'] then the @tuples array should contain
# array refs like ["gcc", "s", "icc", "0"], which selects pairs such that a_compiler='gcc' and a_optim='s'
# and b_compiler='icc' and b_optim='0'.
sub select_tuples {
    my($specimens, $keys, @tuples) = @_;
    print "Selected ", 0+@tuples, " criteria for (", join(", ", @$keys), "):\n";
    print "    (", join(", ", @$_), ")\n" for sort @tuples;
    my %tuples;
    $tuples{join(" ",@$_)} = 1 for @tuples;  # so we can do O(log N) lookups
    return select_pairs $specimens, sub {
        my($a, $b) = @_;
        my $a_key = join " ", map {$a->{$_}} @$keys;
        my $b_key = join " ", map {$b->{$_}} @$keys;
        return $tuples{"$a_key $b_key"};
    }
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

# Select random pairs of specimens.  Selects either $n pairs at random overall, or partitions the set by program name
# and selects $n for each program name.
sub select_random_pairs {
    my($n, @pairs) = @_;
    if (defined $n) {
        if ($per_program) {
            @pairs = select_random_per_program $n, @pairs;
            print "selected up to $n specimen pairs for each program; ", 0+@pairs, " pairs in total\n";
        } else {
            @pairs = select_random $n, @pairs;
            print "selected ", 0+@pairs, " specimen pairs at random\n";
        }
    }
    return @pairs;
}

# Generate a database name for a pair of specimens.
sub database_name {
    my($a, $b) = @_;
<<<<<<< HEAD
    if($same_program){
      $dbprefix . join "_", $a->{program}, substr($a->{compiler},0,1) . $a->{optim}, substr($b->{compiler},0,1) . $b->{optim};
    }else{
      $dbprefix . join "_", $a->{program}, $a->{compiler}, $a->{optim}, $b->{program}, $b->{compiler}, $b->{optim};
    }
=======
    my @name = $dbprefix;
    my $normalize = sub {
	local($_) = @_;
	tr/a-zA-Z0-9//cd;
	return $_;
    };
    push @name, &{$normalize}($a->{package});
    push @name, &${normalize}($b->{package}) if $a->{package} ne $b->{package};
    push @name, &{$normalize}($a->{program});
    push @name, &{$normalize}($b->{program}) if $a->{program} ne $b->{program};
    push @name, &{$normalize}($a->{comp} . $a->{optim});
    push @name, &{$normalize}($b->{comp} . $b->{optim});
    return lc(join "_", @name);
>>>>>>> b2197eaa90e31bc057b41c066b118716b0b65d87
}


# Run something, but also echo the command. Do nothing if $dry_run is set. Return true on success, false on failure
sub run {
    my(@cmd) = @_;
    print "+ ", join(" ", @cmd), "\n";
    0==system @cmd unless $dry_run;
}

# Cartesian product of arrays. cross [0 1], [a b] = ([0 a], [0 b], [1 a], [1 b])
# Members of the operand arrays are either array-refs or other.
# Any number of arguments are allowed, all of which are arefs
# Returns an array.
sub cross {
    return () unless @_;

    # Rewrite all args to the array-of-arrays format: [[0], [1]]
    my @args;
    for my $arg (@_) {
        die unless 'ARRAY' eq ref $arg;
        push @args, [ map {'ARRAY' eq ref $_ ? $_ : [$_] } @$arg ];
    }

    # Do Cartesian product left-to-right
    my $lhs = shift @args;
    for my $rhs (@args) {
        my $tmp = [];
        for my $a (@$lhs) {
            for my $b (@$rhs) {
                push @$tmp, [ @{$a}, @{$b} ];
            }
        }
        $lhs = $tmp;
    }
    return @$lhs;
}

# Given an array of constraint tuples each containing an even number of members, remove those for which the first
# half of the members are equal to the second half of the members.
sub eliminate_diagonal {
    my(@constraints) = @_;
    print "Eliminated criteria that have the same constraints for both specimens of a pair\n";
    return grep {
        my @members = @{$_};
        my $halflen = @members / 2;
        my $allsame = 1;
        for (my $i=0; $i<$halflen && $allsame; ++$i) {
            $allsame = $members[$i] eq $members[$i+$halflen];
        }
        !$allsame;
    } @constraints;
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
    my($specimens) = @_;
    print "\nexample 1: all specimen pairs (a,b)\n";
    return select_pairs $specimens, sub {1}
}

sub example2 {
    my($specimens) = @_;
    my @constraints = select_random 10, cross [qw/gcc icc/], [qw/s 0/], [qw/gcc icc/], [qw/s 0/];
    return select_tuples $specimens, [qw/compiler optim/], @constraints;
}

sub example3 {
    my($specimens) = @_;
    my @constraints = select_random 10, cross [qw/s 0/], [qw/s 0/];
    return select_tuples $specimens, ['optim'], @constraints;
}

sub example4 {
    my($specimens) = @_;
    print "\nexample 4: specimen pairs (a,b) s.t.\n";
    print "               a_compiler != b_compiler\n";
    return select_pairs $specimens, sub {
        my($a, $b) = @_;
        $a->{compiler} ne $b->{compiler};
    }
}

sub example5 {
    my($specimens) = @_;
    print "\nexample 5: specimen pairs (a, b) s.t.\n";
    print "               a_compiler in {gcc, icc} and\n";
    print "               a_optim in {s, 0}\n";
    return select_pairs $specimens, sub {
        my($a, $b) = @_;
        my %want_C = (gcc=>1, icc=>1);
        my %want_X = ('s'=>1, '0'=>1);
        $want_C{$a->{compiler}} && $want_X{$a->{optim}};
    }
}

#Andreas 2014-02-17
sub example8 {
    my($specimens) = @_;
    print "\nexample 8:
        We need rules for the testing harness to get 10 pairs uniformly at random from:
            X={O0,O1,O2,O3,Os,S3} C={gcc} where S3 is stunnix with gcc O3 as backend.\n\n";
    my @cx = (['stunnix', '3'], cross ['gcc'], [qw/0 1 2 3 s/]);
    my @constraints = select_random 20, cross \@cx, \@cx;
    return select_tuples $specimens, ['compiler','optim'], @constraints;
}

sub example9 {
    my($specimens) = @_;
    print "\nexample 9:
        We need rules for the testing harness to get 10 pairs uniformly at random from:
            C={gcc}, and 5 pairs uniformly at random from each of two sets (the union ten will be returned):
                 I) the first optimization is Os and the other is X={O0,O1,O2,O3} 
                II) the first optimization is stunnix O3 and the other is X={O0,O1,O2,O3}\n\n";
    my @constraints1 = select_random 5, cross [['gcc', 's']],     [['gcc', '0'], ['gcc', '1'], ['gcc', '2'], ['gcc', '3']];
    my @constraints2 = select_random 5, cross [['stunnix', '3']], [['gcc', '0'], ['gcc', '1'], ['gcc', '2'], ['gcc', '3']];
    return (select_tuples($specimens, ['compiler', 'optim'], @constraints1),
            select_tuples($specimens, ['compiler', 'optim'], @constraints2));
}

#Andreas 2014-02-17
sub example10 {
    my($specimens) = @_;
    print "\nexample 10:
        We need rules for the testing harness to get 10 pairs uniformly at random from:
            C={gcc,icc,llvm} and X={O3}\n\n";
<<<<<<< HEAD
    my @cx = cross [qw/gcc icc llvm/], ['0','1','2','3','s'];
    my @constraints = select_random 6, eliminate_diagonal cross \@cx, \@cx;
=======
    my @cx = cross [qw/gcc icc llvm/], ['3'];
    my @constraints = select_random 10, eliminate_diagonal cross \@cx, \@cx;
>>>>>>> b2197eaa90e31bc057b41c066b118716b0b65d87
    return select_tuples $specimens, ['compiler', 'optim'], @constraints;
}

#Andreas 2014-02-17
sub example11 {
    my($specimens) = @_;
    print "\nexample 11:
        We need rules for the testing harness to get 10 pairs uniformly at random from:
            C={gcc,icc,llvm} and X={O0,O1,O2,O3,Os} and stunnix compiled with O3\n\n";
    my @cx = cross [qw/gcc icc llvm/], [qw/0 1 2 3 s/];
    my @constraints = select_random 10, cross \@cx, \@cx;
    return select_tuples $specimens, ['compiler', 'optim'], @constraints;
}

###############################################################################################################################
###############################################################################################################################
# The main program
###############################################################################################################################
###############################################################################################################################

# Generate a list of pairs over which to run
my @pairs = select_random_pairs $max_pairs, example10 \@specimens;

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
    my $config_file = `mktemp`; chomp $config_file;
    open CONFIG, ">", $config_file or die "$config_file: $!\n";
    print CONFIG "dbname='postgresql:///$dbname'\n", $configuration;
    close CONFIG;
    print STDERR "# Configuration file ($config_file):\n", `sed 's/^/    /' $config_file`;
    my($mydir) = $0 =~ /(.*)\//; $mydir ||= ".";
    run "$mydir/run-analysis.sh", "--batch", "--config=$config_file", $a_file, $b_file;
    unlink $config_file;

    my $api_config_file = `mktemp`; chomp $api_config_file;
    open API_CONFIG, ">", $api_config_file or die "$api_config_file: $!\n";
    print API_CONFIG "dbname='postgresql:///$dbname'\n", $api_configuration;
    print STDERR "# Configuration file ($api_config_file):\n", `sed 's/^/    /' $api_config_file`;
    run "$mydir/run-api-similarity.sh", "--batch", "--config=$api_config_file";
    unlink $api_config_file;


}
