dnl Support for gracefully bailing out of configure for certain combinations of dependency versions
dnl or other situations.

AC_DEFUN([ROSE_SUPPORT_BLACKLIST],[
    prohibited=

    dnl -----------------------
    dnl    Versions we HAVE
    dnl -----------------------

    dnl Debugging black lists... Spit out the values that will be needed by users when they're
    dnl trying to figure out why their configuration is black listed. These are the versions they
    dnl HAVE. The versions that are PROHIBITED will be emitted later in this function.  The version
    dnl number variables should be set wherever we're detecting the dependency, not here.
    AC_MSG_NOTICE([boost version     = $rose_boost_version])
    AC_MSG_NOTICE([frontend compiler = $FRONTEND_CXX_COMPILER_VENDOR-$FRONTEND_CXX_VERSION_MAJOR.$FRONTEND_CXX_VERSION_MINOR])

    dnl -----------------------
    dnl  Versions we PROHIBIT
    dnl -----------------------

    while true; do
        dnl Boost 1.54 has numerous bugs related to threads. These manifest themselves as compile errors in
        dnl the first ROSE source that happens to include boost thread support, usually something in the
        dnl src/util/Sawyer directory.
	if test "$rose_boost_version" = 105400 -a \
	        "$FRONTEND_CXX_COMPILER_VENDOR" = "gcc" -a \
	        "$FRONTEND_CXX_VERSION_MAJOR" = 4 -a \
		"$FRONTEND_CXX_VERSION_MINOR" -ge 8; then
	    prohibited="boost 1.54 with gcc >= 4.8 and gcc < 5"
            break
        fi

        dnl Add more black list items right above this line with comments like the others. If you detect
        dnl a combination that's black listed, then set the "prohibited" string to say what is black listed
        dnl and then break out of the loop to avoid testing other combinations.
        : whatever tests you like

        dnl If we made it this far then this combination is not black listed
        break
    done

    dnl -----------------------
    dnl    Output results
    dnl -----------------------
    if test "$prohibited" != ""; then
       AC_MSG_NOTICE([
           The following combination of ROSE software dependencies is black listed.
           The ROSE team occassionally black lists combinations that are known to
           cause failures in perhaps subtle ways. (From support-blacklist.m4)])
       AC_MSG_FAILURE([black listed: $prohibited])
    fi
])
