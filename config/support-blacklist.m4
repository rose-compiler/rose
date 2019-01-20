dnl Support for gracefully bailing out of configure for certain combinations of dependency versions
dnl or other situations.

AC_DEFUN([ROSE_SUPPORT_BLACKLIST],[
    prohibited=

    dnl -----------------------
    dnl    Versions we HAVE
    dnl -----------------------

    dnl Debugging blacklists... Spit out the values that will be needed by users when they're
    dnl trying to figure out why their configuration is blacklisted. These are the versions they
    dnl HAVE. The versions that are PROHIBITED will be emitted later in this function.  The version
    dnl number variables should be set wherever we're detecting the dependency, not here.
    ROSE_SUPPORT_SUMMARY

    dnl -----------------------
    dnl  Versions we PROHIBIT
    dnl -----------------------

    ROSE_CONFIGURE_SECTION([Checking blacklisted configurations])
    while true; do
        dnl Boost 1.54 has numerous bugs related to threads. These manifest themselves as compile errors in
        dnl the first ROSE source that happens to include boost thread support, usually something in the
        dnl src/util/Sawyer directory.
        if test "$rose_boost_version" = 105400 -a \
                "$FRONTEND_CXX_COMPILER_VENDOR" = "gnu" -a \
                "$FRONTEND_CXX_VERSION_MAJOR" = 4 -a \
                "$FRONTEND_CXX_VERSION_MINOR" -ge 8; then
            prohibited="boost 1.54 with gcc >= 4.8 and gcc < 5"
            break
        fi

        dnl Boost-1.61 through 1.63 serialization cause link errors when compiled by Intel compilers. Binary analysis
        dnl is the only thing that uses serialization at this time.
        if test "$rose_boost_version" -ge 106100 -a "$rose_boost_version" -le 106300 -a \
                "$FRONTEND_CXX_COMPILER_VENDOR" = "intel" -a \
                "$support_binaries_frontend" = "yes"; then
            prohibited="binary analysis enabled with boost 1.62 with Intel compiler"
            break
        fi

        dnl Add more blacklist items right above this line with comments like the others. If you detect
        dnl a combination that's blacklisted, then set the "prohibited" string to say what is blacklisted
        dnl and then break out of the loop to avoid testing other combinations.
        : whatever tests you like

        dnl If we made it this far then this combination is not blacklisted
        prohibited=
        break
    done

    dnl -----------------------
    dnl    Output results
    dnl -----------------------
    if test "$prohibited" != ""; then
        AC_MSG_ERROR([
            The following combination of ROSE software dependencies is blacklisted. The ROSE
            team occassionally blacklists combinations that are known to cause failures in
            perhaps subtle ways. If you would like to continue with this configuration at your
            own risk, then reconfigure with --disable-blacklist. (This message comes from
            config/support-blacklist.m4.)])

        AC_ARG_ENABLE([blacklist],
            AS_HELP_STRING([--disable-blacklist],
                           [Disabling the blacklist check causes blacklist related messages to become non-fatal.
                            The default --enable-blacklist, which means trying to use a blacklisted configuration is fatal.]),
            [should_die="$enableval"], [should_die=yes])
        if test "$should_die" = "yes"; then
            AC_MSG_FAILURE([blacklisted: $prohibited])
        else
            AC_MSG_WARN([blacklisted: $prohibited])
            AC_MSG_WARN([blacklisted, but user chose to continue at their own risk])
        fi
    fi
])
