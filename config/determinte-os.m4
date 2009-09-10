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


# /etc/redhat-release
# /etc/debian_version
# /etc/SuSE-release
# /etc/slackware-version
# /etc/gentoo-release
# You could do 'cat /etc/*-release' or 'cat /etc/*-version'.

# DQ (9/10/2009): This is a macro I wrote to nail down the OS vendor so that
# I could skip specific tests in ROSE based on the OS vendor as part of the
# NMI testing of ROSE on the Compile Farm.
AC_DEFUN([DETERMINE_OS_VENDOR],
[
   echo "Try to identify the OS vendor...";
   AC_CHECK_TOOL(ROSE_LSB_RELEASE, [lsb_release], [no])
   if test -z "$ROSE_LSB_RELEASE"; then
      echo "********************************";
      echo "* lsb_release is NOT available *";
      echo "********************************";
    # Most OS's output there name buried in /etc/issue
      ls -dl /etc/*-release /etc/*-version;
      echo "***************************";
      echo "* Output /etc/issue file: *";
      cat /etc/issue
      echo "***************************";
#     exit 1
   else
      echo "lsb_release IS available ROSE_LSB_RELEASE = $ROSE_LSB_RELEASE";
      vendor=`lsb_release -is`

      echo "vendor = $vendor"

      case $vendor in
         Debian*)
            DEBIAN=yes;
            ;;
         RedHat*)
            REDHAT=yes
            ;;
         Ubuntu*)
            UBUNTU=yes
            ;;
         CentOS*)
            CENTOS=yes
            ;;
         esac
            AM_CONDITIONAL([OS_VENDOR_DEBIAN],[ test "x$DEBIAN" = xyes ] )
            AM_CONDITIONAL([OS_VENDOR_REDHAT],[ test "x$REDHAT" = xyes ] )
            AM_CONDITIONAL([OS_VENDOR_UBUNTU],[ test "x$UBUNTU" = xyes ] )
            AM_CONDITIONAL([OS_VENDOR_CENTOS],[ test "x$CENTOS" = xyes ] )
   fi

  ])

