#!/bin/bash

# This shell script logic is isolated into a separate shell script because it is a problem to call it from m4 files in autoconf.

ver=$(c++ --version | sed -n 's/.*version \([0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\).*/\1/p' | head -1)

major=${ver%%.*}
rest=${ver#*.}
minor=${rest%%.*}
patch=${ver##*.}

echo $patch
