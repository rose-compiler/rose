AC_DEFUN([ROSE_SUPPORT_EDG], [

    ###############################################################################################################################
    # This part of the configuration is about what *version* of EDG to use when building ROSE. It has nothing to do with deciding
    # whether to compile EDG from source code, use a precompiled EDG binary, or do neither (that's all later).
    ###############################################################################################################################

    AC_ARG_ENABLE([edg-version],
                  [AC_HELP_STRING([--enable-edg-version=VERSION],
                                  [Specifies the version of EDG to use as the parser for C/C++ when ROSE is configured
                                   to be able to analyze C and C++. Specifying a version number with this switch causes
                                   that version of the EDG library to be used by either compiling it from source code
                                   (which is proprietary) or by downloading a precompiled library.])])

    AC_MSG_NOTICE([enable_edg_version = "$enable_edg_version"])
    if test "$enable_edg_version" = ""; then
       AC_MSG_NOTICE([default version of EDG used (5.0)])
       edg_major_version_number=5
       edg_minor_version_number=0
    else
       AC_MSG_NOTICE([specifying EDG version is not recommended])
       edg_major_version_number=`echo $enable_edg_version | cut -d\. -f1`
       edg_minor_version_number=`echo $enable_edg_version | cut -d\. -f2`
    fi

    # Only certain versions of EDG are valid
    if test "$edg_major_version_number" = "4"; then
        if test "$edg_minor_version_number" = "12"; then
            enable_edg_version412=yes
            AC_DEFINE([ROSE_USE_EDG_VERSION_4_12], [], [Whether to use the new EDG version 4.12])
        else
            AC_MSG_FAILURE([EDG-$edg_major_version_number.$edg_minor_version_number is not valid])
        fi
    elif test "x$edg_major_version_number" = "x5"; then
        if test "x$edg_minor_version_number" = "x0"; then
            enable_edg_version50=yes
            AC_DEFINE([ROSE_USE_EDG_VERSION_5_0], [], [Whether to use the new EDG version 5.0])
        else
            AC_MSG_FAILURE([EDG-$edg_major_version_number.$edg_minor_version_number is not valid])
        fi
    elif test "x$edg_major_version_number" = "x6"; then
        if test "x$edg_minor_version_number" = "x0"; then
            enable_edg_version60=yes
            AC_DEFINE([ROSE_USE_EDG_VERSION_6_0], [], [Whether to use the new EDG version 6.0])
        elif test "x$edg_minor_version_number" = "x1"; then
            enable_edg_version61=yes
            AC_DEFINE([ROSE_USE_EDG_VERSION_6_1], [], [Whether to use the new EDG version 6.1])
        else
            AC_MSG_FAILURE([EDG-$edg_major_version_number.$edg_minor_version_number is not valid])
        fi
    else
        AC_MSG_FAILURE([EDG-$edg_major_version_number.$edg_minor_version_number is not valid])
    fi

    enable_new_edg_interface=yes
    AC_DEFINE_UNQUOTED([ROSE_EDG_MAJOR_VERSION_NUMBER], $edg_major_version_number , [EDG major version number])
    AC_DEFINE_UNQUOTED([ROSE_EDG_MINOR_VERSION_NUMBER], $edg_minor_version_number , [EDG minor version number])

    ROSE_EDG_MAJOR_VERSION_NUMBER=$edg_major_version_number
    ROSE_EDG_MINOR_VERSION_NUMBER=$edg_minor_version_number

    AC_SUBST(ROSE_EDG_MAJOR_VERSION_NUMBER)
    AC_SUBST(ROSE_EDG_MINOR_VERSION_NUMBER)

    # DQ (2/3/2010): I would like to not have to use these and use the new
    # ROSE_EDG_MAJOR_VERSION_NUMBER and ROSE_EDG_MINOR_VERSION_NUMBER instead.
    AM_CONDITIONAL(ROSE_USE_EDG_VERSION_4_12, [test "x$enable_edg_version412" = xyes])
    AM_CONDITIONAL(ROSE_USE_EDG_VERSION_5_0,  [test "x$enable_edg_version50" = xyes])
    AM_CONDITIONAL(ROSE_USE_EDG_VERSION_6_0,  [test "x$enable_edg_version60" = xyes])
    AM_CONDITIONAL(ROSE_USE_EDG_VERSION_6_1,  [test "x$enable_edg_version61" = xyes])

    ###############################################################################################################################
    # This part of the configuration is about whether to compile the EDG library from source code, or to use a precompiled EDG
    # library that's downloaded, or to not link with EDG at all (e.g., when ROSE is configured without C or C++ support).
    ###############################################################################################################################

    AC_ARG_ENABLE([edg-compile],
                  [AC_HELP_STRING([--enable-edg-compile=maybe|yes|no],
				  [Whether to compile the Edison Design Group (EDG) C/C++ parsing library from source code.
				   The default, "maybe", means that the library is compiled from source code if the source code
				   is available (it's proprietary) otherwise the build will attempt to download a precompiled
				   version of the library. The values "yes" and "no" are used by the ROSE development team for
				   testing and mean, respectively, fail if the source code is not available or ignore the source
				   code even if it is available.])],
		  [want_edg_source="$enableval"],
		  [want_edg_source=maybe])

    if test -e ${srcdir}/src/frontend/CxxFrontend/EDG/Makefile.am; then
        has_edg_source=yes
    else
        has_edg_source=no
    fi

    if test "$want_edg_source" = yes -a "$has_edg_source" = no; then
        AC_MSG_FAILURE(["--with-edg-source=yes" was specified but the EDG source code is not present])
    elif test "$want_edg_source" = no; then
        AC_MSG_NOTICE([EDG source code ignored even if present])
        has_edg_source=no
    fi

    AM_CONDITIONAL(ROSE_HAS_EDG_SOURCE, [test "$has_edg_source" = "yes"])

    ###############################################################################################################################
    # This part of the configuration is about whether we should build an EDG binary tarball by compiling the EDG source code.
    ###############################################################################################################################

    # Find md5 or md5sum and create a signature for ROSE binary compatibility
    AC_CHECK_PROGS(MD5, [md5 md5sum], [false])
    AC_SUBST(MD5)
    if test "$has_edg_source" = yes; then
        if test "$MD5" = "false"; then
            AC_MSG_WARN([could not find either md5 or md5sum; building binary EDG tarballs is disabled])
            binary_edg_tarball_enabled=no
        else
            binary_edg_tarball_enabled=yes
        fi
    else
        binary_edg_tarball_enabled=no
    fi

    AM_CONDITIONAL(BINARY_EDG_TARBALL_ENABLED, [test "$binary_edg_tarball_enabled" = "yes"])
])

