AC_DEFUN([ROSE_SUPPORT_DWARF],
[
# Begin macro ROSE_SUPPORT_DWARF.

# Inclusion of test for DWARF libdwarf library and it's location.

AC_MSG_CHECKING(for dwarf)
AC_ARG_WITH(dwarf,
[  --with-dwarf=PATH	Specify the path where libdwarf is installed],
,
if test ! "$with_dwarf" ; then
   with_dwarf=no
fi
)

echo "In ROSE SUPPORT MACRO: with_dwarf $with_dwarf"

if test "$with_dwarf" = no; then
   # If dwarf is not specified, then don't use it.
   echo "Skipping use of Dwarf (libdwarf) support!"
else
   if test "$with_dwarf" = yes; then
      # Path not specified...
      echo "Error: Path to libdwarf not specified...(usage: --with-dwarf=PATH)"
      exit 1
   fi
   dwarf_path=$with_dwarf
   echo "Setup Dwarf support in ROSE! path = $dwarf_path"
   AC_DEFINE([USE_ROSE_DWARF_SUPPORT],1,[Controls use of ROSE support for Dwarf (libdwarf) library.])
 # DQ (6/5/2010): Note that Haskell support requires the dynamic library, and in general we want to require the dynamic library anyway.
 # DWARF_LINK="$dwarf_path/libdwarf.a -lelf"
 # DWARF_LINK="$dwarf_path/libdwarf.so -lelf"
   DWARF_LINK="-L$dwarf_path -ldwarf -lelf"
   DWARF_INCLUDE="-I$dwarf_path"

   echo "DWARF_INCLUDE = $DWARF_INCLUDE"
   echo "DWARF_LINK    = $DWARF_LINK"
fi

AC_SUBST(dwarf_path)

# DQ (9/4/2009): Added support for fix make installcheck rule when Dwarf is tested
AC_SUBST(DWARF_LINK)
AC_SUBST(DWARF_INCLUDE)

# End macro ROSE_SUPPORT_DWARF.
]
)
