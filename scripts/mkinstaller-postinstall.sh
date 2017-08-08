#!/bin/bash
set -e

arg0="${0##*/}"

# This script is run on the user's machine when they unpack a ROSE binary distribution by running the script that a
# developer created with $ROSE/scripts/mkinstaller.

# Parse arguments
prefix_bin=/usr/local
prefix_lib=/usr/local
prefix_include=/usr/local

parse_cmdline() {
    while [ "$#" -gt 0 ]; do
	case "$1" in
	    --)
		shift
		break
		;;

	    --help|-h)
		######--------------------------------------------------------------------------------
		echo "usage: $arg0 [SWITCHES]"
		echo "switches:"
		echo "    --prefix_bin=DIR, --prefix_lib=DIR, --prefix_include=DIR"
		echo "        Directories on the user's machine where executables, libraries and"
		echo "        include files are unpacked by the intaller script."
		eixt 0
		;;

	    --prefix_bin=*)
		prefix_bin="${1#*=}"
		shift
		;;

	    --prefix_lib=*)
		prefix_lib="${1#*=}"
		shift
		;;

	    --prefix_include=*)
		prefix_include="${1#*=}"
		shift
		;;

	    --prefix_bin)
		prefix_bin="$2"
		shift 2
		;;

	    --prefix_lib)
		prefix_lib="$2"
		shift 2
		;;

	    --prefix_include)
		prefix_include="$2"
		shift 2
		;;

	    -*)
		echo "$arg0: unrecognized command-line switch: \"$1\"" >&2
		exit 1
		;;

	    *)
		echo "usage: $arg0 [SWITCHES]; see --help" >&2
		exit 1
	esac
    done
}


########################################################################################################################
adjust_rose_config() {
    # Remove all -I arguments from the cppflags string, except change the first one to $prefix_include
    local output="$prefix_lib/lib/rose-config.cfg"
    local input="$output.bak"

    cp "$output" "$input"
    awk \
	-v "PREFIX=$prefix_bin" \
	-v "LIBDIR=$prefix_lib/lib" \
	-v "INCDIR=$prefix_include/include" \
	'
            /^cppflags/ {
                sub(/-I[^ ]*/, "-FIRST_I", $0);
                gsub(/-I[^ ]*/, "", $0);
                gsub(/-FIRST_I/,"-I" INCDIR, $0);
                print;
                next }

            /^ldflags/ {
                sub(/-L[^ ]*/, "-FIRST_L", $0);
                gsub(/-L[^ ]*/, "", $0);
                gsub(/-FIRST_L/, "-L" LIBDIR, $0);
                print;
                next }

            /^libdirs/ {
                print "libdirs  = " LIBDIR;
                next }

            /^prefix/ {
                print "prefix   = " PREFIX;
                next }

            { print }
	' <"$input" >"$output"
}


########################################################################################################################
parse_cmdline "$@"
adjust_rose_config
exit 0
