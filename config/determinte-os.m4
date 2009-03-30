##### http://autoconf-archive.cryp.to/ax_lib_sqlite3.html
#
# SYNOPSIS
#
#   DETERMINE_OS
#
# DESCRIPTION
#
#   Determine which OS the build is configured in.
#
#   This macro sets:
#
#     OS_LINUX
#

AC_DEFUN([DETERMINE_OS],
    [

    case $build_os in
    linux*)
      LINUX=yes;
  ;;
  cygwin)
  MINGW=yes
  ;;
  mingw*)
  MINGW32=yes
  ;;
  darwin*)
  MACOSX=yes
  ;;
  msdos*)
  MSDOS=yes
  ;;
  esac
    AM_CONDITIONAL([OS_MACOSX],[ test "x$MACOSX" = xyes ] )
    AM_CONDITIONAL([OS_LINUX], [ test "x$LINUX"  = xyes ] )
    AM_CONDITIONAL([OS_MINGW], [ test "x$MINGW"  = xyes ] )
    AM_CONDITIONAL([OS_MSDOS], [ test "x$MSDOS"  = xyes ] )

  ])
