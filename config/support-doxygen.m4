dnl Tests for presence of doxygen and some ROSE-specific switches for
dnl configuring documentation.
AC_DEFUN([ROSE_SUPPORT_DOXYGEN],
[
    ROSE_CONFIGURE_SECTION([Checking Doxygen])

    DOXYGEN_DISABLE_HOWTO="Use --without-doxygen to disable generation of ROSE API documentation."

    dnl Find the doxygen program
    AC_ARG_WITH(
        [doxygen],
        AS_HELP_STRING(
            [--with-doxygen=EXECUTABLE],
            [Use the doxygen documentation generator from http://www.doxygen.org.
             The EXECUTABLE, if specified, should be the path of the doxygen tool, such as "/usr/local/bin/doxygen".
             The default is the empty prefix, in which case the tool "doxygen" must be found in the command
             search path. Saying "no" for the prefix is the same as saying "--without-doxygen".]),
            [ROSE_WITH_DOXYGEN=$withval],
            [ROSE_WITH_DOXYGEN=check])

    DOXYGEN_FAIL_IF_NOT_FOUND=                                          dnl 'yes' means fail if we don't find doxygen
    DOXYGEN=                                                            dnl doxygen executable

    if test "$ROSE_WITH_DOXYGEN" = "no"; then
        dnl Do not look for or use doxygen
        DOXYGEN=
        DOXYGEN_FAIL_IF_NOT_FOUND=
    elif test "$ROSE_WITH_DOXYGEN" = "yes"; then
        dnl User wants to use the system-installed doxygen version
        DOXYGEN=doxygen
        DOXYGEN_FAIL_IF_NOT_FOUND=yes
    elif test "$ROSE_WITH_DOXYGEN" = "check"; then
        dnl User doesn't care; use doxygen only if we can find an appropriate system-installed version
        DOXYGEN=doxygen
        DOXYGEN_FAIL_IF_NOT_FOUND=
    else
        dnl User wants us to use a specific doxygen executable, so fail if that's not appropriate
        DOXYGEN="$ROSE_WITH_DOXYGEN"
        DOXYGEN_FAIL_IF_NOT_FOUND=yes
    fi

    dnl Check the doxygen version number if doxygen is available. If doxygen is not found or its too old a version then either
    dnl fail (if DOXYGEN_FAIL_IF_NOT_FOUND is 'yes') or assume that doxygen is not desired.
    while true; do
        if test "$DOXYGEN" != ""; then
            DOXYGEN_VERSION=$($DOXYGEN --version 2>/dev/null)
            if test "$DOXYGEN_VERSION" = ""; then
                if test "$DOXYGEN_FAIL_IF_NOT_FOUND" = yes; then
                    AC_MSG_FAILURE([doxygen command "$DOXYGEN --version" did not report a version number; $DOXYGEN_DISABLE_HOWTO])
                else
                    DOXYGEN=
                    break
                fi
            fi

            DOXYGEN_VERSION_HAVE_MAJOR=$(echo "$DOXYGEN_VERSION" |cut -d. -f1)
            DOXYGEN_VERSION_HAVE_MINOR=$(echo "$DOXYGEN_VERSION" |cut -d. -f2)
            DOXYGEN_VERSION_HAVE_PATCH=$(echo "$DOXYGEN_VERSION" |cut -d. -f3)
            if test "$DOXYGEN_VERSION_HAVE_MAJOR" = "" -o \
                    "$DOXYGEN_VERSION_HAVE_MINOR" = "" -o \
                    "$DOXYGEN_VERSION_HAVE_PATCH" = ""; then
                if test "$DOXYGEN_FAIL_IF_NOT_FOUND" = yes; then
                    AC_MSG_FAILURE([malformed doxygen version number "$DOXYGEN_VERSION" reported by "$DOXYGEN --version"])
                else
                    DOXYGEN=
                    break
                fi
            fi

            DOXYGEN_VERSION_NEED="1.8.1"
            DOXYGEN_VERSION_NEED_MAJOR=$(echo "$DOXYGEN_VERSION_NEED" |cut -d. -f1)
            DOXYGEN_VERSION_NEED_MINOR=$(echo "$DOXYGEN_VERSION_NEED" |cut -d. -f2)
            DOXYGEN_VERSION_NEED_PATCH=$(echo "$DOXYGEN_VERSION_NEED" |cut -d. -f3)

            if test "$DOXYGEN_VERSION_HAVE_MAJOR" -gt "$DOXYGEN_VERSION_NEED_MAJOR"; then
                DOXYGEN_VERSION_IS_OKAY=yes
            elif test "$DOXYGEN_VERSION_HAVE_MAJOR" -lt "$DOXYGEN_VERSION_NEED_MAJOR"; then
                DOXYGEN_VERSION_IS_OKAY=no
            elif test "$DOXYGEN_VERSION_HAVE_MINOR" -gt "$DOXYGEN_VERSION_NEED_MINOR"; then
                DOXYGEN_VERSION_IS_OKAY=yes
            elif test "$DOXYGEN_VERSION_HAVE_MINOR" -lt "$DOXYGEN_VERSION_NEED_MINOR"; then
                DOXYGEN_VERSION_IS_OKAY=no
            elif test "$DOXYGEN_VERSION_HAVE_PATCH" -ge "$DOXYGEN_VERSION_NEED_PATCH"; then
                DOXYGEN_VERSION_IS_OKAY=yes
            else
                DOXYGEN_VERSION_IS_OKAY=no
            fi

            if test "$DOXYGEN_VERSION_IS_OKAY" != "yes"; then
                if test "$DOXYGEN_FAIL_IF_NOT_FOUND" = yes; then
                    AC_MSG_FAILURE([$DOXYGEN $DOXYGEN_VERSION is too old; need at least $DOXYGEN_VERSION_NEED. $DOXYGEN_DISABLE_HOWTO])
                else
                    DOXYGEN=
                    break
                fi
            fi
        fi
        break
    done

    dnl Find the optional doxyindexer program. It's normally in the same directory as doxygen but not
    dnl always present. It didn't exist before 1.8.3, and it's not always installed after that.
    if test "$DOXYGEN" != "" -a "$DOXYGEN_VERSION_IS_OKAY" = "yes"; then
        DOXYDIR="${DOXYGEN%/*}"
        if test "$DOXYDIR" = "$DOXYGEN"; then
            dnl $DOXYGEN had no path; it was probably just the word "doxygen"
            AC_PATH_PROG(DOXYINDEXER, doxyindexer)
        elif test "${DOXYDIR#/}" != "$DOXYDIR"; then
            dnl Absolute path. AC_PATH_PROG is not smart enough to handle this case!
            if test -e "$DOXYDIR/doxyindexer"; then
                DOXYINDEXER="$DOXYDIR/doxyindexer"
            fi
        else
            AC_PATH_PROG(DOXYINDEXER, [$DOXYDIR/doxyindexer])
        fi
    fi

    dnl Results for the doxygen executable
    AM_CONDITIONAL(ROSE_HAVE_DOXYGEN, test "$DOXYGEN" != "")
    AC_SUBST(DOXYGEN, "$DOXYGEN")
    AM_CONDITIONAL(ROSE_HAVE_DOXYINDEXER, test "$DOXYINDEXER" != "")
    AC_SUBST(DOXYINDEXER, "$DOXYINDEXER")

    dnl Should we document private, internal parts of the library?  End user documentation should not contain
    dnl documentaton for internal or private parts of ROSE. Developers can get this by adding
    dnl "--with-doxygen-developer-docs" to the configure command-line.
    AC_ARG_ENABLE(doxygen-developer-docs,
                  [AS_HELP_STRING([--enable-doxygen-developer-docs],
                                  [Produce documentation for private, internal library components])],
                  [enable_doxygen_developer_docs=yes],
                  [enable_doxygen_developer_docs=])
    if test "$enable_doxygen_developer_docs" = "yes"; then
        DOXYGEN_EXTRACT_ALL="YES"
        DOXYGEN_EXTRACT_PRIVATE="YES"
        DOXYGEN_EXTRACT_STATIC="YES"
        DOXYGEN_INTERNAL_DOCS="YES"
        DOXYGEN_HIDE_UNDOC_MEMBERS="NO"
        DOXYGEN_HIDE_UNDOC_CLASSES="NO"
        DOXYGEN_HIDE_FRIEND_COMPOUNDS="NO"
        DOXYGEN_ENABLED_SECTIONS="documentDevelopmentVersionUsingDoxygen"
    else
        dnl [Robb Matzke, 2015-09-30]: I'm turning off DOXYGEN_HIDE_UNDOC_MEMBERS and DOXYGEN_HIDE_UNDOC_CLASSES by
        dnl default until we've had a change to properly document the multitude of things that have no documentation.
        DOXYGEN_EXTRACT_ALL="NO"
        DOXYGEN_EXTRACT_PRIVATE="NO"
        DOXYGEN_EXTRACT_STATIC="NO"
        DOXYGEN_INTERNAL_DOCS="NO"
        DOXYGEN_HIDE_UNDOC_MEMBERS="NO"
        DOXYGEN_HIDE_UNDOC_CLASSES="NO"
        DOXYGEN_HIDE_FRIEND_COMPOUNDS="YES"
        DOXYGEN_ENABLED_SECTIONS="documentUserVersionUsingDoxygen"
    fi

    AC_SUBST(DOXYGEN_EXTRACT_ALL)
    AC_SUBST(DOXYGEN_EXTRACT_PRIVATE)
    AC_SUBST(DOXYGEN_EXTRACT_STATIC)
    AC_SUBST(DOXYGEN_INTERNAL_DOCS)
    AC_SUBST(DOXYGEN_HIDE_UNDOC_MEMBERS)
    AC_SUBST(DOXYGEN_HIDE_UNDOC_CLASSES)
    AC_SUBST(DOXYGEN_HIDE_FRIEND_COMPOUNDS)
    AC_SUBST(DOXYGEN_ENABLED_SECTIONS)

dnl Start of some old stuff...
    dnl *********************************************************************
    dnl * Enable Doxygen build documentation for Rose faster by connecting 
    dnl * Sage III as a separate set of documentation to Rose using the 
    dnl * Doxygen Tag file mechanism.  The documentation is generated much 
    dnl * faster which simplifies the addition of documentation to both Rose 
    dnl * and Sage separately, but it is not as presentable.
    dnl *********************************************************************
    AC_ARG_ENABLE(doxygen-generate-fast-docs,
      [  --enable-doxygen-generate-fast-docs ...... Enable faster generation of Doxygen documents using 
                                             tag file mechanism to connect Sage III documentation to 
                                             Rose documentation (documentation not as presentable).])
    case $srcdir in
      /*) topSourceDirectory=$srcdir ;;
      *) topSourceDirectory=$PWD/$srcdir ;;
    esac

    dnl DQ (12/20/2002) support for Doxygen specific options
    dnl We might want the default to later be to merge Rose and 
    dnl Sage documentation (change it then).
    if (test "$enable_doxygen_generate_fast_docs" = yes) ; then
      AC_MSG_NOTICE([permit separation of Sage III doxygen generated documentation from Rose documentation uses doxygen tag file mechanism to connect the two])
      DOXYGEN_ADDED_DIRECTORY_1=""
      DOXYGEN_ADDED_DIRECTORY_2=""
      DOXYGEN_USE_TAG_FILE="@top_pwd@/docs/Rose/SageIntermediateForm/sageDoxygen.tags"
    else
      AC_MSG_NOTICE([permit Doxygen generation of Rose documentation including Sage III documentation as a single monolithic project; takes longer])
      DOXYGEN_ADDED_DIRECTORY_1="@top_pwd@/src/frontend/SageIII"
      DOXYGEN_ADDED_DIRECTORY_2="$topSourceDirectory/src/frontend/SageIII"
      DOXYGEN_USE_TAG_FILE=""
    fi

    AC_SUBST(DOXYGEN_ADDED_DIRECTORY_1)
    AC_SUBST(DOXYGEN_ADDED_DIRECTORY_2)
    AC_SUBST(DOXYGEN_USE_TAG_FILE)
dnl End of old stuff

    dnl Report results
    AC_MSG_NOTICE([Doxygen detection results:])
    AC_MSG_NOTICE([    DOXYGEN                       = $DOXYGEN])
    AC_MSG_NOTICE([    DOXYGEN_VERSION               = $DOXYGEN_VERSION])
    AC_MSG_NOTICE([    DOXYINDEXER                   = $DOXYINDEXER])
    AC_MSG_NOTICE([    DOXYGEN_EXTRACT_ALL           = $DOXYGEN_EXTRACT_ALL])
    AC_MSG_NOTICE([    DOXYGEN_EXTRACT_PRIVATE       = $DOXYGEN_EXTRACT_PRIVATE])
    AC_MSG_NOTICE([    DOXYGEN_EXTRACT_STATIC        = $DOXYGEN_EXTRACT_STATIC])
    AC_MSG_NOTICE([    DOXYGEN_INTERNAL_DOCS         = $DOXYGEN_INTERNAL_DOCS])
    AC_MSG_NOTICE([    DOXYGEN_HIDE_UNDOC_MEMBERS    = $DOXYGEN_HIDE_UNDOC_MEMBERS])
    AC_MSG_NOTICE([    DOXYGEN_HIDE_UNDOC_CLASSES    = $DOXYGEN_HIDE_UNDOC_CLASSES])
    AC_MSG_NOTICE([    DOXYGEN_HIDE_FRIEND_COMPOUNDS = $DOXYGEN_HIDE_FRIEND_COMPOUNDS])
    AC_MSG_NOTICE([    DOXYGEN_ENABLED_SECTIONS      = $DOXYGEN_ENABLED_SECTIONS])
    AC_MSG_NOTICE([    DOXYGEN_ADDED_DIRECTORY_1     = $DOXYGEN_ADDED_DIRECTORY_1])
    AC_MSG_NOTICE([    DOXYGEN_ADDED_DIRECTORY_2     = $DOXYGEN_ADDED_DIRECTORY_2])
    AC_MSG_NOTICE([    DOXYGEN_USE_TAG_FILE          = $DOXYGEN_USE_TAG_FILE])
])
