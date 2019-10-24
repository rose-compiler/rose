#!/bin/bash

#-------------------------------------------------------------------------------
#  Default global variables
#-------------------------------------------------------------------------------
: ${OS_MACOSX:=false}
: ${LIBRARY:=}

#-------------------------------------------------------------------------------
#  Utilities
#-------------------------------------------------------------------------------
function usage() {
  echo "Usage: $0 <path-to-library:string>"
}

function how_to_fix() {
    echo "  If this is an issue with only the Boost libraries (and not the Boost include files) and"
    echo "  you're using dynamic linking, then you should be able to fix this simply by adjusting"
    echo "  environment variables; otherwise you may need to reconfigure, \"make clean\" and re-make ROSE."
}

#-------------------------------------------------------------------------------
#  Main
#-------------------------------------------------------------------------------

if test $# -lt 1 -o $# -gt 2; then
    usage
    exit 1
else
    LIBRARY="$1"
    if [ -n "$(type ldd 2>/dev/null)" ]; then
        OS_MACOSX=false
    else
        if [ -n "$(type otool 2>/dev/null)" ]; then
            OS_MACOSX=true
        else
            echo "[ERROR] [${0}] ldd/otool does not exist. Can't determine Boost dependencies in '${LIBRARY}'."
            exit 1
        fi
    fi
fi

#-------------------------------------------------------------------------------
#  Check linkage of [Boost C++ library]
#-------------------------------------------------------------------------------
if $OS_MACOSX; then
    cmd="otool -L \"$LIBRARY\" | grep libboost | sed -n 's/\(.*\)libboost_.*/\1/p' | sort | uniq"
else
    cmd="ldd \"$LIBRARY\" | grep libboost | sed 's/.* => \(.*\)\/libboost_.* (.*)/\1/' | sort | uniq"
fi

boostlibdirs="$(eval $cmd)"

if test "$boostlibdirs" = ""; then
    : no dynamic boost linkage at all
elif test -n "$boostlibdirs"; then
    : echo "$0: info: Boost is being linked from '$(echo "$boostlibdirs" | xargs)' in '$LIBRARY'"
else
    echo "$0: error: Boost link dependencies not found in '${LIBRARY}'."
    echo "  It looks like the directory you specified during ROSE's configure step for Boost libraries"
    echo "  is not being used in the ROSE test executable we just compiled.  Perhaps some other Boost"
    echo "  is being picked up instead?"
    how_to_fix
    echo
    echo "  \$ $cmd"
    echo
    exit 1
fi

if test $(echo "$boostlibdirs" | wc -l) -gt 1; then
    echo "$0: error: Boost libraries are being linked from multiple directories in '${LIBRARY}'."
    echo "  This is probably an error; it's not likely that you would have libraries from one version"
    echo "  of Boost spread into multiple directories.  Perhaps one of the installed Boost versions is"
    echo "  not complete?"
    how_to_fix
    echo
    echo "  \$ $cmd"
    if $OS_MACOSX; then
          otool -L "$LIBRARY" | grep libboost
    else
          ldd "$LIBRARY" | sed -n --posix '/libboost/p'
    fi
    echo
fi

