#!/bin/bash

# This script reads ROSE's version string from the first line of the $ROSE_SOURCE/ROSE_VERSION file and increments it. It
# expects that the version string ends follows the pattern "<ANYTHING><N><ANYTHING>" where the first ANYTHING is at least
# one character long, is as long as possible, and doesn't end with a digit. The following example version numbers
# match this pattern:
#    1.2.3                         becomes 1.2.4
#    0.9.6a      		   becomes 0.9.7a
#    0.9.7-rc1   		   becomes 0.9.7-rc2
#    2016.03.21.123-test           becomes 2016.03.21.124-test
#
# Usage:
#   version-increment.sh
#   version-increment.sh NAME_OF_THE_VERSION_FILE
#       The first form (no argument) must be run when the current working directory is the top of the ROSE source tree.
#       The version file must already exist.

version_file="$1"
[ "$version_file" = "" ] && version_file="ROSE_VERSION"

# Read the version number from the file and split it into the three parts: <ANYTHING><N><ANYTHING>
version_parts=($(sed -ne '1,1 s/^\(.*[^0-9]\)\([0-9][0-9]*\)\(.*\)/\1 \2 \3/p' "$version_file"))
if [ "${version_parts[1]}" = "" ]; then
    echo "$0: cannot parse version number from $version_file" >&2
    exit 1
fi

# Increment the N
version_old_n=${version_parts[1]}
version_new_n=$[version_old_n + 1]
version_new_string="${version_parts[0]}${version_new_n}${version_parts[2]}"

# Update the VERSION file
sed -i~ -e '1,1 s/^.*$/'"$version_new_string"'/' "$version_file"
