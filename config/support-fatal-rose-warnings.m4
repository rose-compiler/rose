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
[ AC_MSG_NOTICE([setting up list of fatal rose warnings])
])

AM_CONDITIONAL(ROSE_USE_FATAL_ROSE_WARNINGS_SUPPORT, [test "x$enable_fatal_rose_warnings" = xyes])

if test "x$enable_fatal_rose_warnings" = "xyes"; then
  AC_MSG_WARN([using a list of warnings that will be treated as errors for ROSE development])
  AC_DEFINE([ROSE_USE_FATAL_ROSE_WARNINGS_SUPPORT], [], [Support for a list of warnings that will be treated as errors for ROSE development])

# Clear all previously defined warnings.
  CXX_WARNINGS=""
  C_WARNINGS=""

# Implement support to select warnings based on compiler version used to compile ROSE source code.
  AC_MSG_NOTICE([CXX_COMPILER_VENDOR = "$CXX_COMPILER_VENDOR"])
  AC_MSG_NOTICE([FRONTEND_CXX_COMPILER_VENDOR = "$FRONTEND_CXX_COMPILER_VENDOR"])

  AC_MSG_NOTICE([FRONTEND_CXX_VERSION_MAJOR = "$FRONTEND_CXX_VERSION_MAJOR"])
  AC_MSG_NOTICE([FRONTEND_CXX_VERSION_MINOR = "$FRONTEND_CXX_VERSION_MINOR"])

  case "$FRONTEND_CXX_COMPILER_VENDOR" in
    gnu)
      AC_MSG_NOTICE([Setup fatal warnings specific to GNU compiler use])

      AC_MSG_NOTICE([GNU compiler version: GCC_VERSION = "$GCC_VERSION"])
      AC_MSG_NOTICE([GNU compiler version: GCC_MINOR_VERSION = "$GCC_MINOR_VERSION"])

      CXX_WARNINGS="-Wall -Wextra "
      C_WARNINGS="-Wall -Wextra "

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

    # DQ (12/11/2016): Make this GNU g++ version specific.
      if test $GCC_VERSION -ge 6; then
         if test $GCC_MINOR_VERSION -ge 1; then
          # DQ (12/9/2016): Adding null pointer defererence.
            CXX_WARNINGS+="-Wnull-dereference "
            C_WARNINGS+="-Wnull-dereference "

          # echo "Setup use of -Wnull-dereference (required later version GNU compiler)"
          # exit 1
         fi
      fi

    # DQ (12/8/2016): Turn off this GNU specific warning which does not work on large files.
      CXX_WARNINGS+=-Wno-misleading-indentation "
      C_WARNINGS+=-Wno-misleading-indentation "

    # Except for a few specific warnings that should not be an error (unclear how widely available this option is).
    # These are specific to Sawyer and we want to get past that code plus the ROSETTA code so that anyone can
    # debug specific warnings in there code more directly (by compiling any subdirectory).
      CXX_WARNINGS+="-Wno-error=unused-variable "
      C_WARNINGS+="-Wno-error=unused-variable "

    # Skip output of this warnings because it happends too much at this point.
      CXX_WARNINGS+="-Wno-unused-variable "
      C_WARNINGS+="-Wno-unused-variable "

    # DQ (12/6/2016): Disable this warning since it too frequent and a result of using the -Wextras option (not clear what to do about it).
      CXX_WARNINGS+="-Wno-unused-parameter "
      C_WARNINGS+="-Wno-unused-parameter "
    ;;

    clang)
      AC_MSG_NOTICE([setup fatal warnings specific to Clang compiler use])

    # DQ (12/7/2016): For a truely insane perspective, all Clang warnings can be turned on 
    # using -Weverything (at least on Clang Version 3.8).  However I don't think this is 
    # productive at this point.  Using --with-CXX_WARNINGS="-Weverything" will also do this
    # from the ROSE configure line.
    # CXX_WARNINGS="-Weverything "
    # C_WARNINGS="-Weverything "

      CXX_WARNINGS="-Wall -Wextra "
      C_WARNINGS="-Wall -Wextra "

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

    # Except for a few specific warnings that should not be an error (unclear how widely available this option is).
    # These are specific to Sawyer and we want to get past that code plus the ROSETTA code so that anyone can
    # debug specific warnings in there code more directly (by compiling any subdirectory).
      CXX_WARNINGS+="-Wno-error=unused-variable "
      C_WARNINGS+="-Wno-error=unused-variable "

    # Skip output of this warnings because it happends too much at this point.
      CXX_WARNINGS+="-Wno-unused-variable "
      C_WARNINGS+="-Wno-unused-variable "

    # Don't make this an error for now.
      CXX_WARNINGS+="-Wno-error=return-type "
      C_WARNINGS+="-Wno-error=return-type "

    # DQ (12/6/2016): Disable this warning since it comes from Boost and we can't do anythng about it.
      CXX_WARNINGS+="-Wno-c++11-extensions "
      C_WARNINGS+="-Wno-c++11-extensions "

    # DQ (12/6/2016): Disable this warning since it too frequent and a result of using the -Wextras option (not clear what to do about it).
      CXX_WARNINGS+="-Wno-unused-parameter "
      C_WARNINGS+="-Wno-unused-parameter "
    ;;

    intel)
      AC_MSG_NOTICE([setup fatal warnings specific to Intel compiler use: no Intel specific warnings are setup as errors])
      CXX_WARNINGS="-Wall -Wextra "
      C_WARNINGS="-Wall -Wextra "

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

    # Except for a few specific warnings that should not be an error (unclear how widely available this option is).
    # These are specific to Sawyer and we want to get past that code plus the ROSETTA code so that anyone can
    # debug specific warnings in there code more directly (by compiling any subdirectory).
    # CXX_WARNINGS+="-Wno-error=unused-variable "
    # C_WARNINGS+="-Wno-error=unused-variable "

    # Skip output of this warnings because it happends too much at this point.
    # CXX_WARNINGS+="-Wno-unused-variable "
    # C_WARNINGS+="-Wno-unused-variable "

    # DQ (12/6/2016): Disable this warning since it too frequent and a result of using the -Wextras option (not clear what to do about it).
    # CXX_WARNINGS+="-Wno-unused-parameter "
    # C_WARNINGS+="-Wno-unused-parameter "

    ;;

    *)
      AC_MSG_NOTICE([setup fatal warnings specific to an unknown compiler being used (this is currently an error)])
      exit 1
    ;;
  esac

fi

# ROSE_USE_UNIFORM_DEBUG_SUPPORT=7
AC_SUBST(ROSE_USE_FATAL_ROSE_WARNINGS_SUPPORT)

AC_MSG_NOTICE([after processing --enable-fatal-rose-warnings: CXX_WARNINGS = "${CXX_WARNINGS}"])
AC_MSG_NOTICE([after processing --enable-fatal-rose-warnings: C_WARNINGS   = "${C_WARNINGS}"])

AC_MSG_NOTICE([CFLAGS   = "$CFLAGS"])
AC_MSG_NOTICE([CXXFLAGS = "$CXXFLAGS"])
AC_MSG_NOTICE([CPPFLAGS = "$CPPFLAGS"])

AC_MSG_NOTICE([adding fatal warnings to compiler flags])

CXXFLAGS="$CXXFLAGS $CXX_WARNINGS"
CFLAGS="$CFLAGS $C_WARNINGS"

AC_MSG_NOTICE([after being reset: CFLAGS   = "$CFLAGS"])
AC_MSG_NOTICE([after being reset: CXXFLAGS = "$CXXFLAGS"])

# echo "Exiting as a test in fatal-rose-warnings support!"
# exit 1

]
)

