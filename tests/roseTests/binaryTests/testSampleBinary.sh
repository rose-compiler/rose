#!/bin/bash
#
# Runs various tests on a single sample binary file given as a command-line argument.

########################################################################################################################
#    Tests.  Their names (sans "test_" are listed in the config file, testSampleBinaries.conf).
#    First argument is the name of the executable which should be tested. If a second argument is present then the
#    test should clean up instead of run.
########################################################################################################################

# A test that always passes; an alias for "true" that makes more sense in the output
test_skip() {
    return 0
}

# A test that always fails; an alias for "false" that makes more sense in the output
test_fail() {
    return 1
}

# A test that just parses the binary container without disassembling
test_parser() {
    local sample="$1"
    local cleanup="$2"
    if [ -n "$cleanup" ]; then
	local basename=$(basename $sample)
	rm -f rose_$basename.s $basename.new $basename.dump
    else
	(set -x; ./execFormatsTest -rose:read_binary_file_format_only -rose:binary $sample) || return 1
    fi
}

# A test of the unparser that tries to parse what was unparsed in the 'test_parser' test.
test_unparser() {
    local sample="$1"
    local cleanup="$2"
    sample="$(basename $sample).new"
    if [ -n "$cleanup" ]; then
	rm -f rose_$sample.s $sample.new $sample.dump
    else
	(set -x; ./execFormatsTest -rose:read_binary_file_format_only -rose:binary $sample) || return 1
    fi
}

# A test that runs the disassembler with default settings
test_disassembler() {
    local sample="$1"
    local cleanup="$2"
    if [ -n "$cleanup" ]; then
	local basename=$(basename $sample)
	rm -f rose_$basename.s $basename.new $basename.dump
    else
	(set -x; ./disassemble -rose:binary --quiet --show-bad $sample) || return 1
    fi
}

# A test that runs the disassembler and tries to reassemble to the same machine code bytes
test_assembler() {
    local sample="$1"
    local cleanup="$2"
    if [ -n "$cleanup" ]; then
	local basename=$(basename $sample)
	rm -f rose_$basename.s $basename.new $basename.dump
    else
	(set -x; ./disassemble -rose:binary --quiet --reassemble --show-bad $sample) || return 1
    fi
}

# A test for read/write of the AST to a file.
test_astio() {
    local sample="$1"
    local cleanup="$2"
    if [ -n "$cleanup" ]; then
	local basename=$(basename $sample)
	rm -f $basename-{1,2}.dump $basename.ast
    else
	(set -x; ./testAstIO -rose:binary $sample) || return  1
    fi
}

########################################################################################################################
# End of tests.
###############################################################################################################################




die() {
    local myname=${0##*/}
    echo "$myname: $@" >&2
    exit 1
}

# Parse command-line arguments
do_cleanup=
while [ "$#" -gt 0 ]; do
    case "$1" in
	--cleanup)
	    do_cleanup=yes
	    shift
  	    ;;
	-*)
	    die "unknown command-line option: $1"
	    exit 1
	    ;;
	*)
	    break
	    ;;
    esac
done
[ "$#" -lt 1 -o "$#" -gt 2 ] && die "usage: BINARY_FILE [CONFIG_FILE]"
sample="$1"        # Name of binary file to test
CONFIG="$2"        # Name of config file. See testSampleBinaries.conf for details.
DEFAULT_TESTS=skip # Tests to run if none are specified
[ -f "$sample" ] || die "no sample binary: $sample"
[ -n "$CONFIG" ] || CONFIG=/dev/null

nfailures=0
samplebase="${sample##*/}"

# Look for the config entry using the full sample name, then just the file base name, and finally a "*" entry.  If all that
# fails then use the default tests defined above (probably "skip" or "fail")
tests="$(egrep ^$sample: $CONFIG)"
[ -n "$tests" ] || tests="$(egrep ^$samplebase: $CONFIG)"
[ -n "$tests" ] || tests="$(egrep '^\*:' $CONFIG)"
[ -n "$tests" ] || tests="*: $DEFAULT_TESTS"
tests="${tests##*:}" # strip off the sample name
tests="${tests%%#*}" # strip off comment, if any

for testname in $tests; do
    if [ -n "$do_cleanup" ]; then
	eval "test_$testname '$sample' cleanup"
    else
	echo "  TESTING $testname $sample" >&2
	echo
	echo
	echo
	echo "==============================================================================================="
	echo "===   $testname $sample"
	echo "==============================================================================================="

	if eval "(test_$testname '$sample' 2>&1)"; then
	    : ok
	else
	    echo "FAILED: $testname $sample" >&2
	    nfailures=$[nfailures+1]
	fi
    fi
done

[ $nfailures -gt 0 ] && exit 1
exit 0
