dnl Tests for presence of doxygen and some ROSE-specific switches for
dnl configuring documentation.
AC_DEFUN([ROSE_SUPPORT_DOXYGEN],
[
    DOXYGEN_DISABLE_HOWTO="Use --without-doxygen to disable generation of ROSE API documentation."

    dnl Find the doxygen program
    ROSE_ARG_WITH(
        [doxygen],                                                      dnl  --with-[doxygen]
        [for doxygen],                                                  dnl "checking [for doxygen]"
        [Program for generating the API reference manual.],             dnl help string
        [yes])                                                          dnl default is to use system version

    if test "$ROSE_WITH_DOXYGEN" = "no"; then
        DOXYGEN=
    elif test "$ROSE_WITH_DOXYGEN" = "yes"; then
        DOXYGEN=doxygen
    else
        DOXYGEN="$ROSE_WITH_DOXYGEN"
    fi

    dnl Check the doxygen version number if doxygen is available
    if test "$DOXYGEN" != ""; then
        DOXYGEN_VERSION=$($DOXYGEN --version 2>/dev/null)
        if test "$DOXYGEN_VERSION" = ""; then
            AC_MSG_FAILURE([doxygen command "$DOXYGEN --version" did not report a version number; $DOXYGEN_DISABLE_HOWTO])
        fi

        DOXYGEN_VERSION_HAVE_MAJOR=$(echo "$DOXYGEN_VERSION" |cut -d. -f1)
        DOXYGEN_VERSION_HAVE_MINOR=$(echo "$DOXYGEN_VERSION" |cut -d. -f2)
        DOXYGEN_VERSION_HAVE_PATCH=$(echo "$DOXYGEN_VERSION" |cut -d. -f3)
        if test "$DOXYGEN_VERSION_HAVE_MAJOR" = "" -o \
                "$DOXYGEN_VERSION_HAVE_MINOR" = "" -o \
                "$DOXYGEN_VERSION_HAVE_PATCH" = ""; then
            AC_MSG_FAILURE([malformed doxygen version number "$DOXYGEN_VERSION" reported by "$DOXYGEN --version"])
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
            AC_MSG_FAILURE([$DOXYGEN $DOXYGEN_VERSION is too old; need at least $DOXYGEN_VERSION_NEED. $DOXYGEN_DISABLE_HOWTO])
        fi
    fi

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
        DOXYGEN_EXTRACT_ALL="NO"
        DOXYGEN_EXTRACT_PRIVATE="NO"
        DOXYGEN_EXTRACT_STATIC="NO"
        DOXYGEN_INTERNAL_DOCS="NO"
        DOXYGEN_HIDE_UNDOC_MEMBERS="YES"
        DOXYGEN_HIDE_UNDOC_CLASSES="YES"
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
      echo "Permit separation of Sage III doxygen generated documentation from Rose documentation uses doxygen tag file mechanism to connect the two"
      DOXYGEN_ADDED_DIRECTORY_1=""
      DOXYGEN_ADDED_DIRECTORY_2=""
      DOXYGEN_USE_TAG_FILE="@top_pwd@/docs/Rose/SageIntermediateForm/sageDoxygen.tags"
    else
      echo "Permit Doxygen generation of Rose documentation including Sage III documentation as a single monolithic project; takes longer"
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
