# This file is sourced by many of the rosegit scripts. It contains shell (Bash) functions.
# Functions are mostly alphabetical. They use underscores in their names while shell scripts use hyphens.

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
    local myname="${0##*/}"
    echo "$myname:" "$@" >&2
    exit 1
}

# Prints a warning message
rosegit_warn () {
    local myname="${0##*/}"
    echo "$myname:" "$@" >&2
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

# Configure certain environment variables:
#   Adjust LD_LIBRARY_PATH so we can run executables without installing them and without going through the libtool shell script.
#   This allows us to run debuggers on the uninstalled executables.  DYLD_LIBRARY_PATH is for Darwin.
rosegit_environment () {
    [ -d "$ROSEGIT_SRC" ] || rosegit_die "no source directory"
    [ -d "$ROSEGIT_BLD" ] || rosegit_die "no build directory"

    if [ -d "$BOOST_ROOT" ]; then
	type path-adjust >/dev/null 2>&1 && eval $(path-adjust --var=LD_LIBRARY_PATH remove --regexp /ROSE/ /boost_)
	[ -d "$BOOST_ROOT" ] && LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$BOOST_ROOT/lib"
    fi
    LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$ROSEGIT_BLD/src/.libs"
    LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$ROSEGIT_BLD/libltdl/.libs"
    LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$ROSEGIT_BLD/src/3rdPartyLibraries/libharu-2.1.0/src/.libs"
    LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$ROSEGIT_BLD/src/3rdPartyLibraries/qrose/QRoseLib/.libs"
    export LD_LIBRARY_PATH
    export DYLD_LIBRARY_PATH="$LD_LIBRARY_PATH"
}

# Filters output of GNU make(1) so that only parts where an error occurred are output. Reads stdin and writes to stdout using
# the rosegit-filter-make-error perl script found in the same directory as the rosegit command that's using this function.
rosegit_filter_make_error () {
    $mydir/rosegit-filter-make-error
}

# Finds the top of a build tree by looking at the specified directory and all ancestors and returning the first one that is
# either empty (a freshly created build tree) or which contains eitehr a config.status or CMakeCache.txt file (already
# configured build tree).
rosegit_find_builddir () {
    local dir="$1"; [ -n "$dir" ] || dir=.
    [ -d "$dir" ] || rosegit die "not a directory: $dir"
    dir=$(cd $dir && pwd)
    origdir=$dir
    while [ "$dir" != "/" ]; do
	if [ -f "$dir/config.status" -o -f "$dir/config.log" -o -f "$dir/CMakeCache.txt" -o "$(echo $dir/*)" = "$dir/*" ]; then
	    echo $dir
	    return 0
	fi
	dir=$(cd $dir/..; pwd)
    done
    rosegit_die "could not find build dir starting search from $origdir"
}

# Finds source and build directories
#    ROSEGIT_BLD -- the name of the top of the build tree, set to the absolute name of the current working directory
#    ROSEGIT_SRC -- the name of the top of the source tree corresponding to this build tree.
#    ROSE_SRC    -- the name of the top of the source tree for ROSE (same as ROSEGIT_SRC if we're compiling ROSE itself)
rosegit_find_directories () {
    [ -n "$ROSEGIT_BLD" ] || ROSEGIT_BLD=$(rosegit_find_builddir)
    [ -n "$ROSEGIT_SRC" ] || ROSEGIT_SRC=$(rosegit_find_sources $ROSEGIT_BLD)

    # Sanity checks
    [ -d "$ROSEGIT_BLD" ] || rosegit_die "built tree is not a directory: $ROSEGIT_BLD"
    [ -d "$ROSEGIT_BLD/.git" ] && rosegit_die "build tree appears to be a repository: $ROSEGIT_BLD"
    [ -d "$ROSEGIT_SRC" ] || rosegit_die "no such directory: $ROSEGIT_SRC"
    [ "$ROSEGIT_SRC" = "$ROSEGIT_BLD" ] && rosegit_die "build directory and source directory should not be the same: $ROSEGIT_SRC"

    # Make sure directory names are absolute and exported
    export ROSEGIT_BLD=$(cd $ROSEGIT_BLD && pwd)
    export ROSEGIT_SRC=$(cd $ROSEGIT_SRC && pwd)

    # Find the ROSE source tree
    [ -f "$ROSEGIT_SRC/src/rose.h" ] && ROSE_SRC=$ROSEGIT_SRC
    [ -z "$ROSE_SRC" -a -f "$ROSEGIT_SRC/config/ROSE_SOURCES" ] && ROSE_SRC=$(cat $ROSEGIT_SRC/config/ROSE_SOURCES)
    [ -d "$ROSE_SRC" ] || rosegit_die "cannot find ROSE source tree (perhaps no \$ROSE_SRC environment variable?)"
    export ROSE_SRC
}

# Finds the top of the ROSE source tree by looking first in the specified directory (or current working directory),
# and then in ancestor directories and subdirectories of the ancestors. The subdirectories must be named "sources/$cwdbase", where
# $cwdbase is the base name of the current working directory.
rosegit_find_sources () {
    local dir1="$1"; [ -n "$dir1" ] || dir1=.
    [ -d "$dir1" ] || rosegit_die "not a directory: $dir1"
    dir1=$(cd $dir1 && pwd)

    local cwdbase=$(basename $dir1)

    while [ "$dir1" != "/" ]; do
	for required in configure.in configure.ac; do
	    if [ -e "$dir1/$required" ]; then
		echo $dir1
		return 0
	    fi

	    dir2=$dir1/sources/$cwdbase
	    if [ -e "$dir2/$required" ]; then
		echo $dir2
		return 0
	    fi
	done
	dir1=$(cd $dir1/..; pwd)
    done
}

# Loads configuration files.  All variables starting with "ROSEGIT" are exported; others are only exported if done so
# explicitly in the configuration files.
rosegit_load_config () {
    local repo="$1";   [ -d "$repo" ]      || rosegit_die "not a repository: $repo"
    local ns="$2";     [ -n "$ns" ]        || ns=$(rosegit_namespace)
    local branch="$3"; [ -n "$branch" ]    || branch="none";
    local config="$4";                 # colon-separated list of optional config files and/or directories to search

    # Split $config into a list of directories and (presumably) files.
    local confdirs=
    local conffiles=
    while [ -n "$config" ]; do
	local ltstr="${config%%:*}"
	if [ "$ltstr" = "" ]; then
	    : extraneous colon
	elif [ -d "$ltstr/." ]; then
	    confdirs="$confdirs:$ltstr"
	else
	    conffiles="$conffiles:$ltstr"
	fi
	config="${config#$ltstr}"
	config="${config#:}"
    done
    confdirs="$confdirs:$repo/scripts/rosegit/config"

    ROSEGIT_LOADED=

    # Load the default config, which is required
    echo -n "$myname configuring:" >&2
    rosegit_load_config_file "defaults" "required" "$confdirs" >&2;
    [ -n "$ROSEGIT_LOADED" ] || rosegit_die "default config should have set ROSEGIT_LOADED"

    # Load configurations based on branch name
    rosegit_load_config_file "$ns.conf"         "optional" "$confdirs" >&2
    rosegit_load_config_file "$ns.$branch.conf" "optional" "$confdirs" >&2

    # Load additional configurations specified via $config argument.
    while [ -n "$conffiles" ]; do
	local ltstr="${conffiles%%:*}"
	[ "$ltstr" != "" ] && rosegit_load_config_file "$ltstr" "required" "$confdirs" >&2
	conffiles="${conffiles#$ltstr}"
	conffiles="${conffiles#:}"
    done

    echo >&2

    # Export variables
    eval $(set |sed -n '/^ROSEGIT/s/^\(ROSEGIT[a-zA-Z_0-9]*\).*/export \1/p')
}

# Loads one configuration file.  Every config file that's loaded gets added to the colon-separated list
# of loaded names stored in ROSEGIT_LOADED
rosegit_load_config_file () {
    local cfile="$1";     [ -n "$cfile" ] || rosegit_die "no configuration file specified"
    local flags="$2"
    local paths="$3"
    echo -n " $(basename $cfile .conf)"

    # If $cfile contains a slash then look directly without searching.
    if [ "$cfile" != "${cfile%/}" ]; then
	if [ -f "$cfile" ]; then
	    echo -n "[ok]"
	    source "$cfile" || rosegit_die "cannot source file: $cfile"
	    ROSEGIT_LOADED="$ROSEGIT_LOADED:$cfile";
	    return 0;
	elif [ -f "$cfile.conf" ]; then
	    echo -n "[ok]"
	    source "$cfile.conf" || rosegit_die "cannot source file: $cfile.conf"
	    ROSEGIT_LOADED="$ROSEGIT_LOADED:$cfile.conf";
	    return 0;
	fi
    fi

    # If $cfile doesn't start with a slash, then search the specified paths.
    if [ "${cfile#/}" = "$cfile" ]; then
	while [ -n "$paths" ]; do
	    local ltstr="${paths%%:*}"
	    if [ "$ltstr" != "" ]; then
		local fullname="$ltstr/$cfile"
		if [ -f "$fullname" ]; then
		    echo -n "[ok]"
		    source "$fullname" || rosegit_die "cannot source file: $fullname"
		    ROSEGIT_LOADED="$ROSEGIT_LOADED:$fullname";
		    return 0
		elif [ -f "$fullname.conf" ]; then
		    echo -n "[ok]"
		    source "$fullname.conf" || rosegit_die "cannot source file: $fullname.conf"
		    ROSEGIT_LOADED="$ROSEGIT_LOADED:$fullname.conf";
		    return 0
		fi
		paths="${paths#$ltstr}"
	    fi
	    paths="${paths#:}"
	done
    fi

    # Was the file required?
    if [ "$flags" = "required" ]; then
	echo
	rosegit_die "config file not found: $cfile"
    fi
    echo -n "[no]"
    return 1

    # This variable is typically long and spans multiple lines. Change its value to a single line.
#    ROSEGIT_CONFIGURE=$(echo "$ROSEGIT_CONFIGURE" |tr '\n' ' ')
}

# Runs ROSEGIT_MAKE. Used by test scripts.
rosegit_make () {
    eval "$ROSEGIT_MAKE" "$@"
}

# Echoes the current namespace based on the setting of the ROSEGIT_NAMESPACE variable.  If this variable is empty then we
# generate a namespace from the current user name.  For most users, this is simply their login name as stored in $USER.  For
# the ROSE team staff members, this might be an abbreviation such as "dq" or "rpm".
rosegit_namespace() {
    local ns="$ROSEGIT_NAMESPACE"
    [ -n "$ns" ] || ns="$(whoami)"
    [ -n "$ns" ] || ns="$USER"
    ns=$(echo "$ns" | tr -cd 'a-z0-9_')
    case "$ns" in
	dquinlan*) ns=dq ;;
        matzke*)   ns=rpm ;;
    esac
    [ -n "$ns" ] || rosegit_die "could not determine namespace"
    ROSEGIT_NAMESPACE="$ns"
    echo "$ns"
}

# Returns true/false depending on whether the specified argument is a recognized rosegit switch
rosegit_preamble_switch () {
    case "$1" in
	--blddir=*) return 0;;
        --config=*) return 0;;
	--namespace=*) return 0;;
	--srcdir=*) return 0;
    esac
    return 1;
}

# All scripts should call this function to make sure the environment is set up properly.  You'll generally want to pass it the
# script arguments after having removed things the caller recognizes.  In the simplest case, call it as:
#    rosegit_preamble "$@"
rosegit_preamble () {
    local config srcdir blddir namespace

    # Extract arguments we recognize
    while [ "$#" -gt 0 ]; do
	case "$1" in
	    --) break;;
            --blddir=*) blddir="${1##--blddir=}"; shift;;
	    --config=*) config="$config:${1##--config=}"; shift;;
	    --namespace=*) namespace="${1##--namespace=}"; shift;;
	    --srcdir=*) srcdir="${1##--srcdir=}"; shift;;
	    -*) rosegit_die "unknown switch: $1";;
            *) rosegit_die "unknown argument: $1";;
        esac
    done
	
    # Configure rosegit if not configured already
    if [ "$ROSEGIT_LOADED" = "" ]; then
	ROSEGIT_BLD="$blddir"
	ROSEGIT_SRC="$srcdir"
	ROSEGIT_NAMESPACE="$namespace"
	rosegit_find_directories
	local branch=$(cd $ROSEGIT_SRC && git branch 2>/dev/null |sed -n '/^\*/s/^\* //p')

        # If no --config= switch, then try to use the configuration that we already saved.  Configurations are saved by
        # names of config files rather than by specific variables they define.
	local settings="$ROSEGIT_BLD/.rosegit"
	local old_config=$(sed -n '/^config=/s/config=//p' "$settings" 2>/dev/null)
	if [ -n "$config" ]; then
	    [ "$config" = ":default" ] && config=
	    [ -f "$settings" -a "$config" != "$old_config" ] && rosegit_warn "warning: possible change in configuration"
	else
	    config="$old_config"
	fi

	rosegit_load_config "$ROSE_SRC" "$(rosegit_namespace)" "$branch" "$config"
	rosegit_environment
	echo "config=$config" >"$ROSEGIT_BLD/.rosegit"

    elif [ -n "$blddir" -a "$blddir" != "$ROSEGIT_BLD" ] || \
	 [ -n "$srcdir" -a "$srcdir" != "$ROSEGIT_SRC" ] || \
	 [ -n "$namespace" -a "$srcdir" != "$ROSEGIT_NAMESPACE" ] || \
	 [ -n "$config" ]; then
	rosegit_die "you are already in a rosegit environment"
    fi

    # Prevent errors about the progress report file being closed.
    (echo -n "">&6) 2>/dev/null || exec 6>/dev/null
}

# Echos various environment settings to aid in debugging.
rosegit_show_environment () {
    echo "Date:              $(date)"
    echo "User:              $(whoami) [$(rosegit_namespace)] pid=$$"
    local hostname=$(hostname --long 2>/dev/null || hostname)
    local hostip=$(hostname --ip-address 2>/dev/null || dig $hostname |grep "^$hostname" |head -n1 |cut -f5)
    echo "Machine:           $hostname [$hostip]"
    echo "Operating system:  $(uname -s) $(uname -r)"
    echo "Architecture:      $(uname -m)"
    echo "Source tree:       $ROSEGIT_SRC"
    echo "Build tree:        $ROSEGIT_BLD"
    [ "$ROSEGIT_SRC" != "$ROSE_SRC" ] && echo "ROSE source tree:  $ROSE_SRC"
    local commit=$(git rev-parse HEAD 2>/dev/null); [ -n "$commit" ] || commit="not a Git repository"
    echo "Current HEAD:      $commit"
    echo "Software:"
    echo "    $(make --version |head -n1)"
    echo "    $(gcc --version |head -n1)"
    echo "    $(g++ --version |head -n1)"
    echo "    $(bison --version |head -n1)"
    echo "    doxygen $(doxygen --version 2>/dev/null || echo NOT INSTALLED)"
    echo "    $(dot -V 2>&1 |grep version || echo dot NOT INSTALLED)"
    echo "    libtool $((libtool --version || libtool -V) 2>/dev/null |head -n1)"
    echo "    $((tex --version || echo tex NOT INSTALLED) 2>/dev/null |head -n1)"
    echo "    $((latex --version || echo latex NOT INSTALLED) 2>/dev/null |head -n1)"
    echo "    $(swig -version |grep -i version)"
    if [ -n "$BOOST_HOME" ]; then
	echo -n "    boost (in $BOOST_HOME) "
	if [ -n "$BOOST_VERSION" ]; then
	    echo "$BOOST_VERSION"
	elif [ -f "$BOOST_HOME/include/boost/version.hpp" ]; then
	    echo $(sed -n '/#.*BOOST_LIB_VERSION/s/.*"\(.*\)"/\1/p' <"$BOOST_HOME/include/boost/version.hpp" | tr _ .)
	else
	    echo "unknown"
        fi
    else
	echo "    boost: not in /usr/include (see configure output for version)"
    fi

    echo "PATH:"
    path-adjust --list |sed 's/^/    /'

    echo "LD_LIBRARY_PATH:"
    path-adjust --var=LD_LIBRARY_PATH --list |sed 's/^/    /'

    echo "Configured from:"
    local x="$ROSEGIT_LOADED"
    while [ -n "$x" ]; do
	local ltstr="${x%%:*}"
	[ -n "$ltstr" ] && echo "    $ltstr"
	x="${x#$ltstr}"
	x="${x#:}"
    done

    echo "Configuration:"
    eval "perl -e 'print qq{    \$_\n} for sort {(split q{=},\$a)[0] cmp (split q{=},\$b)[0]} @ARGV' -- $ROSEGIT_CONFIGURE"
}
