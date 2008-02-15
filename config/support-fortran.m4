AC_DEFUN([ROSE_SUPPORT_FORTRAN],
[
# Begin macro ROSE_SUPPORT_FORTRAN.

# Inclusion of test for Maple Symbolic Algebra Package and it's location.

AC_MSG_CHECKING(for OpenFortranParser)
AC_ARG_WITH(openFortranParser,
[  --with-openFortranParser=PATH	Specify the prefix where OpenFortranParser is installed],
,
if test ! "$with_openFortranParser" ; then
   with_openFortranParser=no
fi
)

echo "In ROSE SUPPORT MACRO: with_openFortranParser $with_openFortranParser"

if test "$with_openFortranParser" = no; then
   # If OpenFortranParser is not specified, then don't use it.
   echo "Skipping use of openFortranParser (and Fortran2003 support)!"
else
   OPEN_FORTRAN_PARSER_PATH=$with_openFortranParser
   echo "Setup OpenFortranParser support in ROSE! path = $OPEN_FORTRAN_PARSER_PATH"
   AC_DEFINE([USE_ROSE_OPEN_FORTRAN_PARSER_SUPPORT],1,[Controls use of ROSE support for Fortran 2003 (using the Open Fortran Parser from LANL).])
fi

AC_SUBST(OPEN_FORTRAN_PARSER_PATH)

# End macro ROSE_SUPPORT_FORTRAN.
]
)
