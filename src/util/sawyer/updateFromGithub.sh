#!/bin/bash
# Update ROSE with the latest Sawyer source code
set -e

SAWYER_REPO=github.com:matzke1/sawyer

# Download the latest version of the source code
SAWYER_ROOT="sawyer-$(date +%Y%m%d)"
if [ -d "$SAWYER_ROOT" ]; then
    (
	cd "$SAWYER_ROOT"
	git fetch "$SAWYER_REPO" master
	git merge FETCH_HEAD
    )
else
    git clone "$SAWYER_REPO" "$SAWYER_ROOT"
fi
	
# Copy some of Sawyer's source files into the ROSE source tree. Don't worry about overriting ROSE-local changes--they
# should have been contributed back to the Sawyer project by now!
for f in Assert Message ProgressBar Sawyer; do
    cp "$SAWYER_ROOT/src/$f.h" "$SAWYER_ROOT/src/$f.C" .
done
