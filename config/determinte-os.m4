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

  # AC_DEFINE([ROSE_BUILD_OS], $build_os , [Operating System (OS) being used to build ROSE])
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

   OS_vendor="ROSE_unknown_OS";
   OS_release="ROSE_unknown_OS_release";

#  if test -z "$ROSE_LSB_RELEASE"; then
   if test "x$ROSE_LSB_RELEASE" = xno; then
      echo "********************************";
      echo "* lsb_release is NOT available *";
      echo "********************************";
    # Most OS's output their name buried in /etc/issue
      ls -dl /etc/*-release /etc/*-version;
      echo "***************************";
      echo "* Output /etc/issue file: *";
      cat /etc/issue
      echo "***************************";

    # For at least Apple Mac OSX, there is no lsb_release program or /etc/*-release /etc/*-version
    # files but autoconf will guess the vendor and the OS release correctly (so use those vaules).
      echo "Autoconf computed value for cpu       = $build_cpu"
      echo "Autoconf computed value for OS vendor = $build_vendor"
      echo "Autoconf computed value for OS        = $build_os"
      echo "***************************";

    # Fix the case of Apple OSX support.
      if test "x$build_vendor" = xapple; then
         OS_vendor=$build_vendor
         case $build_os in
            darwin8*)
               OS_release=10.4
               ;;
            darwin9*)
               OS_release=10.5
               ;;
            darwin10*)
               OS_release=10.6
               ;;
            darwin11*)
               OS_release=10.7
               ;;
            *)
             echo "Error: Apple Mac OSX version not recognized as either darwin8 or darwin9 ... (build_os = $build_os)";
             exit 1;
             OS_release="";;
         esac
         echo "Identified Apple OSX platform OS_vendor = $OS_vendor OS_release = $OS_release"
      fi
    # exit 1
   else
      echo "lsb_release IS available ROSE_LSB_RELEASE = $ROSE_LSB_RELEASE";
      OS_vendor=`lsb_release -is`
      OS_release=`lsb_release -rs`

      echo "In conditional: OS_vendor  = $OS_vendor"
      echo "In conditional: OS_release = $OS_release"
   fi

   echo "In conditional: OS_vendor  = $OS_vendor"
   echo "In conditional: OS_release = $OS_release"

   case $OS_vendor in
      Debian*)
         DEBIAN=yes
         AC_DEFINE([ROSE_DEBIAN_OS_VENDOR], [] , [Debian Operating System (OS) being used to build ROSE])
         ;;
      RedHat*)
         REDHAT=yes
         AC_DEFINE([ROSE_REDHAT_OS_VENDOR], [] , [RedHat Operating System (OS) being used to build ROSE])
         ;;
      Ubuntu*)
         UBUNTU=yes
         AC_DEFINE([ROSE_UBUNTU_OS_VENDOR], [] , [Ubuntu Operating System (OS) being used to build ROSE])
         ;;
      CentOS*)
         CENTOS=yes
         AC_DEFINE([ROSE_CENTOS_OS_VENDOR], [] , [CentOS Operating System (OS) being used to build ROSE])
         ;;
      apple*)
         APPLE=yes
         AC_DEFINE([ROSE_APPLE_OS_VENDOR], [] , [apple Operating System (OS) being used to build ROSE])
         ;;
      esac
         AM_CONDITIONAL([OS_VENDOR_DEBIAN],[ test "x$DEBIAN" = xyes ] )
         AM_CONDITIONAL([OS_VENDOR_REDHAT],[ test "x$REDHAT" = xyes ] )
         AM_CONDITIONAL([OS_VENDOR_UBUNTU],[ test "x$UBUNTU" = xyes ] )
         AM_CONDITIONAL([OS_VENDOR_CENTOS],[ test "x$CENTOS" = xyes ] )
         AM_CONDITIONAL([OS_VENDOR_APPLE],[ test "x$APPLE"  = xyes ] )

       # Added conditionals for 32-bit vs. 64-bit OS (used only in the binary analysis work -- and rarely).
         AM_CONDITIONAL([OS_32BIT],[ test "x$build_cpu" = xi686 ] )
         AM_CONDITIONAL([OS_64BIT],[ test "x$build_cpu" = xx86_64 ] )

         AM_CONDITIONAL([OS_VENDOR_REDHAT_32BIT],[ test "x$REDHAT" = xyes -a "x$build_cpu" = xi686 ] )

   echo "Leaving DETERMINE OS VENDOR: OS_vendor  = $OS_vendor"
   echo "Leaving DETERMINE OS VENDOR: OS_release = $OS_release"

   AC_SUBST(OS_vendor)
   AC_SUBST(OS_release)

 # AC_DEFINE_UNQUOTED([ROSE_OS_VENDOR], $OS_vendor , [Operating System (OS) being used to build ROSE])
 # AC_DEFINE([ROSE_OS_VENDOR], $OS_vendor , [Operating System (OS) being used to build ROSE])

  ])

