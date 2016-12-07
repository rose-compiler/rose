AC_DEFUN([ROSE_SUPPORT_FATAL_WARNINGS],
[

# ***************************************************************************
#    Option to define a uniform set of fatal warnings for ROSE development
# ***************************************************************************

# Semantics:
# 1. If this option is used then it sets a prescribed set of warnings and make them an error.
# 2. Additionally specific options may be specified to not be an error (not clear now widely 
#    available this last feature is on different compilers).
# 3. If specific warnings are specified, using the --with-CXX_WARNINGS and/or --with-C_WARNINGS options,
#    then these additional warnings will be added to the list of warnings which we be considered as errors.
# 4. The --with-CXX_WARNINGS and/or --with-C_WARNINGS options can also be used to specify warnings that 
#    would not be an error (see syntax below).
# 5. If fatal rose warning are not enabled, then the default behavior of the ROSE_FLAG_C_OPTIONS and 
#    ROSE_FLAG_CXX_OPTIONS functions will be to set specific default values that are suggested for ROSE 
#    development.
# 6. Default warnings are not added if --enable-fatal-rose-warnings is specified.

# Note that this function appears BEFORE the ROSE_FLAG_C_OPTIONS and ROSE_FLAG_CXX_OPTIONS functions
# and so this can be used to set values for C_WARNINGS and CXX_WARNINGS used in those functions.
# Note also that the values of C_WARNINGS and CXX_WARNINGS are added to CXXFLAGS and CFLAGS variable
# within those functions.

AC_MSG_CHECKING([for fatal rose warnings support])

AC_ARG_ENABLE(fatal-rose-warnings,
[  --enable-fatal-rose-warnings     support for an explicit list of warnings that are treated as errors (e.g. -Wunused-variable).],
[ echo "Setting up list of fatal rose warnings"
])

AM_CONDITIONAL(ROSE_USE_FATAL_ROSE_WARNINGS_SUPPORT, [test "x$enable_fatal_rose_warnings" = xyes])

if test "x$enable_fatal_rose_warnings" = "xyes"; then
  AC_MSG_WARN([Using a list of warnings that will be treated as errors for ROSE development.])
  AC_DEFINE([ROSE_USE_FATAL_ROSE_WARNINGS_SUPPORT], [], [Support for a list of warnings that will be treated as errors for ROSE development])

# Clear all previously defined warnings.
  CXX_WARNINGS=""
  C_WARNINGS=""

# Set default value to -Wall.
  CXX_WARNINGS="-Wall -Wextra "
  C_WARNINGS="-Wall -Wextra "

# Suggested C++ specific warnings (turning them on explicitly as errors).
# See documentation for suggested list in https://rosecompiler.atlassian.net/wiki/display/~matzke/Warnings+that+should+be+fixed
  CXX_WARNINGS+="-Werror=maybe-uninitialized "
  C_WARNINGS+="-Werror=maybe-uninitialized "

  CXX_WARNINGS+="-Werror=unused-but-set-variable "
  C_WARNINGS+="-Werror=unused-but-set-variable "

  CXX_WARNINGS+="-Werror=unused-variable "
  C_WARNINGS+="-Werror=unused-variable "

  CXX_WARNINGS+="-Werror=sign-compare "
  C_WARNINGS+="-Werror=sign-compare "

  CXX_WARNINGS+="-Werror=reorder "
  C_WARNINGS+="-Werror=reorder "

  CXX_WARNINGS+="-Werror=delete-non-virtual-dtor "
  C_WARNINGS+="-Werror=delete-non-virtual-dtor "

  CXX_WARNINGS+="-Werror=deprecated-declarations "
  C_WARNINGS+="-Werror=deprecated-declarations "

  CXX_WARNINGS+="-Werror=return-type "
  C_WARNINGS+="-Werror=return-type "

  CXX_WARNINGS+="-Werror=comment "
  C_WARNINGS+="-Werror=comment "

  CXX_WARNINGS+="-Werror=sequence-point "
  C_WARNINGS+="-Werror=sequence-point "

  CXX_WARNINGS+="-Werror=implicit-function-declaration "
  C_WARNINGS+="-Werror=implicit-function-declaration "

# Suggested C++ specific warnings.
# CXX_WARNINGS+="-Wunused-variable "
# C_WARNINGS+="-Wunused-variable "

# DQ (12/3/2016): Some examples of warnings noticed in clang 3.8 use on ROSE source code.
# CXX_WARNINGS+=" -Wc++11-extensions -Wunused-private-field -Woverloaded-virtual -Wtautological-compare -Wvarargs -Wuninitialized -Wimplicit-function-declaration -Wdelete-non-virtual-dtor -Wparentheses"
# CXX_WARNINGS+=" -Wsometimes-uninitialized -Wreorder -Wunneeded-internal-declaration -Wunused-function -Wmissing-declarations -Wcomment -Wempty-body -Wlogical-op-parentheses -Wreturn-type"
# CXX_WARNINGS+=" -Wmismatched-tags -Wint-to-pointer-cast -Wformat -W#warnings -Wparentheses-equality"

# Make all warnings an error (turne this off until we get a proper list of warnings to make errors.
# CXX_WARNINGS+="-Werror "
# C_WARNINGS+="-Werror "

# Except for a few specific warnings that should not be an error (unclear how widely available this option is).
# These are specific to Sawyer and we want to get past that code plus the ROSETTA code so that anyone can
# debug specific warnings in there code more directly (by compiling any subdirectory).
  CXX_WARNINGS+="-Wno-error=unused-variable "
  C_WARNINGS+="-Wno-error=unused-variable "

  CXX_WARNINGS+="-Wno-error=return-type "
  C_WARNINGS+="-Wno-error=return-type "

# DQ (12/6/2016): Disable this warning since it comes from Boost and we can't do anythng about it.
  CXX_WARNINGS+="-Wno-c++11-extensions "
  C_WARNINGS+="-Wno-c++11-extensions "

# DQ (12/6/2016): Disable this warning since it too frequent and a result of using the -Wextras option (not clear what to do about it).
  CXX_WARNINGS+="-Wno-unused-parameter "
  C_WARNINGS+="-Wno-unused-parameter "

# Experimenting with making some warnings non-fatal.
# CXX_WARNINGS+="-Wno-error=return-type  -Wno-error=#warnings "
# C_WARNINGS+="-Wno-error=return-type  -Wno-error=#warnings "
# CXX_WARNINGS+="-Wno-error=return-type "
# C_WARNINGS+="-Wno-error=return-type "

# DQ (12/3/2016): These show up as blank because we need to escape the "#" (something to figure out later).
# Note that use of "\#" to escape the "#" character causes the boost configure tests to fail.
# Currently a #warning in Saywer can not be eliminated as an error if -Werror is used.
# CXX_WARNINGS+="-Wno-error=\#warnings "
# C_WARNINGS+="-Wno-error=\#warnings "

# Incrementally add the advanced options
# if test "$CXX_ADVANCED_WARNINGS"; then CXXFLAGS="$CXXFLAGS $CXX_ADVANCED_WARNINGS"; fi
fi
# ROSE_USE_UNIFORM_DEBUG_SUPPORT=7
AC_SUBST(ROSE_USE_FATAL_ROSE_WARNINGS_SUPPORT)

echo "After processing --enable-fatal-rose-warnings: CXX_WARNINGS = ${CXX_WARNINGS}"
echo "After processing --enable-fatal-rose-warnings: C_WARNINGS   = ${C_WARNINGS}"

echo "CFLAGS   = $CFLAGS"
echo "CXXFLAGS = $CXXFLAGS"
echo "CPPFLAGS = $CPPFLAGS"

]
)

