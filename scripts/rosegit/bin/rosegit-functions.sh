# This file is sourced by many of the rosegit scripts. It contains shell (Bash) functions.
# Functions are alphabetical. They use underscores in their names while shell scripts use hypens.

# Set up directory where calling script is running, and the base name of that script.
# Note: the extra checks here are to do something reasonable if source directly from the shell PS1 level commandline.
mydir=${0%/*}; mydir=$(cd -- $mydir 2>/dev/null && pwd); [ -n "$mydir" ] || mydir="/bin"
myname="${0##*/}"; [ "$myname" = "-bash" ] && myname="rosegit-functions.sh"

# Debugging utility that to help show what the shell things the arguments are. Call it with any number of arguments and
# they will be emitted on a line to standard error, each enclosed in square brackets.
rosegit_checkargs () {
    perl -e 'print STDERR join(" ",map {"[$_]"} @ARGV), "\n"' -- "$@"
}
    
# Dies with a message
rosegit_die () {
    local myname=${0##*/}
    echo "$myname:" "$@" >&2
    exit 1
}

# Format elapsed time into a human-readable value like 5d2h20m5s
rosegit_elapsed_human () {
    local nsec=$1
    local y= x=$((nsec/86400)) nsec=$((nsec%86400))
    [ $x -gt 0 ] && echo -n "${x}d" && y=yes
    x=$((nsec/3600)) nsec=$((nsec%3600))
    [ -n "$y" -o $x -gt 0 ] && echo -n "${x}h" && y=yes
    x=$((nsec/60)) nsec=$((nsec%60))
    [ -n "$y" -o $x -gt 0 ] && echo -n "${x}m" && y=yes
    echo "${nsec}s"
}

# Configure the environment for building rose. The following environment variables are set if they have no values:
#    ROSE_BLD -- the name of the top of the build tree, set to the absolute name of the current working directory
#    ROSE_SRC -- the name of the top of the source tree
rosegit_environment () {
    [ -n "$ROSE_BLD" ] || ROSE_BLD=$(rosegit_find_builddir)
    [ -n "$ROSE_SRC" ] || ROSE_SRC=$(rosegit_find_sources $ROSE_BLD)

    # Sanity checks
    [ -d "$ROSE_BLD" ] || rosegit_die "built tree is not a directory: $ROSE_BLD"
    [ -d "$ROSE_BLD/.git" ] && rosegit_die "build tree appears to be a repository: $ROSE_BLD"
    [ -d "$ROSE_SRC" ] || rosegit_die "no such directory: ROSE_SRC"
    [ -d "$ROSE_SRC/.git" ] || rosegit_die "not a Git repository: $ROSE_SRC"
    [ "$ROSE_SRC" = "$ROSE_BLD" ] && rosegit_die "build directory and source directory should not be the same: $ROSE_SRC"

    # Make sure directory names are absolute and exported
    export ROSE_BLD=$(cd $ROSE_BLD && pwd)
    export ROSE_SRC=$(cd $ROSE_SRC && pwd)

    # Adjust LD_LIBRARY_PATH so we can run executables without installing them and without going through the libtool shell script.
    # This allows us to run debuggers on the uninstalled executables.
    if [ -d "$BOOST_ROOT" ]; then
	type path-adjust >/dev/null 2>&1 && eval $(path-adjust --var=LD_LIBRARY_PATH remove --regexp /ROSE/ /boost_)
	[ -d "$BOOST_ROOT"] && LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$BOOST_ROOT/lib"
    fi
    LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$ROSE_BLD/src/.libs"
    LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$ROSE_BLD/libltdl/.libs"
    LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$ROSE_BLD/src/3rdPartyLibraries/libharu-2.1.0/src/.libs"
    LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$ROSE_BLD/src/3rdPartyLibraries/qrose/QRoseLib/.libs"
    export LD_LIBRARY_PATH
}

# Filters output of GNU make(1) so that only parts where an error occurred are output. Reads stdin and writes to stdout using
# the rosegit-filter-make-error perl script found in the same directory as the rosegit command that's using this function.
rosegit_filter_make_error () {
    $mydir/rosegit-filter-make-error
}

# Finds the top of a build tree by looking at the specified directory and all ancestors and returning the first one that is
# either empty (a freshly created build tree) or which contains a config.status file (already configured build tree).
rosegit_find_builddir () {
    local dir="$1"; [ -n "$dir" ] || dir=.
    [ -d "$dir" ] || rosegit die "not a directory: $dir"
    dir=$(cd $dir && pwd)
    origdir=$dir
    while [ "$dir" != "/" ]; do
	if [ -f "$dir/config.status" -o -f "$dir/config.log" -o "$(echo $dir/*)" = "$dir/*" ]; then
	    echo $dir
	    return 0
	fi
	dir=$(cd $dir/..; pwd)
    done
    rosegit_die "could not find build dir starting search from $origdir"
}

# Finds a local Git repository containing ROSE by looking first in the specified directory (or current working directory), and
# then in ancestor directories and subdirectories of the ancestors. The subdirectories must be named "sources/$cwdbase", where
# $cwdbase is the base name of the current working directory.
rosegit_find_sources () {
    local dir1="$1"; [ -n "$dir1" ] || dir1=.
    local required="$2"; [ -n "$required" ] || required="src/rose.h"
    [ -d "$dir1" ] || rosegit_die "not a directory: $dir1"
    dir1=$(cd $dir1 && pwd)

    local cwdbase=$(basename $dir1)

    while [ "$dir1" != "/" ]; do
	if [ -d $dir1/.git -a -e "$dir1/$required" ]; then
	    echo $dir1
	    return 0
        fi

	dir2=$dir1/sources/$cwdbase
	if [ -d $dir2/.git -a -e "$dir2/$required" ]; then
	    echo $dir2
	    return 0
	fi

	dir1=$(cd $dir1/..; pwd)
    done
}

# Loads configuration files.  All variables starting with "ROSEGIT" are exported; others are only exported if done so
# explicitly in the configuration files.
rosegit_load_config () {
    local repo="$1";   [ -d "$repo" ]      || rosegit_die "not a repository: $repo"
    local ns="$2";     [ -n "$ns" ]        || ns=$(rosegit_namespace)
    local branch="$3"; [ -n "$branch" ]    || rosegit_die "no branch name supplied"
    local config="$4";                     # optional file or directory

    local confdir=$repo/scripts/rosegit/config
    config=$(eval "echo $config")          # expand tidle, etc.
    if [ -n "$config" ]; then
	if [ -d "$config/." ]; then
	    confdir="$config"
	    config=
        elif [ -f "$config" ]; then
	    :
        elif [ -f "$confdir/$config" ]; then
	    config="$confdir/$config"
	else
	    rosegit_die "unable to find configuration file or directory: $config"
	fi
    fi

    echo -n "$myname configuring:" >&2

    # The defaults.conf must be present (it may be empty). This is a sanity check!
    ROSEGIT_LOADED=
    local defaults=$confdir/defaults.conf
    [ -r $defaults ] || rosegit_die "no configuration file: $defaults"
    rosegit_load_config_file $defaults >&2
    [ -n "$ROSEGIT_LOADED" ] || rosegit_die "$defaults should have set ROSEGIT_LOADED"

    # Load other config files. These are just shell scripts. The later, more specific files can override what the earlier ones did.
    rosegit_load_config_file $confdir/$ns.conf >&2
    rosegit_load_config_file $confdir/$ns.$branch.conf    >&2
    [ -f "$config" ] && rosegit_load_config_file $config  >&2

    echo >&2

    # Export variables
    eval $(set |sed -n '/^ROSEGIT/s/^\(ROSEGIT[a-zA-Z_0-9]*\).*/export \1/p')

}

# Loads one configuration file
rosegit_load_config_file () {
    local cfile="$1";   [ -n "$cfile" ] || rosegit_die "no configuration file specified"
    echo -n " $(basename $cfile .conf)"
    if [ -f $cfile ]; then
	source $cfile || rosegit_die "cannot source file: $cfile"
	echo -n "[ok]"
    else
	echo -n "[no]"
    fi
}

# Runs ROSEGIT_MAKE. Used by test scripts.
rosegit_make () {
    eval "$ROSEGIT_MAKE" "$@"
}

# Echoes the current namespace based on the setting of the ROSEGIT_NAMESPACE variable. If this variable is empty then we generate
# a namespace based on the gcos field of /etc/passwd for the effective user, or the first three letters of the effective user
# login name. The result is cached in ROSEGIT_NAMESPACE.
rosegit_namespace () {
    local ns="$ROSEGIT_NAMESPACE"
    if [ ! -n "$ns" ]; then
       local euser=$(whoami)
       ns=$(grep "^$euser:" /etc/passwd |cut -d: -f5 |perl -ane 'print map {substr lc,0,1} @F')
    fi
    [ -n "$ns" ] || ns=$(whoami)
    [ -n "$ns" ] || ns=$USER
    [ -n "$ns" ] || rosegit_die "could not determine namespace; please set the ROSEGIT_NAMESPACE config variable."
    ns=$(echo "$ns""xxx" |cut -c1-3)
    ROSEGIT_NAMESPACE="$ns"
    echo "$ns"
}

# All scripts should call this function first to make sure the environment is set up properly
rosegit_preamble () {
    local config="$1";      # optional configuration file or directory
    if [ ! -n "$ROSEGIT_LOADED" ]; then
	local blddir=$ROSE_BLD; [ -n "$blddir" ] || blddir=$(rosegit_find_builddir)
	local srcdir=$ROSE_SRC; [ -n "$srcdir" ] || srcdir=$(rosegit_find_sources $blddir)
	[ -d "$srcdir" ] || rosegit_die "could not find source directory"
	local branch=$(cd $srcdir && git branch |sed -n '/^\*/s/^\* //p')
	rosegit_load_config $srcdir $(rosegit_namespace) $branch $config
	rosegit_environment
    fi

    # Prevent errors about the progress report file being closed.
    (echo -n "">&6) 2>/dev/null || exec 6>/dev/null
}

# Echos various environment settings to aid in debugging.
rosegit_show_environment () {
    echo "Date:              $(date)"
    echo "User:              $(whoami) [$(rosegit_namespace)] pid=$$"
    echo "Machine:           $(hostname --long) [$(hostname --ip-address)]"
    echo "Operating system:  $(uname -s) $(uname -r) $(uname -v)"
    echo "Architecture:      $(uname -m) $(uname -i) $(uname -p)"
    echo "ROSE source tree:  $ROSE_SRC"
    echo "ROSE build tree:   $ROSE_BLD"
    echo "Software:"
    echo "    $(make --version |head -n1)"
    echo "    $(gcc --version |head -n1)"
    echo "    $(g++ --version |head -n1)"
    echo "    $(bison --version |head -n1)"
    echo "    doxygen $(doxygen --version)"
    echo "    $(dot -V 2>&1)"
    echo "    $(libtool --version |head -n1)"
    echo "    $(tex --version |head -n1)"
    echo "    $(latex --version |head -n1)"
    echo "    $(swig -version |grep -i version)"
    echo "Configuration:"
    eval "perl -e 'print qq{    \$_\n} for @ARGV' -- $ROSEGIT_CONFIGURE"
}
