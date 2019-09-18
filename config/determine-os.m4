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
        solaris*)
            SOLARIS=yes;
            ;;
    esac
    AM_CONDITIONAL([OS_MACOSX], [ test "x$MACOSX"  = xyes ] )
    AM_CONDITIONAL([OS_LINUX],  [ test "x$LINUX"   = xyes ] )
    AM_CONDITIONAL([OS_MINGW],  [ test "x$MINGW"   = xyes ] )
    AM_CONDITIONAL([OS_MSDOS],  [ test "x$MSDOS"   = xyes ] )
    AM_CONDITIONAL([OS_SOLARIS],[ test "x$SOLARIS" = xyes ] )
])


# /etc/redhat-release
# /etc/debian_version
# /etc/SuSE-release
# /etc/slackware-version
# /etc/gentoo-release
# You could do 'cat /etc/*-release' or 'cat /etc/*-version'.

dnl DQ (9/10/2009): This is a macro I wrote to nail down the OS vendor so that
dnl I could skip specific tests in ROSE based on the OS vendor as part of the
dnl NMI testing of ROSE on the Compile Farm.
AC_DEFUN([DETERMINE_OS_VENDOR],
[
    AC_CHECK_TOOL(ROSE_LSB_RELEASE, [lsb_release], [no])

    OS_vendor="ROSE_unknown_OS";
    OS_release="ROSE_unknown_OS_release";

    if test "x$ROSE_LSB_RELEASE" = xno; then
        dnl Most OS's output their name buried in /etc/issue
        dnl  ls -dl /etc/*-release /etc/*-version;
        dnl  echo "***************************";
        dnl  echo "* Output /etc/issue file: *";
        dnl  cat /etc/issue
        dnl  echo "***************************";

        # PP (05/14/2019) add solaris
        dnl Fix the case of Apple OSX and Sun/Oracle Solaris support.
        dnl
        dnl For at least Apple Mac OSX and Solaris, there is no lsb_release program or /etc/*-release /etc/*-version
        dnl files but autoconf will guess the vendor and the OS release correctly (so use those values).
        case $build_vendor in
            apple)
                OS_vendor=$build_vendor
                case $build_os in
                    darwin13*)
                        OS_release=10.9
                        ;;
                    darwin14*)
                        OS_release=10.10
                        ;;
                    darwin15*)
                        OS_release=10.11
                        ;;
                    darwin16*)
                        OS_release=10.12
                        ;;
                    darwin17*)
                        OS_release=10.13
                        ;;
                    darwin18*)
                        OS_release=10.14
                        ;;
                    *)
                        AC_MSG_ERROR([Apple macOS X minor version not recognized as either darwin13 through darwin18 (macOS 10.9-10.14) ... (build_os = $build_os)])
                        ;;
                esac
                ;;

            sun) 
                OS_vendor=$build_vendor
                case $build_os in
                    solaris2.10)                 
                        OS_release=2.10
                        ;;
                    *)
                        AC_MSG_ERROR([Solaris version not supported (only solaris-2.10 is supported, build_os = $build_os)])
                        ;;
                esac
                ;;
        esac  
    else
        OS_vendor=`lsb_release -is`
        OS_release=`lsb_release -rs`
    fi

    AC_MSG_CHECKING([$OS_vendor release version])
    AC_MSG_RESULT([$OS_release])

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



    # Major and minor release numbers for OS.
    OS_major_release_number=`echo "$OS_release" |cut -d. -f1`
    OS_minor_release_number=`echo "$OS_release" |cut -d. -f2`

    AC_MSG_CHECKING([$OS_vendor major release number])
    AC_MSG_RESULT([$OS_major_release_number])
    AC_MSG_CHECKING([$OS_vendor minor release number])
    AC_MSG_RESULT([$OS_minor_release_number])

    AM_CONDITIONAL([OS_VENDOR_DEBIAN],[ test "x$DEBIAN" = xyes ])
    AM_CONDITIONAL([OS_VENDOR_REDHAT],[ test "x$REDHAT" = xyes ])
    AM_CONDITIONAL([OS_VENDOR_UBUNTU],[ test "x$UBUNTU" = xyes ])
    AM_CONDITIONAL([OS_VENDOR_CENTOS],[ test "x$CENTOS" = xyes ])
    AM_CONDITIONAL([OS_VENDOR_APPLE],[ test "x$APPLE"  = xyes ])

    # Conditionals for 32-bit vs. 64-bit OS (used only in the binary analysis work -- and rarely).
    AM_CONDITIONAL([OS_32BIT],[ test "x$build_cpu" = xi686 ])
    AM_CONDITIONAL([OS_64BIT],[ test "x$build_cpu" = xx86_64 ])

    AM_CONDITIONAL([OS_VENDOR_REDHAT_32BIT],[ test "x$REDHAT" = xyes -a "x$build_cpu" = xi686 ])

    # Major and minor release numbers for REDHAT OS.
    AM_CONDITIONAL([OS_VENDOR_REDHAT_REL5],[ test "x$REDHAT" = xyes -a "x$OS_major_release_number" = x5])
    AM_CONDITIONAL([OS_VENDOR_REDHAT_REL6],[ test "x$REDHAT" = xyes -a "x$OS_major_release_number" = x6])

    AC_SUBST(OS_vendor)
    AC_SUBST(OS_release)

    # Major release number for OS.
    AC_DEFINE_UNQUOTED([ROSE_OS_MAJOR_RELEASE_NUMBER], $OS_major_release_number , [RedHat Operating System (OS) major release number being used to build ROSE])
    AC_DEFINE_UNQUOTED([ROSE_OS_MINOR_RELEASE_NUMBER], $OS_minor_release_number , [RedHat Operating System (OS) minor release number being used to build ROSE])

    # Major and minor release numbers for REDHAT OS.
    AC_SUBST(OS_major_release_number)
    AC_SUBST(OS_minor_release_number)
])
