# This file is sourced by many of the rosegit scripts. It contains shell (Bash) functions.
# Functions are alphabetical. They use underscores in their names while shell scripts use hypens.

# Set up directory where calling script is running, and the base name of that script.
# Note: the extra checks here are to do something reasonable if source directly from the shell PS1 level commandline.
mydir=${0%/*}; mydir=$(cd -- $mydir 2>/dev/null && pwd); [ -n "$mydir" ] || mydir="/bin"
myname="${0##*/}"; [ "$myname" = "-bash" ] && myname="rosegit-functions.sh"

# Returns the name of a branch that contains the specified commit.  If the commit is on more than one branch then return the
# best one, where "best" is the branch with fewest commits between the specified commit and the branch head. Ties are broken
# arbitrarily.
rosegit_branch_of () {
    local repo="$1";   [ -d "$repo" ]      || rosegit_die "not a repository: $repo"
    local commit="$2"; [ -n "$commit" ]    || rosegit_die "no committish"

    local head= found= score=none
    commit=$(cd $repo && git rev-parse $commit)
    for head in $(cd $repo/.git/refs/heads && echo *); do
	local sha1=$(cd $repo && git rev-parse $head)
	local this_score=$(cd $repo && git rev-list $head ^$commit |wc -l)
	if [ "$commit" = "$sha1" ]; then
	    echo $head # Perfect match
	    return 0
	elif [ $this_score -eq 0 ]; then
	    : commit is not reachable from this head
	elif [ "$score" = "none" ]; then
	    score=$this_score
	    found=$head
        elif [ $this_score -lt $score ]; then
	    score=$this_score
	    found=$head
        fi
    done
    echo $found
}

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
    type path-adjust >/dev/null 2>&1 && eval $(path-adjust --var=LD_LIBRARY_PATH remove --regexp /ROSE/ /boost_)
    LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$BOOST_ROOT/lib"
    LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$ROSE_BLD/src/.libs"
    LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$ROSE_BLD/3rdPartyLibraries/libharu-2.1.0/src/.libs"
    export LD_LIBRARY_PATH
}

# Finds the part of the "make" output where an error occurred. It does this by looking for GNU makes "Entering directory" and
# "Leaving directory" messages and the error messages from make.  Unfortunately, it assumes that make's "-j" was not used since
# doing so can cause lines to be output in strange orders. This function reads standard input.
rosegit_filter_make_error () {
    sed '
        /^make.*: Entering directory/ {
            x
            s/.*//
            x
        }
        /^make.*: Entering directory/,/^make.*: Leaving directory/ H
        /^make.*: Leaving directory/ {
            x
            /\nmake.*: \*\*\*/ {
                i\
============================================================================================= \
=== Error detected in this section of the output                                          === \
=============================================================================================
                p
            }
        }
        d
'
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
    local dir1="$1"
    [ -n "$dir1" ] || dir1=.
    [ -d "$dir1" ] || rosegit_die "not a directory: $dir1"
    dir1=$(cd $dir1 && pwd)

    local cwdbase=$(basename $dir1)

    while [ "$dir1" != "/" ]; do
	if [ -d $dir1/.git -a -e $dir1/src/rose.h ]; then
	    echo $dir1
	    return 0
        fi

	dir2=$dir1/sources/$cwdbase
	if [ -d $dir2/.git -a -e $dir2/src/rose.h ]; then
	    echo $dir2
	    return 0
	fi

	dir1=$(cd $dir1/..; pwd)
    done
}

# Returns the commit having the most recent tag matching the specified pattern for the specified branch. If no commit within
# the last 20 has that tag then the 20th oldest commit on the branch is returned.
rosegit_latest_tag () {
    local repo="$1";   [ -d "$repo" ]      || rosegit_die "not a repository: $repo"
    local match="$2";  [ -n "$namespace" ] || match='*'
    local branch="$3"; [ -n "$branch" ]    || rosegit_die "no branch name supplied"

    local firstref=$(cd $repo && git describe --tags --long --candidates=20 --match "$match" $branch 2>/dev/null)
    [ -n "$firstref" ] || firstref=$(cd $repo && git rev-parse $branch~20 2>/dev/null)
    [ -n "$firstref" ] || firstref=$(cd $repo && git rev-parse $branch~10 2>/dev/null)
    [ -n "$firstref" ] || firstref=$(cd $repo && git rev-parse $branch~5  2>/dev/null)
    [ -n "$firstref" ] || firstref=$(cd $repo && git rev-parse $branch^   2>/dev/null)

    case "$firstref" in
	*-*) firstref=$(echo $firstref |perl -pe 's/-[^-]+-[^-]+$//') ;;
    esac
    echo $firstref
}

# Loads configuration files from the current branch of the specified repository. We use the current branch so as not to mess
# with what might already be checked out in that repo (and we need to read the config before we clone). All variables starting
# with "ROSEGIT" are exported; others are only exported if done so explicitly in the configuration files.
rosegit_load_config () {
    local repo="$1";   [ -d "$repo" ]      || rosegit_die "not a repository: $repo"
    local ns="$2";     [ -n "$ns" ]        || ns=$(rosegit_namespace)
    local branch="$3"; [ -n "$branch" ]    || rosegit_die "no branch name supplied"
    local config="$4";                     # optional file or directory

    local branch_ns=$(echo $branch |cut -d- -f1)
    local confdir=$repo/scripts/rosegit/config
    config=$(eval "echo $config")          # expand tidle, etc.
    if [ -n "$config" ]; then
	if [ -d $config/. ]; then
	    confdir=$config
	    config=
	fi
    fi

    echo -n "$myname configuring:" >&2

    # The defaults.conf must be present (it may be empty). This is a sanity check!
    ROSEGIT_LOADED=
    local defaults=$confdir/defaults.conf
    [ -r $defaults ] || rosegit_die "no configuration file: $defaults"
    rosegit_load_config_file $defaults >&2
    [ -n "$ROSEGIT_LOADED" ] || rosegit_die "$defaults should have set ROSEGIT_LOADED"

    # Load other config files. These are just shell scripts--the later, more specific files can override what the earlier ones did.
    rosegit_load_config_file $confdir/$ns.conf >&2
    rosegit_load_config_file $confdir/$ns.$branch_ns.conf >&2
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

# Sends an e-mail message using configuration variables ROSEGIT_MAIL_X_* where X is specified as the first argument. The second
# argument should be the name of a file that contains the message body.
rosegit_mail () {
    local mf="$1";   # mail facility
    local subj="$2";
    local file="$3";  [ -f "$file" ] || rosegit_die "no email body file: $file"
    local x=$(rosegit_mail_var $mf)
    [ -z "$x" -o "$x" = "no" -o "$x" = "false" ] && return 0
    local to="$(rosegit_mail_var $mf TO)"
    #rosegit_checkargs "$to"; exit 0
    (echo "Subject: $subj"; echo "To: $to"; echo; fold $file) |sendmail "$to"
}

# Helper that returns the value of an email variable
rosegit_mail_var () {
    local facility="$1"
    local var="$2"
    local v="\$ROSEGIT_MAIL"
    [ -n "$facility" ] && v="${v}_$facility"
    [ -n "$var"      ] && v="${v}_$var"
    eval "local ret=\"$v\""
    if [ -z "$facility" -o -n "$ret" ]; then
	echo "$ret"
    else
	v="\$ROSEGIT_MAIL_$var"
	eval "local ret=\"$v\""
	echo "$ret"
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

# Returns the next available serial number for the given tag. In order to make tags unique, they have three parts separated by
# hyphens (the middle part may have additional hyphens).  For example, the tag "rpm-new-feature-123" has a name space "rpm" which
# is typically the user's intials, a local name "new-feature", and a serial number "123".  The name space and local name together
# are called the base name.
rosegit_next_serial () {
    local repo="$1";    [ -d "$repo" ]    || rosegit_die "not a repository: $repo"
    local tagbase="$2"; [ -n "$tagbase" ] || rosegit_die "no tag basename specified"
    local lastused=$(cd $repo && git tag -l "$tagbase-*" |perl -F- -ane 'print $F[-1]' |sort -nr |head -n1)
    echo $((lastused+1))
}

# Returns the name of the ROSE git repository. This comes from the ROSEGIT_REPOSITORY variable, which the caller should set
# explicitly if they use a command-line specified repository.
rosegit_repository () {
    [ -n "$ROSEGIT_REPOSITORY" ] || rosegit_die "Could not determine repository. Please set ROSEGIT_REPOSITORY config variable."
    echo "$ROSEGIT_REPOSITORY"
}

# Runs a shell command and spits out its title on file 6 (so command output can be independently redirected) and an indication
# of whether the command succeeded or failed, and how long it ran.
rosegit_run () {
    echo -n "+ "; rosegit_checkargs "$@"
    local name="$1"; shift         # first arg is name, the rest are the command
    local start_time=$(date +%s)
    echo -n "$name..." >&6
    eval "$@"; local status=$?
    local end_time=$(date +%s)
    local elapsed=$(rosegit_elapsed_human $((end_time - start_time)))
    [ $status -eq 0 ] && echo "OK ($elapsed)" >&6
    [ $status -ne 0 ] && echo "FAIL ($elapsed)" >&6
    return $status
}

# Echos various environment settings to aid in debugging.
rosegit_show_environment () {
    echo "Date:              $(date)"
    echo "User:              $(whoami) [$(rosegit_namespace)] pid=$$"
    echo "Machine:           $(hostname --long) [$(hostname --ip-address)]"
    echo "Operating system:  $(uname -s) $(uname -r) $(uname -v)"
    echo "Architecture:      $(uname -m) $(uname -i) $(uname -p)"
    echo "Repository:        $(rosegit_repository)"
    echo "Tester script:     $(rosegit_tester)"
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
    echo
    echo "Working directory $(rosegit_workdir)"
    df -h $(rosegit_workdir)
    echo
    echo "Uptime $(uptime)"
    echo
    echo "Memory usage (MB):"
    free -tom
}

# Returns all tags that are defined for the specified commit
rosegit_tags_of () {
    local repo="$1";    [ -d "$repo" ]    || rosegit_die "not a repository: $repo"
    local commit="$2";  [ -n "$commit" ]  || rosegit_die "no committish for rosegit_tags_of"
    local glob="$3";    [ -n "$glob" ]    || glob="*"

    local tag= tags= commit=$(cd $repo && git rev-parse $commit)
    for tag in "" $(cd $repo && git tag -l "$glob"); do
	[ -n "$tag" ] || continue
	[ "$commit" = $(cd $repo && git rev-parse "$tag") ] && tags="$tags $tag"
    done
    echo $tags
}

# Returns the name of the test script. This could be a file name or a whole shell command.
rosegit_tester () {
    [ -n "$ROSEGIT_TESTER" ] || rosegit_die "no test configured in ROSEGIT_TESTER"
    echo "$ROSEGIT_TESTER"
}

# All tester scripts should call this function first.  It enables a test script to be called from rosegit-ats or by a user. When
# called by a user it loads configuration files just like rosegit-ats would have done, and it can be called from any directory
# of the build tree.
rosegit_tester_preamble () {
    local config="$1";      # optional configuration file or directory
    if [ ! -n "$ROSEGIT_LOADED" ]; then
	local blddir=$ROSE_BLD; [ -n "$blddir" ] || blddir=$(rosegit_find_builddir)
	local srcdir=$ROSE_SRC; [ -n "$srcdir" ] || srcdir=$(rosegit_find_sources $blddir)
	[ -d "$srcdir" ] || rosegit_die "could not find source directory"
	local branch=$(cd $srcdir && git branch |sed -n '/^\*/s/^\* //p')
	rosegit_load_config $srcdir $(rosegit_namespace) $branch $config
    fi

    rosegit_environment
    export ROSEGIT_REPO="$ROSE_SRC"

    # Prevent errors about the progress report file being closed.
    (echo -n "">&6) 2>/dev/null || exec 6>/dev/null
}

# Returns the name of the scratch directory, caching it in $ROSEGIT_WORKDIR and creating it if necessary.
rosegit_workdir () {
    [ -n "$ROSEGIT_WORKDIR" ] || ROSEGIT_WORKDIR=/tmp/$(whoami)
    mkdir -p $ROSEGIT_WORKDIR || rosegit_die "cannot create scratch directory: $ROSEGIT_WORKDIR"
    echo $ROSEGIT_WORKDIR
}