#!/bin/bash
# Update ROSE with latest tup-scripts release
#
# WARNING: This overwrites local changes to these scripts. You've
#          been pushing your local tup-scripts changes upstream, right?
set -e

REMOTE_REPO=${1-https://github.com/matzke1/tup-scripts}

LOCAL_REPO=tup-scripts-$(date +%Y%m%d%H%M%S)-$RANDOM
git clone "$REMOTE_REPO" "$LOCAL_REPO"
trap "rm -rf '$LOCAL_REPO'" EXIT

(cd "$LOCAL_REPO" && git archive --format=tar HEAD) |tar xvf -
(cd "$LOCAL_REPO" && git rev-parse HEAD) >VERSION
