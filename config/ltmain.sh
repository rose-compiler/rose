#!/bin/sh
if test -f /usr/share/libtool/ltmain.sh; then
  # Try to use a system ltmain.sh if possible
  exec /bin/sh /usr/share/libtool/ltmain.sh "$@"
fi
if test `uname` = Linux; then
  if test `uname -m` = x86_64; then
    # We need a Red Hat-modified version to get multilibs right on x86-64
    exec /bin/sh `dirname $0`/ltmain.sh.linux "$@"
  fi
fi
# This is a newer version that should be preferred
exec /bin/sh `dirname $0`/ltmain.sh.mac "$@"
