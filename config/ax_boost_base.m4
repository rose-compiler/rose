##### http://autoconf-archive.cryp.to/ax_boost_base.html
#
# SYNOPSIS
#
#   AX_BOOST_BASE([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   Test for the Boost C++ libraries of a particular version (or newer)
#
#   If no path to the installed boost library is given the macro
#   searchs under /usr, /usr/local, /opt and /opt/local and evaluates
#   the $BOOST_ROOT environment variable. Further documentation is
#   available at <http://randspringer.de/boost/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(BOOST_CPPFLAGS) / AC_SUBST(BOOST_LDFLAGS)
#
#   And sets:
#
#     HAVE_BOOST
#
# COPYLEFT
#
#   Copyright (c) 2008 Thomas Porschberg <thomas@randspringer.de>
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([AX_BOOST_BASE],
[
AC_ARG_WITH([boost],
	AS_HELP_STRING([--with-boost@<:@=DIR@:>@], [use boost (default is yes) - it is possible to specify the root directory for boost (optional)]),
	[
    if test "$withval" = "no"; then
		want_boost="no"
    elif test "$withval" = "yes"; then
        want_boost="yes"
        ac_boost_path=""
    else
	    want_boost="yes"
        ac_boost_path="$withval"
	fi
    ],
    [want_boost="yes"])


AC_ARG_WITH([boost-libdir],
        AS_HELP_STRING([--with-boost-libdir=LIB_DIR],
        [Force given directory for boost libraries. Note that this will overwrite library path detection, so use this parameter only if default library detection fails and you know exactly where your boost libraries are located.]),
        [
        if test -d $withval
        then
                ac_boost_lib_path="$withval"
        else
                AC_MSG_ERROR([--with-boost-libdir expected directory name])
        fi
        ],
        [ac_boost_lib_path=""]
)

if test "x$want_boost" = "xyes"; then
	boost_lib_version_req=ifelse([$1], ,1.20.0,$1)
	boost_lib_version_req_shorten=`expr $boost_lib_version_req : '\([[0-9]]*\.[[0-9]]*\)'`
	boost_lib_version_req_major=`expr $boost_lib_version_req : '\([[0-9]]*\)'`
	boost_lib_version_req_minor=`expr $boost_lib_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
	boost_lib_version_req_sub_minor=`expr $boost_lib_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
	if test "x$boost_lib_version_req_sub_minor" = "x" ; then
		boost_lib_version_req_sub_minor="0"
    	fi
	WANT_BOOST_VERSION=`expr $boost_lib_version_req_major \* 100000 \+  $boost_lib_version_req_minor \* 100 \+ $boost_lib_version_req_sub_minor`
	AC_MSG_CHECKING(for boostlib >= $boost_lib_version_req)
	succeeded=no

 # DQ (2/4/2010): Added to support setting Automake conditionals for boost versions.
 # export boost_lib_version_req_major
 # export boost_lib_version_req_minor
 # export boost_lib_version_req_sub_minor

 # echo "In ax boost base macro: boost_lib_version_req_major     = $boost_lib_version_req_major"
 # echo "In ax boost base macro: boost_lib_version_req_minor     = $boost_lib_version_req_minor"
 # echo "In ax boost base macro: boost_lib_version_req_sub_minor = $boost_lib_version_req_sub_minor"

	dnl first we check the system location for boost libraries
	dnl this location ist chosen if boost libraries are installed with the --layout=system option
	dnl or if you install boost with RPM
	if test "$ac_boost_path" != ""; then
		BOOST_LDFLAGS=" -L$ac_boost_path/lib"
		BOOST_CPPFLAGS="-I$ac_boost_path/include"
                ROSE_BOOST_INCLUDE_PATH="$ac_boost_path/include"
	else
		for ac_boost_path_tmp in /usr /usr/local /opt /opt/local ; do
			if test -d "$ac_boost_path_tmp/include/boost" && test -r "$ac_boost_path_tmp/include/boost"; then
				BOOST_LDFLAGS="-L$ac_boost_path_tmp/lib"
				BOOST_CPPFLAGS="-I$ac_boost_path_tmp/include"
                                ROSE_BOOST_INCLUDE_PATH="$ac_boost_path_tmp/include"
				break;
			fi
		done
	fi

# echo "WANT_BOOST_VERSION = $WANT_BOOST_VERSION"
# echo "boost_lib_version_req = $boost_lib_version_req"
# echo "BOOST_LDFLAGS  = $BOOST_LDFLAGS"
# echo "BOOST_CPPFLAGS = $BOOST_CPPFLAGS"

# DQ (1/1/2009): Set the default value based on BOOST_CPPFLAGS
# Liao (1/12/2009): ac_boost_path_tmp is one of the if's branches. 
# Moved the assignment into the two branches above
#   ROSE_BOOST_INCLUDE_PATH="$ac_boost_path_tmp/include"

 # DQ (1/1/2009): Added testing for previously installed Boost (always older version)
 # so that we can trigger the use of "-isystem" option (to g++) only when required
 # (it appears to be a problem for SWIG).
 # Use this set of paths, and the set including "/home/dquinlan" for testing this macro.
 # for ac_boost_path_tmp in /usr /usr/local /opt /opt/local /home/dquinlan; do
   for ac_boost_path_tmp in /usr /usr/local /opt /opt/local ; do
		if test "x${ac_boost_path_tmp}/include" != "x${ROSE_BOOST_INCLUDE_PATH}" && test -d "$ac_boost_path_tmp/include/boost" && test -r "$ac_boost_path_tmp/include/boost"; then
			PREVIOUSLY_INSTALLED_BOOST="$ac_boost_path_tmp/include/boost"
                        AC_MSG_WARN([detected a previously installed version of boost library: "$PREVIOUSLY_INSTALLED_BOOST"])
			break;
		fi
	done

    dnl overwrite ld flags if we have required special directory with
    dnl --with-boost-libdir parameter
    if test "$ac_boost_lib_path" != ""; then
       BOOST_LDFLAGS="-L$ac_boost_lib_path"
    fi

	CPPFLAGS_SAVED="$CPPFLAGS"
	CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
	export CPPFLAGS

	LDFLAGS_SAVED="$LDFLAGS"
	LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
	export LDFLAGS

	AC_LANG_PUSH(C++)
     	AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
	@%:@include <boost/version.hpp>
	]], [[
	#if BOOST_VERSION >= $WANT_BOOST_VERSION
	// Everything is okay
	#else
	#  error Boost version is too old
	#endif
	]])],[
        AC_MSG_RESULT(yes)
	succeeded=yes
	found_system=yes
       	],[
       	])
	AC_LANG_POP([C++])

	dnl if we found no boost with system layout we search for boost libraries
	dnl built and installed without the --layout=system option or for a staged(not installed) version
	if test "x$succeeded" != "xyes"; then
		_version=0
		if test "$ac_boost_path" != ""; then
			if test -d "$ac_boost_path" && test -r "$ac_boost_path"; then
				for i in `ls -d $ac_boost_path/include/boost-* 2>/dev/null`; do
					_version_tmp=`echo $i | sed "s#$ac_boost_path##" | sed 's/\/include\/boost-//' | sed 's/_/./'`
					V_CHECK=`expr $_version_tmp \> $_version`
					if test "$V_CHECK" = "1" ; then
						_version=$_version_tmp
					fi
					VERSION_UNDERSCORE=`echo $_version | sed 's/\./_/'`
					BOOST_CPPFLAGS="-I$ac_boost_path/include/boost-$VERSION_UNDERSCORE"
             # DQ (12/22/2008): Modified macro to save the boost path so that it could be used with "-isystem" option (gcc).
					ROSE_BOOST_INCLUDE_PATH="$ac_boost_path/include/boost-$VERSION_UNDERSCORE"
				done
			fi
		else
			for ac_boost_path in /usr /usr/local /opt /opt/local ; do
				if test -d "$ac_boost_path" && test -r "$ac_boost_path"; then
					for i in `ls -d $ac_boost_path/include/boost-* 2>/dev/null`; do
						_version_tmp=`echo $i | sed "s#$ac_boost_path##" | sed 's/\/include\/boost-//' | sed 's/_/./'`
						V_CHECK=`expr $_version_tmp \> $_version`
						if test "$V_CHECK" = "1" ; then
							_version=$_version_tmp
	               					best_path=$ac_boost_path
						fi
					done
				fi
			done

			VERSION_UNDERSCORE=`echo $_version | sed 's/\./_/'`
			BOOST_CPPFLAGS="-I$best_path/include/boost-$VERSION_UNDERSCORE"
            if test "$ac_boost_lib_path" = ""
            then
               BOOST_LDFLAGS="-L$best_path/lib"
            fi

	    		if test "x$BOOST_ROOT" != "x"; then
				if test -d "$BOOST_ROOT" && test -r "$BOOST_ROOT" && test -d "$BOOST_ROOT/stage/lib" && test -r "$BOOST_ROOT/stage/lib"; then
					version_dir=`expr //$BOOST_ROOT : '.*/\(.*\)'`
					stage_version=`echo $version_dir | sed 's/boost_//' | sed 's/_/./g'`
			        	stage_version_shorten=`expr $stage_version : '\([[0-9]]*\.[[0-9]]*\)'`
					V_CHECK=`expr $stage_version_shorten \>\= $_version`
                    if test "$V_CHECK" = "1" -a "$ac_boost_lib_path" = "" ; then
						AC_MSG_NOTICE([will use a staged boost library from $BOOST_ROOT])
						BOOST_CPPFLAGS="-I$BOOST_ROOT"
						BOOST_LDFLAGS="-L$BOOST_ROOT/stage/lib"
					fi
				fi
	    		fi
		fi

    # DQ (12/22/2008): Fixup Boost to not use the system (OS) installation of Boost
    # BOOST_CPPFLAGS="$BOOST_CPPFLAGS/boost"

		CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
		export CPPFLAGS
		LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"
		export LDFLAGS

		AC_LANG_PUSH(C++)
	     	AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
		@%:@include <boost/version.hpp>
		]], [[
		#if BOOST_VERSION >= $WANT_BOOST_VERSION
		// Everything is okay
		#else
		#  error Boost version is too old
		#endif
		]])],[
        	AC_MSG_RESULT(yes)
		succeeded=yes
		found_system=yes
       		],[
	       	])
		AC_LANG_POP([C++])
	fi

 # DQ (2/4/2010): Added to get version information for what we are using.
   AC_MSG_NOTICE([in boost base macro -- Boost version being used is: "$_version"])
 # rose_boost_version_number=$_version
 # export rose_boost_version_number

	if test "$succeeded" != "yes" ; then
		if test "$_version" = "0" ; then
       # This was always a confusing error message so make it more explicit for users.
       # AC_MSG_ERROR([[We could not detect the boost libraries (version $boost_lib_version_req_shorten or higher). If you have a staged boost library (still not installed) please specify \$BOOST_ROOT in your environment and do not give a PATH to --with-boost option.  If you are sure you have boost installed, then check your version number looking in <boost/version.hpp>. See http://randspringer.de/boost for more documentation.]])
			AC_MSG_ERROR([Boost libraries (version $boost_lib_version_req_shorten or higher) must be specified on the configure line (using the --with-boost switch) and the boost libraries must be in your LD_LIBRARY_PATH])
		else
			AC_MSG_FAILURE([your boost libraries seems to old (version $_version)])
		fi
	else
		AC_SUBST(BOOST_CPPFLAGS)
		AC_SUBST(BOOST_LDFLAGS)
		AC_DEFINE(HAVE_BOOST,,[define if the Boost library is available])
	fi

        CPPFLAGS="$CPPFLAGS_SAVED"
       	LDFLAGS="$LDFLAGS_SAVED"
fi

# DQ (12/22/2008): Modified macro to save the boost path so that it could be used with "-isystem" 
# option to include the boost path specified on the configure command ahead of "/usr/local/include" 
# so that we can get the required version of Boost on systems that have it installed by default.
# echo "Final Test: ROSE_BOOST_INCLUDE_PATH = $ROSE_BOOST_INCLUDE_PATH"
# AC_SUBST(ROSE_BOOST_INCLUDE_PATH)

# DQ (1/1/2009): This use of "-isystem" is not triggered only when there is
# a previously installed version of ROSE detected (e.g. in /usr/liclude/boost).
# Note that use of "-isystem" option with g++ will cause SWIG to fail.
if test "$PREVIOUSLY_INSTALLED_BOOST" != ""; then
 # echo "Using the -isystem option of g++ to force the use of the specified version of Boost ahead of a previously installed version of boost on your system at: $PREVIOUSLY_INSTALLED_BOOST"
   AC_MSG_NOTICE(using the -isystem option of g++ to force the use of the specified version of Boost ahead of a previously installed version of boost on your system at: $PREVIOUSLY_INSTALLED_BOOST)
   AC_MSG_WARN([--with-javaport cannot be used with the -isystem option])
   ROSE_BOOST_PREINCLUDE_PATH="-isystem $ROSE_BOOST_INCLUDE_PATH"
   ROSE_BOOST_NORMAL_INCLUDE_PATH=""
   AC_MSG_WARN([detected previously installed version of boost (please remove older version of Boost before installing ROSE); continuing but expect Boost to be a problem])
 # AC_MSG_ERROR([[Detected previously installed version of boost (please remove older version of Boost before installing ROSE) (continuing but expect Boost to be a problem...)]])
 # Remove this exit (as a test) after detecting what I expect is a problem...
 # exit 1
else
   AC_MSG_NOTICE(no previously installed version of boost detected: using boost include directories with normal -I option)
   ROSE_BOOST_PREINCLUDE_PATH=""
   ROSE_BOOST_NORMAL_INCLUDE_PATH="-I$ROSE_BOOST_INCLUDE_PATH"
fi

AC_SUBST(ROSE_BOOST_PREINCLUDE_PATH)
AC_SUBST(ROSE_BOOST_NORMAL_INCLUDE_PATH)


])
