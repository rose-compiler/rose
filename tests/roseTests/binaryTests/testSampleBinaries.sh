#!/bin/bash

# Runs various tests on all the sample binaries
# Test output, etc. is to stdout and stderr; progress info is to file descriptor 99
SAMPLE_DIR="$1"    # Name of directory containing binary files to test
CONFIG="$2"        # Name of config file. See testSampleBinaries.conf for details.
DEFAULT_TESTS=skip # Tests to run if none are specified

die() {
    echo "$0: $@" >&2
    exit 1
}

########################################################################################################################
#    Tests.  Their names (sans "test_" are listed in the config file).
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
    ./execFormatsTest -rose:read_binary_file_format_only -rose:binary $sample
}

# A test that runs the disassembler with default settings
test_disassembler() {
    local sample="$1"
    ./disassemble -rose:binary $sample
}

# A test that runs the disassembler and tries to reassemble to the same machine code bytes
test_assembler() {
    local sample="$1"
    ./disassemble -rose:binary --reassemble $sample
}


########################################################################################################################
# End of tests.
###############################################################################################################################






[ -d "$SAMPLE_DIR" ] || die "no sample directory: $SAMPLE_DIR"
[ -n "$CONFIG" ] || CONFIG=/dev/null
failures= nfailures=0
echo "Running binary tests on samples in $SAMPLE_DIR"
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
    done
    (echo >&99) >/dev/null 2>&1
done


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
