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

if test -n "$boostlibdirs"; then
    echo "[INFO] [${0}] Boost is being linked from '$(echo "$boostlibdirs" | xargs)' in '$LIBRARY'"
else
    echo "[ERROR] [${0}] Boost link dependencies not found in '${LIBRARY}'."
    echo
    echo "  \$ $cmd"
    echo
    exit 1
fi

if test $(echo "$boostlibdirs" | wc -l) -gt 1; then
    echo "[ERROR] [${0}] Boost libraries are being linked from multiple directories in '${LIBRARY}'. This is probably an error:";
    echo
    echo "  \$ $cmd"
    if $OS_MACOSX; then
          otool -L "$LIBRARY" | grep libboost
    else
          ldd "$LIBRARY" | sed -n --posix '/libboost/p'
    fi
    echo
fi

