#!/bin/bash

# Get command line arguments
if [[ $# -lt 1 ]]; then
    echo "Usage: AmrexF2Cxx.sh <args> <fortranfile>"
    exit 1
fi

# Get the current directory.
# http://unix.stackexchange.com/questions/188182/how-to-get-current-working-directory
F2CXXROOT="$( cd "$(dirname "$0")" ; pwd -P )"

# Location of the file to convert to C++
FORTRANFILEFULL=${*: -1}

# What the converted file will be called without the extension
FORTRANFILE=$(basename "${FORTRANFILEFULL}")

# The C++ file
CXXFILE="$FORTRANFILE".cc

# Create a dummy C++ file
cp "$F2CXXROOT/support/blueprint/empty_cxx_amrex.cc" "$CXXFILE"

# Converter Command
CMD="$F2CXXROOT/f2cxx -I $F2CXXROOT/support/include $CXXFILE $@"

# Log and execute converter command
echo "$CMD"
eval $CMD

# Get exit status of running AmrexF2Cxx.sh
if [[ $? -ne 0 ]]; then
    echo "Error running AmrexF2Cxx.sh on file ${filePath}"
    exit 1
fi
