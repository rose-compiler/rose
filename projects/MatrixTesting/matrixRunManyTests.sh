#!/bin/bash

# Prompt and read response in a way that works regardless of whether stdin is redirected.  The
# "read" command's interaction of "-i" and "-e" switches behave differently depending on whether
# input is from a terminal.
query() {
        local prompt="$1"
        local init_value="$2"
        if [ "$(tty)" = "not a tty" ]; then
                echo -n "$prompt" >&2
                read REPLY
                echo "$REPLY" >&2
                if [ "$REPLY" = "" ]; then
                        echo "$init_value"
                else
                        echo "$REPLY"
                fi
        else
                read -p "$prompt" -i "$init_value" -e REPLY
                echo "$REPLY"
        fi
}

expect_yes() {
    local prompt="$1"
    read -p "$prompt [Y/n] "
    case "$REPLY" in
        y|yes|Y|YES|Yes|"")
            return 0
            ;;
    esac
    return 1
}

die() {
    echo "$@" >&2
    exit 1
}

override() {
    local prompt="$1"
    local varname="$2"
    local rmc_list_item="$3"

    local dflt=$(eval echo '$'$varname)
    local result=
    while true; do
        local p=$(printf "  %-29s %-24s: " "$prompt" "($varname)")
        result=$(query "$p" "$dflt")
        if [ "$result" = "list" ]; then
            if [ "$rmc_list_item" = "" ]; then
                echo "cannot query that" >&2
            else
                dflt=$(rmc list --terse "$rmc_list_item")
            fi
        else
            break
        fi
    done

    eval "$varname='$result'"
}

section() {
    local title="$1"
    echo
    echo "===================================== $title ====================================="
    echo
}

echo "This script configures and runs the ROSE configuration matrix testing. You can run this script concurrently"
echo "in as many terminal windows as you like and they can all share the same directories.  Each of the questions"
echo "below have a corresponding environment variable which you can set prior to invoking this script. In fact,"
echo "you don't really need this script at all if you'd rather just invoke 'matrixRunOneTest.sh' directly. Both"
echo "scripts provide default values for all variables (although they might not be the same defaults)."



########################################################################################################################
section "Directories"
########################################################################################################################

echo
echo "The directory you provide for this question is only used to construct the default directory names for some"
echo "subsequent questions."
echo
: ${MATRIX_ROOT:="$HOME/matrix-testing"}
MATRIX_ROOT=$(query "Directory to hold all matrix testing files (MATRIX_ROOT): " "$MATRIX_ROOT")
mkdir -p "$MATRIX_ROOT" || exit 1
export MATRIX_ROOT

echo
echo "This directory is where tests will build ROSE. Each test will create a temporary subdirectory here so it's"
echo "safe for you to specify a directory that's also used by other instances of this script. The directory and"
echo "its parents need not exist yet.  Tests will perform best if this is a local directory. When testing is"
echo "finished you can delete this directory."
echo
: ${WORKSPACE:="$MATRIX_ROOT/tmp"}
WORKSPACE=$(query "Temporary matrix testing workspace (WORKSPACE): " "$WORKSPACE")
mkdir -p "$WORKSPACE" || exit 1
export WORKSPACE

echo
echo "This is the local ROSE source repository that will be used for testing. Since none of the tests write to"
echo "this directory it's safe for many tests to use the same ROSE source tree."
echo
: ${ROSE_SRC:="$HOME/GS-CAD/ROSE/matrix/source-repo"}
ROSE_SRC=$(query "Location of quiescent ROSE source tree (ROSE_SRC): " "$ROSE_SRC")
[ -e "$ROSE_SRC" ] || die "ROSE source tree must exist: $ROSE_SRC"
export ROSE_SRC

# We can't run "build" from this script because we've promised not to modify the ROSE build tree. There might be
# other scripts already running matrix tests and us running "build" would screw them up!
expect_yes "Is the ROSE source repo up-to-date?" || exit 1
expect_yes "Do you promise not to edit/change the repo while tests are running?" || exit 1
expect_yes "Have you run 'build' in the repo already?" || exit 1

########################################################################################################################
section "Database"
########################################################################################################################

echo
echo "Please enter the name of the database where results are stored. Leave this blank if the testing"
echo "machine should not use a database. If no database is used, then the configuration space must be"
echo "obtained by alternate means and the results will be emailed to the database (later questions)."
echo
: ${DATABASE:="postgresql://rose:fcdc7b4207660a1372d0cd5491ad856e@www.hoosierfocus.com/rose_matrix"}
[ "$DATABASE" = "none" ] && DATABASE=
DATABASE=$(query "Database URL: " "$DATABASE")
export DATABASE


# Definition of configuration space
if [ "$DATABASE" = "" ]; then
    echo
    echo "The testing normally communicates with a database to obtain a point in the configuration space"
    echo "that it should test, but you just said not to contact any database. Therefore the configuration"
    echo "space must be defined in a file which is usually created with 'matrixNextTest --format=overrides'"
    echo "on a machine that can connect to the database."
    echo
    : ${CONFIGURATION_SPACE_FILE:="$MATRIX_ROOT/configurationSpace.txt"}
    while true; do
        CONFIGURATION_SPACE_FILE=$(query "Configuration space file (CONFIGURATION_SPACE_FILE): " "$CONFIGURATION_SPACE_FILE")
        if [ "$CONFIGURATION_SPACE_FILE" = "" ]; then
            CONFIGURATION_SPACE_URL="$DATABASE"
        elif [ -r "$CONFIGURATION_SPACE_FILE" ]; then
            CONFIGURATION_SPACE_URL="file://$CONFIGURATION_SPACE_FILE"
            break
        fi
        echo "error: file does not exist or is not readable" >&2
    done
else
    : ${CONFIGURATION_SPACE_URL:="$DATABASE"}
fi
export CONFIGURATION_SPACE_URL

# Where to send results
echo
echo "Results are normally sent directly to the database, but if you don't have write permission or"
echo "lack an internet connection you can mail them somewhere instead.  If you want to mail them, enter"
echo "an email address here (do not include any 'mailto:' prefix)."
echo
: ${RESULTS_URL:="$DATABASE"}
while true; do
    RESULTS_EMAIL=$(query "Email address for results: " "$RESULTS_EMAIL")
    if [ "$RESULTS_EMAIL" = "" -a "$DATABASE" = "" ]; then
        echo "error: I need an email address since there's no database" >&2
    elif [ "$RESULTS_EMAIL" != "" ]; then
        RESULTS_URL="mailto:$RESULTS_EMAIL"
        break
    else
        break
    fi
done
export RESULTS_URL

########################################################################################################################
section "Matrix Tools"
########################################################################################################################

echo
echo "Testing requires that the tools in the ROSE projects/MatrixTesting directory are built. You can"
echo "do that by running 'make install-rose-library && make -C projects/MatrixTesting install'. The"
echo "value you specify here should be the ROSE prefix directory--the directory to which ROSE was installed"
echo "and which contains a 'bin' subdirectory."
: ${ROSE_TOOLS:="$HOME/GS-CAD/ROSE/matrix/tools-build"}
ROSE_TOOLS=$(query "Location of build tree for ROSE matrix tools (ROSE_TOOLS): " "$ROSE_TOOLS")
if [ ! -e "$ROSE_TOOLS/bin/matrixTestResult" ]; then
    echo "You must build and install the ROSE library and the projects/MatrixTesting directories before"
    echo "you can start any matrix tests."
    exit 1
fi

########################################################################################################################
section "Config Space Overrides"
########################################################################################################################

echo
echo "The matrix testing selects configurations at random by querying the database. Since the database doesn't"
echo "know what's installed on our system, many of the configurations returned from the database will be invalid"
echo "here and will be kicked out without even testing.  If our valid configuration space is a tiny fraction of"
echo "the configuration space known by the database, then we'll spend most of our time requesting and kicking out"
echo "configurations. Therefore, the following overrides can be used to ignore certain things from the database"
echo "and choose them ourself. Their values are the same as what's accepted by the corresponding 'rmc_*' directive"
echo "in RMC configuration files. For example, the database knows about many compilers but we maybe have only one"
echo "installed here, so we would set the compiler override to the string 'gcc-4.8-default gcc-4.8-c++11', which"
echo "means no matter what compiler the database tells us to use, use one of these two. (RMC specifies compilers"
echo "as a triplet: VENDOR-VERSION-LANGUAGE). Typing the word 'list' will query RMC for a list of versions installed"
echo "on this system."

echo
if ! expect_yes "Should I skip initializing overrides from the configuration space?"; then
    eval $(rmc -C "$ROSE_TOOLS" ./matrixNextTest --database="$CONFIGURATION_SPACE_URL" --format=overrides)
fi

while true; do
    override "  Override build system"       OVERRIDE_BUILD
    override "  Override frontend languages" OVERRIDE_LANGUAGES
    override "  Override compiler"           OVERRIDE_COMPILER      compiler
    override "  Override debug mode"         OVERRIDE_DEBUG
    override "  Override optimize mode"      OVERRIDE_OPTIMIZE
    override "  Override warnings mode"      OVERRIDE_WARNINGS
    override "  Override code_coverage"      OVERRIDE_CODE_COVERAGE
    override "  Override parallelism"        OVERRIDE_PARALLELISM
    echo
    override "  Override assertions mode"    OVERRIDE_ASSERTIONS
    override "  Override boost versions"     OVERRIDE_BOOST         boost
    override "  Override cmake versions"     OVERRIDE_CMAKE         cmake
    override "  Override dlib versions"      OVERRIDE_DLIB          dlib
    override "  Override doxygen versions"   OVERRIDE_DOXYGEN       doxygen
    override "  Override dwarf versions"     OVERRIDE_DWARF         dwarf
    override "  Override edg versions"       OVERRIDE_EDG
    override "  Override java versions"      OVERRIDE_JAVA          java
    override "  Override magic versions"     OVERRIDE_MAGIC         magic
    override "  Override python versions"    OVERRIDE_PYTHON        python
    override "  Override qt versions"        OVERRIDE_QT            qt
    override "  Override readline versions"  OVERRIDE_READLINE      readlin
    override "  Override sqlite versions"    OVERRIDE_SQLITE        sqlite
    override "  Override wt versions"        OVERRIDE_WT            wt
    override "  Override yaml versions"      OVERRIDE_YAML          yaml
    override "  Override yices versions"     OVERRIDE_YICES         yices
    echo
    expect_yes "Are you satisfied with these overrides?" && break
done

export OVERRIDE_BUILD OVERRIDE_LANGUAGES OVERRIDE_COMPILER OVERRIDE_DEBUG
export OVERRIDE_OPTIMIZE OVERRIDE_WARNINGS OVERRIDE_CODE_COVERAGE OVERRIDE_PARALLELISM
export OVERRIDE_ASSERTIONS OVERRIDE_BOOST OVERRIDE_CMAKE OVERRIDE_DLIB
export OVERRIDE_DOXYGEN OVERRIDE_DWARF OVERRIDE_EDG OVERRIDE_JAVA OVERRIDE_MAGIC OVERRIDE_PYTHON
export OVERRIDE_QT OVERRIDE_READLINE OVERRIDE_SQLITE OVERRIDE_WT
export OVERRIDE_YAML OVERRIDE_YICES

########################################################################################################################
section "Run Tests"
########################################################################################################################

echo
echo "Type 'stop' and Enter at any time to stop testing at the next break."
echo "Or type C-c a couple times to interrupt in the middle of a test."
echo
expect_yes "Shall I start running tests? " || exit 0

testNumber=0
while true; do
    testNumber=$[testNumber+1]
    (figlet "Test $testNumber" || banner "Test $testNumber" || (echo; echo "Test $testNumber"; echo)) 2>/dev/null
    matrixRunOneTest.sh
    read -t 1                   # use 1 second so Ctrl-C works over a slow link if a bug causes us to spew
    [ "$REPLY" = "stop" ] && break
done
