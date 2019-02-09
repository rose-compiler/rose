##### http://www.rosecompiler.org
#
# SYNOPSIS
#
#   ROSE_SUPPORT_MPI([])
#
# DESCRIPTION
#
#   Determine if support is requested for the Message Passing Interface.
#
# COPYLEFT
#
#   Copyright (c) 2012 Justin Too <too1@llnl.gov>
#
#   Copying and distribution of this file, with or without
#   modification, are permitted in any medium without royalty provided
#   the copyright notice and this notice are preserved.

AC_DEFUN([ROSE_SUPPORT_MPI],
[
  ROSE_CONFIGURE_SECTION([Checking message passing interface (MPI)])
  dnl --with-mpi=<path>
  dnl
  dnl --show* option of MPICC is used to determine the include and library flags for MPI
  ROSE_ARG_WITH(
    [mpi],
    [for the Message Passing Interface (MPI)],
    [use the Message Passing Interface (MPI)]
    [Note: `mpicc` will be used to determine the MPI include and library flags],
    []
  )
  if test "x$CONFIG_HAS_ROSE_WITH_MPI" != "xno"; then
    MPI_INSTALL_PATH="$ROSE_WITH_MPI"
    MPI_BIN_PATH="$ROSE_WITH_MPI/bin"
    MPI_INCLUDE_PATH="$ROSE_WITH_MPI/include"
    MPI_LIBRARY_PATH="$ROSE_WITH_MPI/lib"
  else
    MPI_INSTALL_PATH=
    MPI_BIN_PATH=
    MPI_INCLUDE_PATH=
    MPI_LIBRARY_PATH=
  fi

  # Determine C MPI compiler flags
  #
  AC_REQUIRE([AC_PROG_CC])
  LX_QUERY_MPI_COMPILER(MPICC, [mpicc mpiicc mpixlc mpipgcc], C, $MPI_BIN_PATH)

  # Determine MPICXX compiler flags
  #
  AC_REQUIRE([AC_PROG_CXX])
  LX_QUERY_MPI_COMPILER(MPICXX, [mpicxx mpiCC mpic++ mpig++ mpiicpc mpipgCC mpixlC], CXX, $MPI_BIN_PATH)

  # Determine MPIF77 compiler flags
  #
  AC_REQUIRE([AC_PROG_F77])
  LX_QUERY_MPI_COMPILER(MPIF77, [mpif77 mpiifort mpixlf77 mpixlf77_r], F77, $MPI_BIN_PATH)

  # Determine MPI Fortran compiler flags
  #
  AC_REQUIRE([AC_PROG_FC])
  mpi_default_fc="mpif95 mpif90 mpigfortran mpif2003"
  mpi_intel_fc="mpiifort"
  mpi_xl_fc="mpixlf95 mpixlf95_r mpixlf90 mpixlf90_r mpixlf2003 mpixlf2003_r"
  mpi_pg_fc="mpipgf95 mpipgf90"
  LX_QUERY_MPI_COMPILER(MPIFC, [$mpi_default_fc $mpi_intel_fc $mpi_xl_fc $mpi_pg_fc], F, $MPI_BIN_PATH)

  if test -n "$MPI_BIN_PATH"; then
      # check if any mpi compiler was found
      #
      if test "x$MPICC" = "xnot-found" && \
         test "x$MPICXX" = "xnot-found" && \
         test "x$MPIF77" = "xnot-found" && \
         test "x$MPIFC" = "xnot-found"; then
          ROSE_MSG_ERROR([cannot detect MPI binaries (C, CXX, F77, F)])
      fi
  fi

  # Should we have separate flags for C and Fortran ?
  #
  AM_CONDITIONAL(ROSE_WITH_MPI_C, [test "x$MPICC" != "xnot-found" || test "x$MPICXX" != "xnot-found"])
  AM_CONDITIONAL(ROSE_WITH_MPI_FORTRAN, [test "x$MPIFC" != "xnot-found" || test "x$MPIF77" != "xnot-found"])
])

#
# Todd Gamblin's module to detect mpi flags
# LX_QUERY_MPI_COMPILER([compiler-var-name], [compiler-names], [output-var-prefix])
#  ------------------------------------------------------------------------
# AC_SUBST variables:
#     MPI_<prefix>FLAGS       Includes and defines for MPI compilation
#     MPI_<prefix>LDFLAGS     Libraries and library paths for linking MPI C programs
# 
# Shell variables output by this macro:
#     found_mpi_flags         'yes' if we were able to get flags, 'no' otherwise
#
AC_DEFUN([LX_QUERY_MPI_COMPILER],
[
     # Try to find a working MPI compiler from the supplied names
     # AC_PATH_TOOL checks for the programs in [$4] below
     # Omitting $PATH_SEPARATOR from args seems to break in linux
     AC_PATH_TOOL($1, [$2], [not-found], [$4]$PATH_SEPARATOR)

     mpi_command_line=`echo $$1`
     
     # Figure out what the compiler responds to get it to show us the compile
     # and link lines.  After this part of the macro, we'll have a valid 
     # lx_mpi_command_line

      if test "x$mpi_command_line" != "xnot-found"; then
         AC_MSG_CHECKING([if $$1 responds to -showme:compile])
         lx_mpi_compile_line=`$$1 -showme:compile 2>/dev/null`
      	 if [[ "$?" -eq 0 ]]; then
        	    AC_MSG_RESULT([yes])
             lx_mpi_link_line=`$$1 -showme:link 2>/dev/null`
         else
             AC_MSG_RESULT([no])
             AC_MSG_CHECKING([Checking if $$1 responds to -showme])
             lx_mpi_command_line=`$$1 -showme 2>/dev/null`
             if [[ "$?" -ne 0 ]]; then
                 AC_MSG_RESULT([no])
                 AC_MSG_CHECKING([Checking if $$1 responds to -compile-info])
                 lx_mpi_compile_line=`$$1 -compile-info 2>/dev/null`
                 if [[ "$?" -eq 0 ]]; then
                     AC_MSG_RESULT([yes])
                     lx_mpi_link_line=`$$1 -link-info 2>/dev/null`
                 else
                     AC_MSG_RESULT([no])
                     AC_MSG_CHECKING([Checking if $$1 responds to -show])
                     lx_mpi_command_line=`$$1 -show 2>/dev/null`
                     if [[ "$?" -eq 0 ]]; then
                         AC_MSG_RESULT([yes])
                     else
                         AC_MSG_RESULT([no]) 
                     fi
                 fi
             else
                 AC_MSG_RESULT([no]) 
             fi
         fi
	 	  
         if [[ ! -z "$lx_mpi_compile_line" -a ! -z "$lx_mpi_link_line" ]]; then
           # Now extract the different parts of the MPI command line.  Do these separately in case we need to 
           # parse them all out in future versions of this macro.
           lx_mpi_defines=`    echo "$lx_mpi_compile_line" | grep -o -- '\(^\| \)-D\([[^\"[:space:]]]\+\|\"[[^\"[:space:]]]\+\"\)'`
           lx_mpi_includes=`   echo "$lx_mpi_compile_line" | grep -o -- '\(^\| \)-I\([[^\"[:space:]]]\+\|\"[[^\"[:space:]]]\+\"\)'`
           lx_mpi_link_paths=` echo "$lx_mpi_link_line" | grep -o -- '\(^\| \)-L\([[^\"[:space:]]]\+\|\"[[^\"[:space:]]]\+\"\)'`
           lx_mpi_libs=`       echo "$lx_mpi_link_line" | grep -o -- '\(^\| \)-l\([[^\"[:space:]]]\+\|\"[[^\"[:space:]]]\+\"\)'`
           lx_mpi_link_args=`  echo "$lx_mpi_link_line" | grep -o -- '\(^\| \)-Wl,\([[^\"[:space:]]]\+\|\"[[^\"[:space:]]]\+\"\)'`

           # Create variables and clean up newlines and multiple spaces
           ROSE_WITH_MPI_$3FLAGS="$lx_mpi_defines $lx_mpi_includes"
           ROSE_WITH_MPI_$3LDFLAGS="$lx_mpi_link_paths $lx_mpi_libs $lx_mpi_link_args"
           ROSE_WITH_MPI_$3FLAGS=`  echo "$ROSE_WITH_MPI_$3FLAGS"   | tr '\n' ' ' | sed 's/^[[ \t]]*//;s/[[ \t]]*$//' | sed 's/  +/ /g'`
           ROSE_WITH_MPI_$3LDFLAGS=`echo "$ROSE_WITH_MPI_$3LDFLAGS" | tr '\n' ' ' | sed 's/^[[ \t]]*//;s/[[ \t]]*$//' | sed 's/  +/ /g'`

         fi
     else
	AC_MSG_WARN([$3 MPI compiler not found])
     fi

     #echo "flags: $ROSE_WITH_MPI_$3FLAGS"

     # try linking a MPI program
     #
     OLD_CPPFLAGS=$CPPFLAGS
     OLD_LIBS=$LIBS
     CPPFLAGS=$ROSE_WITH_MPI_$3FLAGS
     LIBS=$ROSE_WITH_MPI_$3LDFLAGS
     AC_TRY_LINK([#include <mpi.h>],
                [int rank, size;
                 MPI_Comm_rank(MPI_COMM_WORLD, &rank);
                 MPI_Comm_size(MPI_COMM_WORLD, &size);],
                [# Add a define for testing at compile time.
                 AC_DEFINE([HAVE_MPI], [1], [Define to 1 if you have MPI libs and headers.])
                 have_$3_mpi='yes'
                 AC_MSG_NOTICE([$3 MPI compiler binary found under $4])],
                [# zero out mpi flags so we don't link against the faulty library.
                 ROSE_WITH_MPI_$3FLAGS=""
                 ROSE_WITH_MPI_$3LDFLAGS=""
                 have_$3_mpi='no'])

     #Substitute in Makefiles
     AC_SUBST($1)
     AC_SUBST(ROSE_WITH_MPI_$3FLAGS)
     AC_SUBST(ROSE_WITH_MPI_$3LDFLAGS)
     LIBS=$OLD_LIBS
     CPPFLAGS=$OLD_CPPFLAGS
])
