AC_DEFUN([ROSE_SUPPORT_GNAT],
[
# Begin macro ROSE_SUPPORT_GNAT.

# Inclusion of test for GNAT ADA Compiler Support and it's location.

AC_MSG_CHECKING(for gnat)
AC_ARG_WITH(gnat,
[  --with-gnat=PATH	Specify the prefix where GNAT support is installed],
,
if test ! "$with_gnat" ; then
   with_gnat=no
fi
)

AC_MSG_NOTICE([in ROSE SUPPORT MACRO: with_gnat = "$with_gnat"])

if test "$with_gnat" = no; then
   # If gnat is not specified, then don't use it.
   AC_MSG_NOTICE([skipping use of GNAT])
else
   gnat_path=$with_gnat
   AC_MSG_NOTICE([GNAT supported in ROSE: gnat path = "$gnat_path"])

 # GNAT_SYSTEM_TYPE="`$gnat_path/bin/gnat.system.type`"
 # GNAT_LIBDIR="$gnat_path/$GNAT_SYSTEM_TYPE"
 # GNAT_EXTRA_LIBS="-L$GNAT_LIBDIR -Wl,-rpath,$GNAT_LIBDIR -lrt"
 # AC_CHECK_LIB(gnatc,StartGNAT,[:],[AC_ERROR([Unable to link to GNAT!])],[$GNAT_EXTRA_LIBS])

 # GNAT_INCLUDES="-I$gnat_path/include"
   GNAT_INCLUDES="-I$gnat_path"
 # GNAT may includes its own old copy of libstdc++ in its bin directory so we need to override this with the system provided one
 # GNAT_LIBS="-Wl,-rpath,/usr/lib -lgnatc $GNAT_EXTRA_LIBS"
   AC_SUBST(GNAT_INCLUDES)
 # AC_SUBST(GNAT_LIBS)
 # AC_SUBST(GNAT_LIBDIR)

   AC_DEFINE([USE_ROSE_GNAT_SUPPORT],1,[Controls use of ROSE support for GNAT ADA frontend Support.])
fi

AC_SUBST(gnat_path)

# End macro ROSE_SUPPORT_GNAT.
]
)
