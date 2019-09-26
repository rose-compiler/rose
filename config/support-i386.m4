dnl  Tests whether the system compiler supports generating 32-bit x86 executables. These executables are typically
dnl  used as inputs for testing ROSE's binary support.

AC_DEFUN([ROSE_SUPPORT_I386],[
	AC_MSG_CHECKING([for i386 support (see --enable-i386)])
	
	AC_ARG_ENABLE([i386],
		      [AC_HELP_STRING([--enable-i386],
				      [If explicitly enabled, the build system will attempt to create 32-bit x86
				       programs for use as input to ROSE binary support testing and those tests
				       will be required to pass.  If explicitly disabled, 32-bit x86 support is
				       not required and the affected tests will not run.  If unspecified, 32-bit
				       x86 support will be turned on or off based on the capabilities of the
				       environment.])],
		      [RSI386_SPECIFIED=yes; ac_cv_enable_i386="$enableval"],
		      [RSI386_SPECIFIED=no])

	dnl Create a very simple source file that we'll try to compile various ways
	RSI386_TEMP=/tmp/rose_support_i386_$$
	echo "int main() {return 0;}" >$RSI386_TEMP.c

	if test "$RSI386_SPECIFIED" = "no" -o "$ac_cv_enable_i386" = "yes"; then
		dnl If the user specified neither "--enable-i386" nor "--disable-i386". Try to determine if we can
		dnl generate a 32-bit x86 program...
		ac_cv_enable_i386=no

		dnl Method 1: Perhaps the compiler creates this kind of executable by default?
		if (test "$ac_cv_enable_i386" = no &&
		    $CC $CFLAGS -o $RSI386_TEMP $RSI386_TEMP.c &&
		    $RSI386_TEMP &&
		    file $RSI386_TEMP |grep 32-bit |grep -q Intel); then
			$ac_cv_enable_i386=
		fi

		dnl Method 2: Perhaps the "-m32" switch will work
		if (test "$ac_cv_enable_i386" = no &&
		    $CC -m32 $CFLAGS -o $RSI386_TEMP $RSI386_TEMP.c &&
		    $RSI386_TEMP &&
                    file $RSI386_TEMP |grep 32-bit |grep -q Intel); then
			ac_cv_enable_i386="-m32"
		fi

		dnl It is an error if --enable-i386 was specified but we could not figure out how to generate such a file.
		if test "$RSI386_SPECIFIED" = "yes" -a "$ac_cv_enable_i386" = "no"; then
			AC_MSG_ERROR([--enable-i386 was specified but common switches to produce such a file don't work])
		fi

	elif test "$ac_cv_enable_i386" != "no"; then
		dnl Check that what the user specified actually dos work...
		if ($CC $ac_cv_enable_i386 $CFLAGS -o $RSI386_TEMP $RSI386_TEMP.c &&
		    $RSI386_TEMP &&
		    file $RSI386_TEMP |grep 32-bit |grep -q Intel); then
			: all ok
		else
			AC_MSG_RESULT($ac_cv_enable_i386)
			AC_MSG_ERROR([user-specified --enable-i386 value ($ac_cv_enable_i386) does not work properly])
		fi
	fi

	AC_MSG_RESULT($ac_cv_enable_i386)
	rm -f $RSI386_TEMP $RSI386_TEMP.c $RSI386_TEMP.o

	dnl Result variables
	AM_CONDITIONAL(ENABLE_I386, [test "$ac_cv_enable_i386" != no])
	CFLAGS32="$ac_cv_enable_i386"
	AC_SUBST(CFLAGS32)
])
