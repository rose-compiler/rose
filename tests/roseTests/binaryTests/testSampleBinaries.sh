#!/bin/bash

# Runs various tests on all the sample binaries
# Test output, etc. is to stdout and stderr; progress info is to file descriptor 99

########################################################################################################################
#    Tests.  Their names (sans "test_" are listed in the config file).
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


########################################################################################################################
# End of tests.
###############################################################################################################################




die() {
    echo "$0: $@" >&2
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
[ "$#" -gt 2 ] && die "usage: SAMPLE_DIR [CONFIG_FILE]"
SAMPLE_DIR="$1"    # Name of directory containing binary files to test
CONFIG="$2"        # Name of config file. See testSampleBinaries.conf for details.
DEFAULT_TESTS=skip # Tests to run if none are specified
[ -d "$SAMPLE_DIR" ] || die "no sample directory: $SAMPLE_DIR"
[ -n "$CONFIG" ] || CONFIG=/dev/null

failures= nfailures=0
if [ -n "$do_cleanup" ]; then
    echo "Cleaning up test files for samples in $SAMPLE_DIR"
else
    echo "Running binary tests on samples in $SAMPLE_DIR"
fi

for sample in $(find $SAMPLE_DIR \( -type d -name .svn -prune -false \) -o -type f |sort); do
    sampleshort="${sample#$SAMPLE_DIR/}"
    samplebase="${sample##*/}"

    # Skip certain file names
    case "$samplebase" in
	*.txt) continue ;;
	Makefile.*) continue ;;
	*~)         continue ;;
    esac

    # Look for the config entry using the full name returned by the "find" command above, then the name with the SAMPLE_DIR
    # part stripped off the front, then just the file name with no directory component, and finally a "*" entry.  If all that
    # fails then use the default tests defined above (probably "skip" or "fail")
    tests="$(egrep ^$sample: $CONFIG)"
    [ -n "$tests" ] || tests="$(egrep ^$sampleshort: $CONFIG)"
    [ -n "$tests" ] || tests="$(egrep ^$samplebase: $CONFIG)"
    [ -n "$tests" ] || tests="$(egrep '^\*:' $CONFIG)"
    [ -n "$tests" ] || tests="*: $DEFAULT_TESTS"
    tests="${tests##*:}" # strip off the sample name
    (echo -n "$sampleshort:" >&99) >/dev/null 2>&1

    for testname in $tests; do
	if [ -n "$do_cleanup" ]; then
	    eval "test_$testname '$sample' cleanup"
	else
	    (echo -n " $testname" >&99) >/dev/null 2>&1
	    echo
	    echo
	    echo
	    echo "==============================================================================================="
	    echo "===   $testname $sample"
	    echo "==============================================================================================="

	    if eval "test_$testname '$sample'"; then
		: ok
	    else
		echo "FAILED: $testname $sample" >&2
		(echo -n "[FAILED]" >&99) >/dev/null 2>&1
		failures="$failures $sampleshort[$testname]"
		nfailures=$[nfailures+1]
	    fi
	fi
    done
    (echo >&99) >/dev/null 2>&1
done


if [ -n "$do_cleanup" ]; then
    :
else
    echo "==============================================================================================="
    echo "===   Summary"
    echo "==============================================================================================="
    (echo "Number of failed tests: $nfailures" >&99) >/dev/null 2>&1
    if [ $nfailures -eq 0 ]; then
	echo "All tests passed!"
	exit 0
    else
	echo "Encountered $nfailures failures:"
	echo "$failures" | tr ' ' '\n' |sed 's/^/    /'
	echo "Test(s) failed!"
	exit 1
    fi
fi