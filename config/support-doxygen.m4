dnl Tests for presence of doxygen and some ROSE-specific switches for
dnl configuring documentation.
AC_DEFUN([ROSE_SUPPORT_DOXYGEN],
[
    DOXYGEN_DISABLE_HOWTO="Use --without-doxygen to disable generation of ROSE API documentation."

    dnl Find the doxygen program
    ROSE_ARG_WITH(
        [doxygen],							dnl  --with-[doxygen]
        [for doxygen],							dnl "checking [for doxygen]"
        [Program for generating the API reference manual.],		dnl help string
        [yes])	     		    		  			dnl default is to use system version

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

    # Report    
    if test "$DOXYGEN" = ""; then
        AC_MSG_WARN([The ROSE API reference manual will not be generated (needs --with-doxygen)])
    else      
        AC_MSG_NOTICE([$DOXYGEN version $DOXYGEN_VERSION is used to generate the ROSE API reference manual.])
    fi


    # Results for the doxygen executable
    AM_CONDITIONAL(ROSE_HAVE_DOXYGEN, test "$DOXYGEN" != "")
    AC_SUBST(DOXYGEN, "$DOXYGEN")


# The remaining stuff is the original content of this macro and enables/disables various features
# assuming that doxygen is present.
    


# DQ Comment: We need a better name than --enable-doxygen-internal
# use: --enable-developer-docs

dnl *********************************************************************
dnl * Enable Doxygen to display internal information on ROSE project
dnl *********************************************************************
dnl 
dnl DQ (8/25/2004): Make the default to generate the developer documentation
dnl so that configure without any options will generate reasonable documentation
dnl within its construction of the documentation.
dnl
dnl AC_ARG_ENABLE(doxygen-developer-docs,
dnl   [  --enable-doxygen-developer-docs .......... Enable display of internal project detail with Doxygen.])
AC_ARG_ENABLE(doxygen-developer-docs,
  [  --enable-doxygen-developer-docs .......... Enable display of internal project detail with Doxygen.],
  [enable_doxygen_developer_docs=yes],
  [enable_doxygen_developer_docs=yes])

echo "enable_doxygen_developer_docs = $enable_doxygen_developer_docs"

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

dnl DQ (12/15/2002) support for Doxygen specific options
if test "$enable_doxygen_developer_docs" = yes ; then
  echo "force generation of internal project detail"
  dnl AC_DEFINE(DOXYGEN_EXTRACT_ALL,"YES",[Force Doxygen to document everything (even if no documentation is explicit).])
  dnl AC_DEFINE(DOXYGEN_EXTRACT_PRIVATE,YES,[Force Doxygen to document private interfaces.])
  dnl AC_DEFINE(DOXYGEN_EXTRACT_STATIC,YES,[Force Doxygen to document static interfaces.])
  dnl AC_DEFINE(DOXYGEN_EXTRACT_INTERNAL_DOCS,YES,[Force Doxygen to output all internal project documentation.])
  DOXYGEN_EXTRACT_ALL="YES"
  DOXYGEN_EXTRACT_PRIVATE="YES"
  DOXYGEN_EXTRACT_STATIC="YES"
  DOXYGEN_INTERNAL_DOCS="YES"
  DOXYGEN_HIDE_UNDOC_MEMBERS="NO"
  DOXYGEN_HIDE_UNDOC_CLASSES="NO"
  DOXYGEN_HIDE_FRIEND_COMPOUNDS="NO"
  DOXYGEN_ENABLED_SECTIONS="documentDevelopmentVersionUsingDoxygen"
else
  echo "generation user documentation only (no internal project detail)"
  dnl AC_DEFINE([DOXYGEN_EXTRACT_ALL],[NO],[Force Doxygen to document everything (even if no documentation is explicit).])
  dnl AC_DEFINE([DOXYGEN_EXTRACT_PRIVATE],[NO],[Force Doxygen to document private interfaces.])
  dnl AC_DEFINE([DOXYGEN_EXTRACT_STATIC],[NO],[Force Doxygen to document static interfaces.])
  dnl AC_DEFINE([DOXYGEN_EXTRACT_INTERNAL_DOCS],[NO],[Force Doxygen to output all internal project documentation.])
  DOXYGEN_EXTRACT_ALL="NO"
  DOXYGEN_EXTRACT_PRIVATE="NO"
  DOXYGEN_EXTRACT_STATIC="NO"
  DOXYGEN_INTERNAL_DOCS="NO"
  DOXYGEN_HIDE_UNDOC_MEMBERS="YES"
  DOXYGEN_HIDE_UNDOC_CLASSES="YES"
  DOXYGEN_HIDE_FRIEND_COMPOUNDS="YES"
  DOXYGEN_ENABLED_SECTIONS="documentUserVersionUsingDoxygen"
fi

echo "srcdir = $srcdir"
# echo "top_srcdir = $top_srcdir"
# echo "ac_top_srcdir = $ac_top_srcdir"
# echo "enable_doxygen_generate_fast_docs = $enable_doxygen_generate_fast_docs"

# DQ (7/13/2004): Fix for compiling in the source tree.
# Doxygen is easier to use if it uses an absolute path to
# find the ROSE documentation.  So we build an absolute path
# from the srcdir variable if it is a relative path.
# Note that top_srcdir is not set if we build the compile
# tree in the source tree. So this fixes that support in ROSE.
# if (test -d /$srcdir); then
# if ( (test -d /$srcdir) && ( test $srcdir != "." ) ); then
#     echo "Absolute Source Tree Path Found"
#     topSourceDirectory=$srcdir
#   else
#     echo "Relative Source Tree Path Found (computing source directory)"
#     current_dir=$PWD
#     cd $srcdir;
#     topSourceDirectory=`pwd`
#     cd $current_dir
#     final_location_dir=$PWD
# fi

# DQ (7/25/2004): This is better code (found on the web, it sure is cute)
case $srcdir in
  /*) topSourceDirectory=$srcdir ;;
  *) topSourceDirectory=$PWD/$srcdir ;;
esac

echo "Computed topSourceDirectory (Absolute Path) = $topSourceDirectory"

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

dnl Debugging output
echo "In macro ROSE SUPPORT DOXYGEN: DOXYGEN_EXTRACT_ALL           = $DOXYGEN_EXTRACT_ALL"
echo "In macro ROSE SUPPORT DOXYGEN: DOXYGEN_EXTRACT_PRIVATE       = $DOXYGEN_EXTRACT_PRIVATE"
echo "In macro ROSE SUPPORT DOXYGEN: DOXYGEN_EXTRACT_STATIC        = $DOXYGEN_EXTRACT_STATIC"
echo "In macro ROSE SUPPORT DOXYGEN: DOXYGEN_INTERNAL_DOCS         = $DOXYGEN_INTERNAL_DOCS"
echo "In macro ROSE SUPPORT DOXYGEN: DOXYGEN_HIDE_UNDOC_MEMBERS    = $DOXYGEN_HIDE_UNDOC_MEMBERS"
echo "In macro ROSE SUPPORT DOXYGEN: DOXYGEN_HIDE_UNDOC_CLASSES    = $DOXYGEN_HIDE_UNDOC_CLASSES"
echo "In macro ROSE SUPPORT DOXYGEN: DOXYGEN_HIDE_FRIEND_COMPOUNDS = $DOXYGEN_HIDE_FRIEND_COMPOUNDS"
echo "In macro ROSE SUPPORT DOXYGEN: DOXYGEN_ENABLED_SECTIONS      = $DOXYGEN_ENABLED_SECTIONS"
echo "In macro ROSE SUPPORT DOXYGEN: DOXYGEN_ADDED_DIRECTORY_1     = $DOXYGEN_ADDED_DIRECTORY_1"
echo "In macro ROSE SUPPORT DOXYGEN: DOXYGEN_ADDED_DIRECTORY_2     = $DOXYGEN_ADDED_DIRECTORY_2"
echo "In macro ROSE SUPPORT DOXYGEN: DOXYGEN_USE_TAG_FILE          = $DOXYGEN_USE_TAG_FILE"

AC_SUBST(DOXYGEN_EXTRACT_ALL)
AC_SUBST(DOXYGEN_EXTRACT_PRIVATE)
AC_SUBST(DOXYGEN_EXTRACT_STATIC)
AC_SUBST(DOXYGEN_INTERNAL_DOCS)
AC_SUBST(DOXYGEN_HIDE_UNDOC_MEMBERS)
AC_SUBST(DOXYGEN_HIDE_UNDOC_CLASSES)
AC_SUBST(DOXYGEN_HIDE_FRIEND_COMPOUNDS)
AC_SUBST(DOXYGEN_ENABLED_SECTIONS)
AC_SUBST(DOXYGEN_ADDED_DIRECTORY_1)
AC_SUBST(DOXYGEN_ADDED_DIRECTORY_2)
AC_SUBST(DOXYGEN_USE_TAG_FILE)

# End macro ROSE_SUPPORT_DOXYGEN.
]
)







