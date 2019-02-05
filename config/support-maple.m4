AC_DEFUN([ROSE_SUPPORT_MAPLE],
[
# Begin macro ROSE_SUPPORT_MAPLE.

# Inclusion of test for Maple Symbolic Algebra Package and it's location.

AC_MSG_CHECKING(for maple)
AC_ARG_WITH(maple,
[  --with-maple=PATH	Specify the prefix where Maple is installed],
,
if test ! "$with_maple" ; then
   with_maple=no
fi
)

AC_MSG_NOTICE([in ROSE SUPPORT MACRO: with_maple = "$with_maple"])

if test "$with_maple" = no; then
   # If maple is not specified, then don't use it.
   AC_MSG_NOTICE([skipping use of Maple])
else
   maple_path=$with_maple
   AC_MSG_NOTICE([setup Maple support in ROSE. path = "$maple_path"])

   MAPLE_SYSTEM_TYPE="`$maple_path/bin/maple.system.type`"
   MAPLE_LIBDIR="$maple_path/$MAPLE_SYSTEM_TYPE"
   MAPLE_EXTRA_LIBS="-L$MAPLE_LIBDIR -Wl,-rpath,$MAPLE_LIBDIR -lrt"
   AC_CHECK_LIB(maplec,StartMaple,[:],[AC_ERROR([Unable to link to Maple!])],[$MAPLE_EXTRA_LIBS])

   MAPLE_INCLUDES="-I$maple_path/extern/include"
   # Maple includes its own old copy of libstdc++ in its bin directory so we need to override this with the system provided one
   MAPLE_LIBS="-Wl,-rpath,/usr/lib -lmaplec $MAPLE_EXTRA_LIBS"
   AC_SUBST(MAPLE_INCLUDES)
   AC_SUBST(MAPLE_LIBS)

   # DQ (10/27/2014): Added lib directory.
   AC_SUBST(MAPLE_LIBDIR)

   AC_DEFINE([USE_ROSE_MAPLE_SUPPORT],1,[Controls use of ROSE support for Maple Symbolic Algebra Package.])
fi

AC_SUBST(maple_path)

# End macro ROSE_SUPPORT_MAPLE.
]
)
