#!/bin/bash
# Runs syntactic analysis.  Give zero or more specimens as arguments.

argv0="${0##*/}"
dir0="${0%$argv0}"
[ -n "$dir0" ] || dir0="."
worklist="clone-analysis-$$"

usage () {
    local exit_status="${1:-0}"
    echo "usage: $argv0 [SWITCHES] [--] [SPECIMENS...]" >&2
    echo "    This command runs the syntactic analysis steps interactively.  The following" >&2
    echo "    switches are understood:" >&2
    echo "        --batch" >&2
    echo "            Instead of being interactive, use only the default values stored in" >&2
    echo "            the configuration file.  Beware of the \$recreate value, which might" >&2
    echo "            cause the database to be recreated from scratch each run rather than" >&2
    echo "            building it incrementally, or vice versa." >&2
    echo "        --config=FILE" >&2
    echo "            Use the specified file for reading and/or storing configuration info." >&2
    echo "            the default file is '.run-analysis.conf'." >&2
    echo "        --[no-]save" >&2
    echo "            The --save switch (the default) saves configuration information into the" >&2
    echo "            configuration file specified with the --config switch (or a default)." >&2
    exit $exit_status
}

# Show an error message and exit
die () {
    echo "$argv0: $*" >&2
    exit 1
}

# For affirmative answers, echo "yes" and return success; otherwise silently return failure
yes_or_no () {
    local prompt="$1" dflt="$2" ans
    read -e -p "$prompt " -i "$dflt" ans
    [ "$ans" = "" ] && ans="$dflt"
    if [ "${ans:0:1}" = "y" -o "${ans:0:1}" = "Y" ]; then
	echo "yes"
	return 0
    fi
    return 1;
}

# Execute the command, and show users what it is
execute () {
    echo "+" "$@" >&2
    "$@"
    return $?
}

# Counts the number of arguments
count_args () {
    echo "$#"
}

save_settings () {
    if [ "$save_config" = "yes" ]; then
	cat >$configfile <<EOF
# Name or URL of the database
dbname='$dbname'

# Should the database tables be dropped and recreated?
recreate='$recreate'

# Switches for adding specimens to the database
add_functions_flags='$add_functions_flags'

# Switches for the createVectorsBinary command
create_vectors_flags='$create_vectors_flags'

# Switches for the findClones command
find_clones_flags='$find_clones_flags'
EOF
    fi
}

###############################################################################################################################
###############################################################################################################################

# Parse command-line
interactive=yes
configfile=.run-analysis.conf
save_config=yes
while [ "$#" -gt 0 -a "${1:0:1}" = "-" ]; do
    arg="$1"; shift
    case "$arg" in
	--)
	    break
	    ;;
	--help|-h)
	    usage 0
	    ;;
	--batch)
	    interactive=""
	    ;;
	--config=*)
	    configfile=${arg#--config=}
	    ;;
	--save)
	    save_config=yes
	    ;;
	--no-save)
	    save_config=
	    ;;
	*)
	    echo "$argv0: unknown command-line switch: $arg" >&2
	    echo "$argv0: see --help for more info" >&2
	    exit 1
	    ;;
    esac
done

if [ -r "$configfile" ]; then
    echo "$0: reading from config file: $configfile" >&2
    . "$configfile"
else
    echo "$0: creating configuration file: $configfile" >&2
fi

: ${ROSE_SRC:=$ROSEGIT_SRC}
: ${ROSE_BLD:=$ROSEGIT_BLD}
: ${SRCDIR:=$ROSE_SRC/projects/BinaryCloneDetection/syntactic}
: ${BLDDIR:=$ROSE_BLD/projects/BinaryCloneDetection/syntactic}

[ -n "$ROSE_SRC" -a -d "$ROSE_SRC/projects" ] || die "ROSE_SRC should be set to the root of the ROSE source directory"
[ -n "$ROSE_BLD" -a -d "$ROSE_BLD/projects" ] || die "ROSE_BLD should be set to the root of the ROSE build directory"
[ -n "$SRCDIR" -a -d "$SRCDIR" ]              || die "not a directory: $SRCDIR"
[ -n "$BLDDIR" -a -d "$BLDDIR" ]              || die "not a directory: $BLDDIR"

# Make sure everything we need is built.  We run make here for explicitly specified targets because "make all"
# often fails in this directory.
make -j16 -C $BLDDIR/../semantic || exit 1
make -j16 -C $BLDDIR createSchema createVectorsBinary findClones lshCloneDetection computeClusterPairs || exit 1

if [ "$interactive" = "yes" ]; then
    echo "=================================================================================================="
    echo "Specify a database which will hold information about the analysis.  The database can be either a"
    echo "file name (SQLite3 database) or a URL.  URLs that have the following syntax:"
    echo "    postgresql://[USER[:PASSWORD]@]HOST[:PORT][/DATABASE][?PARAM=VALUE&...]"
    echo "    sqlite3://[FILENAME[?PARAM[=VALUE]&...]"
    echo "SQLite3 databases are created if they don't exist; PostgreSQL databases must be created before this"
    echo "analysis runs (see the 'createdb' man page)."
    read -e -p "Database name: " -i "$dbname" dbname
fi

if [ "$interactive" = "yes" ]; then
    echo
    echo "=================================================================================================="
    echo "The analysis can incrementally build the database a few specimens at a time. If you want to delete"
    echo "all existing tables and start over, then enter 'no' for this question..."
    echo
    echo "Note: syntactic analysis results generally cannot be built incrementally, so it is best to always"
    echo "      answer 'yes' to this prompt."
    recreate=$(yes_or_no "Do you want to (re)create the database?" ${recreate:-no})
    save_settings
fi
if [ "$recreate" = "yes" ]; then
    execute $BLDDIR/createSchema --drop "$dbname" || exit 1
    execute $BLDDIR/../semantic/00-create-schema "$dbname" || exit 1
    execute $BLDDIR/createSchema "$dbname" || exit 1
fi

if [ "$#" -gt 0 ]; then
    if [ "$interactive" = "yes" ]; then
	echo
	echo "=================================================================================================="
	echo "These are the flags for the 11-add-functions command, which parses a specimen and adds its"
	echo "functions to the database."
	$BLDDIR/../semantic/11-add-functions --help 2>&1 |sed -n '/^$/,/^  *DATABASE$/ p' |tail -n +2 |head -n -1
	read -e -p "Switches for adding specimens: " -i "$add_functions_flags" add_functions_flags
	save_settings
    fi
    for specimen in "$@"; do
	export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$BLDDIR/../semantic"
	execute $BLDDIR/../semantic/11-add-functions $add_functions_flags "$dbname" "$specimen" || exit 1
    done
fi

if [ "$interactive" = "yes" ]; then
    echo
    echo "=================================================================================================="
    echo "These are the flags for the createVectorsBinary command, creates syntax vectors for each of the"
    echo "functions to the database."
    $BLDDIR/createVectorsBinary --help 2>&1
    read -e -p "Switches for creating vectors: " -i "$create_vectors_flags" create_vectors_flags
    save_settings
fi
execute $BLDDIR/createVectorsBinary --database "$dbname" $create_vectors_flags || exit 1

if [ "$interactive" ]; then
    echo
    echo "=================================================================================================="
    echo "These are the flags for the findClones command. You do not need to specify the --database switch."
    $BLDDIR/findClones --help 2>&1
    read -e -p "Switches for for findClones: " -i "$find_clones_flags" find_clones_flags
    save_settings
fi
execute $BLDDIR/findClones --database "$dbname" $find_clones_flags
execute $BLDDIR/computeClusterPairs "$dbname"
