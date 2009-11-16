#!/bin/bash
# Machine-specific setup invoked by tests before configure is called.
#
# Eventually we'll need a better way of doing this, but for now each test just sources this file.
#
#
# Basically all we do is look for packages that might be installed in non-standard places. When we find them
# we'll set shell variables used in the test scripts when launching "configure", and any other environment
# settings that we need.


# Look for BOOST in non-standard places
if [ -z "$BOOST" ]; then
    if [ -d $HOME/GS-CAD/boost_1_35_0/installed ]; then
	BOOST_ROOT=$HOME/GS-CAD/boost_1_35_0/installed
	eval $(path-adjust --var=LD_LIBRARY_PATH --prepend $BOOST_ROOT/lib)
    elif [ -d /usr/include/boost ]; then
	BOOST_ROOT=/usr
    fi
fi

# Look for DWARF in non-standard places
if [ -z $DWARF_ROOT ]; then
    if [ -d $HOME/GS-CAD/dwarf-20081013/libdwarf ]; then
	DWARF_ROOT=$HOME/GS-CAD/dwarf-20081013/libdwarf
    fi
fi
