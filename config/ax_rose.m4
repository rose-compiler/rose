##### http://autoconf-archive.cryp.to/ax_boost_base.html
#
# SYNOPSIS
#
#   AX_ROSE([MINIMUM-VERSION])
#
# DESCRIPTION
#
#   Test for the ROSE C++ libraries of a particular version (or newer)
#
#   If no path to the installed rose library is given the macro
#   searchs under /usr, /usr/local, /opt and /opt/local and evaluates
#   the $ROSE_ROOT environment variable. Further documentation is
#   available at <http://randspringer.de/rose/index.html>.
#
#   This macro calls:
#
#     AC_SUBST(ROSE_CPPFLAGS) / AC_SUBST(ROSE_LDFLAGS)
#
#   And sets:
#
#     HAVE_ROSE
#
# COPYLEFT
#
#   Copyright (c) 2008 Thomas Porschberg <thomas@randspringer.de>
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([AX_ROSE],
[
AC_ARG_WITH([rose],
	AS_HELP_STRING([--with-rose@<:@=DIR@:>@], [use rose (default is yes) - it is possible to specify the root directory for rose (optional)]),
	[
    if test "$withval" = "no"; then
		want_rose="no"
    elif test "$withval" = "yes"; then
        want_rose="yes"
        ac_rose_path=""
    else
	    want_rose="yes"
        ac_rose_path="$withval"
	fi
    ],
    [want_rose="yes"])


AC_ARG_WITH([rose-libdir],
        AS_HELP_STRING([--with-rose-libdir=LIB_DIR],
        [Force given directory for rose libraries. Note that this will overwrite library path detection, so use this parameter only if default library detection fails and you know exactly where your rose libraries are located.]),
        [
        if test -d $withval
        then
                ac_rose_lib_path="$withval"
        else
                AC_MSG_ERROR(--with-rose-libdir expected directory name)
        fi
        ],
        [ac_rose_lib_path=""]
)

if test "x$want_rose" = "xyes"; then
	rose_lib_version_req=ifelse([$1], ,1.20.0,$1)
	rose_lib_version_req_shorten=`expr $rose_lib_version_req : '\([[0-9]]*\.[[0-9]]*\)'`
	rose_lib_version_req_major=`expr $rose_lib_version_req : '\([[0-9]]*\)'`
	rose_lib_version_req_minor=`expr $rose_lib_version_req : '[[0-9]]*\.\([[0-9]]*\)'`
	rose_lib_version_req_sub_minor=`expr $rose_lib_version_req : '[[0-9]]*\.[[0-9]]*\.\([[0-9]]*\)'`
	if test "x$rose_lib_version_req_sub_minor" = "x" ; then
		rose_lib_version_req_sub_minor="0"
    	fi
	WANT_ROSE_VERSION=90200
	AC_MSG_CHECKING(for roselib >= $rose_lib_version_req)
	succeeded=yes

	dnl first we check the system location for rose libraries
	dnl this location ist chosen if rose libraries are installed with the --layout=system option
	dnl or if you install rose with RPM
	if test "$ac_rose_path" != ""; then
		ROSE_LDFLAGS="-L$ac_rose_path/lib"
		ROSE_CPPFLAGS="-I$ac_rose_path/include"
	else
		for ac_rose_path_tmp in /usr /usr/local /opt /opt/local ; do
			if test -d "$ac_rose_path_tmp/include/rose" && test -r "$ac_rose_path_tmp/include/rose"; then
				ROSE_LDFLAGS="-L$ac_rose_path_tmp/lib"
				ROSE_CPPFLAGS="-I$ac_rose_path_tmp/include"
				break;
			fi
		done
	fi

    dnl overwrite ld flags if we have required special directory with
    dnl --with-rose-libdir parameter
    if test "$ac_rose_lib_path" != ""; then
       ROSE_LDFLAGS="-L$ac_rose_lib_path"
    fi

	CPPFLAGS_SAVED="$CPPFLAGS"
	CPPFLAGS="$CPPFLAGS $ROSE_CPPFLAGS"
	export CPPFLAGS

	LDFLAGS_SAVED="$LDFLAGS"
	LDFLAGS="$LDFLAGS $ROSE_LDFLAGS"
	export LDFLAGS

	dnl if we found no rose with system layout we search for rose libraries
	dnl built and installed without the --layout=system option or for a staged(not installed) version
	if test "x$succeeded" != "xyes"; then
		_version=0
		if test "$ac_rose_path" != ""; then
			if test -d "$ac_rose_path" && test -r "$ac_rose_path"; then
				for i in `ls -d $ac_rose_path/include/rose-* 2>/dev/null`; do
					_version_tmp=`echo $i | sed "s#$ac_rose_path##" | sed 's/\/include\/rose-//' | sed 's/_/./'`
					V_CHECK=`expr $_version_tmp \> $_version`
					if test "$V_CHECK" = "1" ; then
						_version=$_version_tmp
					fi
					VERSION_UNDERSCORE=`echo $_version | sed 's/\./_/'`
					ROSE_CPPFLAGS="-I$ac_rose_path/include/rose-$VERSION_UNDERSCORE"
				done
			fi
		else
			for ac_rose_path in /usr /usr/local /opt /opt/local ; do
				if test -d "$ac_rose_path" && test -r "$ac_rose_path"; then
					for i in `ls -d $ac_rose_path/include/rose-* 2>/dev/null`; do
						_version_tmp=`echo $i | sed "s#$ac_rose_path##" | sed 's/\/include\/rose-//' | sed 's/_/./'`
						V_CHECK=`expr $_version_tmp \> $_version`
						if test "$V_CHECK" = "1" ; then
							_version=$_version_tmp
	               					best_path=$ac_rose_path
						fi
					done
				fi
			done

			VERSION_UNDERSCORE=`echo $_version | sed 's/\./_/'`
			ROSE_CPPFLAGS="-I$best_path/include/rose-$VERSION_UNDERSCORE"
            if test "$ac_rose_lib_path" = ""
            then
               ROSE_LDFLAGS="-L$best_path/lib"
            fi

	    		if test "x$ROSE_ROOT" != "x"; then
				if test -d "$ROSE_ROOT" && test -r "$ROSE_ROOT" && test -d "$ROSE_ROOT/stage/lib" && test -r "$ROSE_ROOT/stage/lib"; then
					version_dir=`expr //$ROSE_ROOT : '.*/\(.*\)'`
					stage_version=`echo $version_dir | sed 's/rose_//' | sed 's/_/./g'`
			        	stage_version_shorten=`expr $stage_version : '\([[0-9]]*\.[[0-9]]*\)'`
					V_CHECK=`expr $stage_version_shorten \>\= $_version`
                    if test "$V_CHECK" = "1" -a "$ac_rose_lib_path" = "" ; then
						AC_MSG_NOTICE([will use a staged rose library from "$ROSE_ROOT"])
						ROSE_CPPFLAGS="-I$ROSE_ROOT"
						ROSE_LDFLAGS="-L$ROSE_ROOT/stage/lib"
					fi
				fi
	    		fi
		fi

		CPPFLAGS="$CPPFLAGS $ROSE_CPPFLAGS"
		export CPPFLAGS
		LDFLAGS="$LDFLAGS $ROSE_LDFLAGS"
		export LDFLAGS

	fi

	if test "$succeeded" != "yes" ; then
		if test "$_version" = "0" ; then
			AC_MSG_ERROR([[We could not detect the rose libraries (version $rose_lib_version_req_shorten or higher). If you have a staged rose library (still not installed) please specify \$ROSE_ROOT in your environment and do not give a PATH to --with-rose option.  If you are sure you have rose installed, then check your version number looking in <rose_paths.h>. See http://randspringer.de/rose for more documentation.]])
		else
			AC_MSG_NOTICE([your rose libraries seems to old (version $_version)])
		fi
	else
		AC_SUBST(ROSE_CPPFLAGS)
		AC_SUBST(ROSE_LDFLAGS)
		AC_DEFINE(HAVE_ROSE,,[define if the ROSE library is available])
	fi

        CPPFLAGS="$CPPFLAGS_SAVED"
       	LDFLAGS="$LDFLAGS_SAVED"
fi

])
