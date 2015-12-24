#!/bin/bash

# Answers the question: Which Makefiles support "make check" but are not using $(RTH_RUN)?

if [ "$#" -ne 1 ]; then
    echo "usage: $0 directory" >&2
    exit 1
fi
root_dir="$1"
[ "$root_dir" != "/" ] && root_dir="${root_dir%/}"

echo "|-"
for dir in $(find "$root_dir" -name Makefile.am |sed 's%/Makefile.am$%%' |sort -f); do
    makefile="$dir/Makefile.am"
    dir="${dir#$root_dir}"
    dir="${dir#/}"
    [ "$dir" = "" ] && dir="."


    has_check_rule=
    if grep '^check-local:' "$makefile" >/dev/null; then
	has_check_rule=yes
    fi

    has_test_code_lists=
    if grep '^TESTCODES_' "$makefile" >/dev/null; then
	has_test_code_lists=yes
    fi

    has_rth_run=
    if grep RTH_RUN "$makefile" >/dev/null; then
	has_rth_run=yes
    fi

    uses_rth=
    if [ "$has_rth_run" ]; then
	uses_rth="yes"
    elif [ "$has_check_rule" != "" -o "$has_test_code_lists" != "" ]; then
	uses_rth="no"
    else
	uses_rth="N/A"
    fi

    # Output as a Emacs tbl-mode table for easy formatting
    echo "| $uses_rth | " $(echo "$dir" |sed 's%/% | %g') " |"
done
echo "|-"
