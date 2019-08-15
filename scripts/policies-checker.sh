#!/bin/bash
#
# This shell script is the harness for running a set of policy-enforcing scripts.  This harness can be run from any location
# in the ROSE source tree at any time during development. We enforce a set of policies on ROSE developers.  Each policy is
# enforced by a single script in the $ROSESRC/scripts/policies.d directory.
#

# DQ (3/9/2009): Additional things that we can enforce within ROSE (written by Andreas).
# Additional rules to enforce:
#    2) Finding link dependencies in Makefile.am files:
#       find -name Makefile.am -exec grep -nPH "^[ \t]*[a-zA-Z]*[_]?LDADD[ \t]*" {} \;

# Find the directory containing the scripts that enforce individual policies
mydir="${0%/*}"
arg0="${0#$mydir/}"
policies_dir="$mydir/policies"
if [[ ! -d "$policies_dir" ]]; then
    echo "$arg0: cannot find policies directory: $policies_dir" >&2
    exit 1
fi

# Turn a CamelCase file name into something more friendly
demangle () {
    local s="$1"
    s="${s##*/}" # strip of directory part if any
    s="${s%%.*}" # strip of extensions
    s="$(echo "$s" |sed 's/\([A-Z]\)/ \1/g')" # de-CamelCase
    s="${s# }" # remove leading space from de-CamelCase if necessary
    s="$(echo "$s" |tr A-Z a-z)" # convert to all lower-case
    echo "$s"
}

# Run each executable file and keep track of exit status:
#     0         success
#     1-127     failure
#     128-256   warning
nsuccesses=0
nwarnings=0
nerrors=0
for script in "$policies_dir"/*; do
    [ "$script" != "${script%'~'}" ] && continue        # editor temp file
    [ "$script" != "${script%.disabled}" ] && continue  # script is disabled
    [ -x "$script" ] || continue
    name="$(demangle $script)"
    echo "  POLICY  Checking $name policy ($script)" # "POLICY" causes line to be emited by scripts/rosegit/bin/rosegit-make
    $script 2>&1 |sed 's/^/    /'
    status=${PIPESTATUS[0]}
    if [[ $status = 0 ]]; then
	nsuccesses=$[nsuccesses + 1]
    elif [[ $status -lt 128 ]]; then
#	nerrors=$[nerrors + 1]
	echo "Errors detected by the $name policy. Aborting now."
	exit 1
    else
	nwarnings=$[nwarnings + 1]
    fi
    echo
done

# Final report and exit
echo "Policies checked:  success=$nsuccesses; warnings=$nwarnings; errors=$nerrors"
[ $nerrors -ne 0 ] && exit 1
exit 0
