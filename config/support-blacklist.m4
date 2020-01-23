# Support for gracefully bailing out of configure for certain combinations of dependency versions
# or other situations.

AC_DEFUN([ROSE_SUPPORT_BLACKLIST],[
    prohibited=

    # -----------------------
    #    Versions we HAVE
    # -----------------------

    # Debugging blacklists... Spit out the values that will be needed by users when they're
    # trying to figure out why their configuration is blacklisted. These are the versions they
    # HAVE. The versions that are PROHIBITED will be emitted later in this function.  The version
    # number variables should be set wherever we're detecting the dependency, not here.
    ROSE_SUPPORT_SUMMARY

    # -----------------------
    #  Versions we PROHIBIT
    # -----------------------

    ROSE_CONFIGURE_SECTION([Checking blacklisted configurations])
    while true; do
        # Boost 1.54 has numerous bugs related to threads. These manifest themselves as compile errors in
        # the first ROSE source that happens to include boost thread support, usually something in the
        # src/util/Sawyer directory.
        if test "$rose_boost_version" = 105400 -a \
                "$FRONTEND_CXX_COMPILER_VENDOR" = "gnu" -a \
                "$FRONTEND_CXX_VERSION_MAJOR" = 4 -a \
                "$FRONTEND_CXX_VERSION_MINOR" -ge 8; then
            prohibited="boost 1.54 with gcc >= 4.8 and gcc < 5"
            break
        fi

        # Boost-1.61 through 1.63 serialization cause link errors when compiled by Intel compilers. Binary analysis
        # is the only thing that uses serialization at this time.
        if test "$rose_boost_version" -ge 106100 -a "$rose_boost_version" -le 106300 -a \
                "$FRONTEND_CXX_COMPILER_VENDOR" = "intel" -a \
                "$support_binaries_frontend" = "yes"; then
            prohibited="binary analysis enabled with boost 1.62 with Intel compiler"
            break
        fi

        # Boost-1.68.0 has serialization bugs reported by numerous other projects. Within ROSE, 1.68.0 fails the
        # basic boost serialization tests that we've written. These tests don't depend on ROSE, rather link only
        # with boost.
        if test "$rose_boost_version" = 106800; then
            prohibited="boost-1.68.0"
            break
        fi

        # Boost versions prior to 1.68.0 do not work with C++17 and GNU++17 compilers. They experience compiler
        # errors when a program tries to use the two-argument version of boost::lock, which is needed by ROSE for
        # thread synchronization.
        if test "$rose_boost_version" -le 106800; then
            if test "$HOST_CXX_LANGUAGE" = "c++17" -o "$HOST_CXX_LANGUAGE" = "gnu++17"; then
                prohibited="boost-$rose_boost_version with $HOST_CXX_LANGUAGE (problems with boost::lock)"
                break
            fi
        fi

        # Boost versions 1.69.0 and later are not compatible with the edg binaries generated as of January 2020. 
        # Any bost 1.69.0 and later need to build ROSE from edg source.
        if test "$rose_boost_version" -ge 106900; then
            if test "$has_edg_source" = "no"; then
                prohibited="boost-1.69.0 and later are not compatible with EDG binary"
                break
            fi
        fi

        # Z3 4.7.1 is blacklisted because it introduced an API change without any easy way to detect the change
        # at compile time.
        if test "$Z3_VERSION" = "4.7.1.0"; then
            prohibited="z3-4.7.1"
            break
        fi

        # Add more blacklist items right above this line with comments like the others. If you detect
        # a combination that's blacklisted, then set the "prohibited" string to say what is blacklisted
        # and then break out of the loop to avoid testing other combinations.
        : whatever tests you like

        # If we made it this far then this combination is not blacklisted
        prohibited=
        break
    done

    # -----------------------
    #    Output results
    # -----------------------
    AC_ARG_ENABLE([blacklist],
        AS_HELP_STRING([--disable-blacklist],
                       [Disabling the blacklist check causes blacklist related messages to become non-fatal.
                        The default --enable-blacklist, means trying to use a blacklisted configuration is fatal.]),
        [should_die="$enableval"], [should_die=yes])

    if test "$prohibited" != ""; then
        if test "$should_die" != no; then
            AC_MSG_FAILURE([blacklisted: $prohibited
                The above combination of ROSE software dependencies is blacklisted. The ROSE
                team occassionally blacklists combinations that are known to cause failures in
                perhaps subtle ways. If you would like to continue with this configuration at your
                own risk, then reconfigure with --disable-blacklist. (This message comes from
                config/support-blacklist.m4, which also contains more information about the reason
                for blacklisting.)])
        else
            AC_MSG_WARN([blacklisted: $prohibited])
            AC_MSG_WARN([    continuing with unsafe configuration at user's request (--disable-blacklist)])
        fi
    fi
])
