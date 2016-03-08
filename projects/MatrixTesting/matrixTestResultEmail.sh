#!/bin/bash

# Updates the database from an email provided as standard input
: ${DATABASE:=postgresql://rose:fcdc7b4207660a1372d0cd5491ad856e@www.hoosierfocus.com/rose_matrix}

# Directory containing either the installed version of ROSE (library and matrix tools) or the
# root of an RMC build tree where projects/MatrixTesting has been compiled.
if [ "$MATRIX_HOME" = "" ]; then
    if [ "$RMC_ROSEBLD_ROOT" != "" ]; then
	# If we've got our developer hat on, use the matrix tools we're developing.
	MATRIX_HOME="$RMC_ROSEBLD_ROOT"
    elif [ -d "/home/matzke/rose-matrix/rose" ]; then
	# Use the matrix tools that are installed on the system
	MATRIX_HOME=/home/matzke/rose-matrix/rose
    else
	echo "$0: cannot find matrix tools" >&2
	exit 1
    fi
fi


########################################################################################################################
# Create files for the entire message, the headers, and the body
whole_message=$(tempfile)
mail_headers=$(tempfile)
mail_body=$(tempfile)
sed 's/\r$//' >$whole_message
sed -n '1,/^$/p' <$whole_message >$mail_headers
sed -n '/^$/,$p' <$whole_message >$mail_body
trap "rm -f $whole_message $mail_headers $mail_body" EXIT

# Run a matrix command.
run_matrix_command() {
    local cmd="$1"; shift
    if [ -r "$MATRIX_HOME/.rmc-main.cfg" ]; then
	# The tools live in an RMC build tree, so we can get library information easily
	rmc -C "$MATRIX_HOME/projects/MatrixTesting" "./$cmd" "$@"
    elif [ -d "$MATRIX_HOME/bin" ]; then
	# Assume the matrix commands have been installed. Some of these variables depend on the specific ROSE configuration.
	local rose_libdir="$MATRIX_HOME/lib"
	local rose_bindir="$MATRIX_HOME/bin"
	local tool_chain="/home/matzke/GS-CAD"
	local boost_libdir="$tool_chain/boost/1.50/gcc-4.8.4-default/lib"
	local readline_libdir="$tool_chain/readline/6.3/lib"
	local yaml_libdir="$tool_chain/yaml/0.5.1/boost-1.50/gcc-4.8.4-default/lib"
	local jre_libdir="/usr/lib/jvm/jdk1.7.0_51/jre/lib/amd64/server"
	local wt_libdir="$tool_chain/wt/3.3.4/boost-1.50/gcc-4.8.4-default/lib"

	LD_LIBRARY_PATH="$rose_libdir:$readline_libdir:$jre_libdir:$yaml_libdir:$wt_libdir:$boost_libdir" "$rose_bindir/$cmd" "$@"
    else
	echo "$0: cannot run $cmd"
    fi
}

# Authenticate sender before processing message. For now this is very simplistic!
authenticate_sender() {
    if [ $(sed -n '/^Subject: matrix test result$/p' $mail_headers |wc -l) -ne 1 ]; then
	echo "$0: invalid subject line" >&2
	exit 1
    fi
    return 0
}

# Any part of the body that looks like a var=value setting needs to be collected. Try to weed out malicious stuff by
# being quite strict about what we accept.
var_val_pairs() {
    sed -n '1,/^====/ s/^\([a-zA-Z_][a-zA-Z_0-9]*\)[ \t]*=[ \t]*\([-+., a-zA-Z_0-9]*\)$/\1='"'"'\2'"'"'/p' <$mail_body |\
	tr '\n' ' '
}

# Update the test_results table and echo the new test number
update_test_results() {
    local kvpairs=$(var_val_pairs);
    eval "run_matrix_command matrixTestResult -L 'tool(>=trace)' --database='$DATABASE' $kvpairs"
}

# Attach commands to the test
attach_commands() {
    local test_id="$1"
    local tmp=$(tempfile)

    sed -n '/==== COMMANDS BEGIN ====/,/==== COMMANDS END ====/ p' <$mail_body |\
	head -n -1 |tail -n +2 |\
	base64 -d >$tmp

    if [ -s "$tmp" ]; then
	run_matrix_command matrixAttachments --database="$DATABASE" --attach --title="Commands" "$test_id" $tmp
    fi
    rm -f $tmp
}

# Attach command output to the test
attach_output() {
    local test_id="$1"
    local tmp=$(tempfile)

    sed -n '/==== COMMAND OUTPUT BEGIN ====/,/==== COMMAND OUTPUT END ====/ p' <$mail_body |\
	head -n -1 |tail -n +2 |\
	base64 -d >$tmp

    if [ -s "$tmp" ]; then
	run_matrix_command matrixAttachments --database="$DATABASE" --attach --title="Final output" "$test_id" $tmp
    fi
    rm -f $tmp
}

########################################################################################################################
authenticate_sender
test_id=$(update_test_results)
attach_commands $test_id
attach_output $test_id
run_matrix_command matrixErrors --database="$DATABASE" update
