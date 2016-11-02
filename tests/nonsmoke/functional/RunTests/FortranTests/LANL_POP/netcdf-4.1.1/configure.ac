#                                               -*- Autoconf -*-
## Process this file with autoconf to produce a configure script.

# This is part of Unidata's netCDF package. Copyright 2005-2006, see
# the COPYRIGHT file for more information.

# Recall that ${VAR-exp} expands to $VAR if var is set (even to null),
# and to exp otherwise. 

# $Id: configure.ac,v 1.417 2010/04/11 04:15:33 dmh Exp $

## This puts the cvs ID tag in the output configure script.
AC_REVISION([$Id: configure.ac,v 1.417 2010/04/11 04:15:33 dmh Exp $])

# Running autoconf on this file will trigger a warning if 
# autoconf is not at least the specified version.
AC_PREREQ([2.59])

# Initialize with name, version, and support email address. 
AC_INIT([netCDF], [4.1.1], [support-netcdf@unidata.ucar.edu])

# Create the VERSION file, which contains the package version from
# AC_INIT.
echo -n AC_PACKAGE_VERSION>VERSION
AC_SUBST(PACKAGE_VERSION)

AC_MSG_NOTICE([netCDF AC_PACKAGE_VERSION])

# Keep libtool macros in an m4 directory.
AC_CONFIG_MACRO_DIR([m4])

# Find out about the host we're building on.
AC_CANONICAL_HOST

# We will output a config.h for the C library, and a fortran inc file,
# nfconfig.inc.
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_HEADERS([fortran/nfconfig1.inc:fortran/nfconfig.in], 
   [sed '1d;s%^/\* \(.*\) \*/%\1%' fortran/nfconfig1.inc >conftest.tmp && mv conftest.tmp fortran/nfconfig.inc])

# This call is required by automake.
AM_INIT_AUTOMAKE([foreign dist-bzip2 dist-zip dist-tarZ subdir-objects])

# Check for the existance of this file before proceeding.
AC_CONFIG_SRCDIR([libsrc/nc.c])

AC_MSG_NOTICE([checking user options])

# Turn off shared libraries by default
AM_DISABLE_SHARED

# For backward compatibility, check to see if the user has set CXX,
# FC, and/or F90 to null. If so, treat that as the equivilent
# --disable- option to configure. The autoconf manual suggests the
# following to set a var if it's not already set:
# test "${var+set}" = set || var={value}

# Did the user set cxx to null?
AC_MSG_CHECKING([whether CXX is set to ''])
nc_null_cxx=no
test "x${CXX+set}" = xset && test "x$CXX" = x && nc_null_cxx=yes
AC_MSG_RESULT([$nc_null_cxx])

# Did the user set fc to null?
AC_MSG_CHECKING([whether FC is set to ''])
nc_null_fc=no
test "x${FC+set}" = xset && test "x$FC" = x && nc_null_fc=yes
AC_MSG_RESULT([$nc_null_fc])

# Did the user set f90 to null?
AC_MSG_CHECKING([whether F90 is set to ''])
nc_null_f90=no
test "x${F90+set}" = xset && test "x$F90" = x && nc_null_f90=yes
AC_MSG_RESULT([$nc_null_f90])

# Did the user specify a default minimum blocksize (NCIO_MINBLOCKSIZE) for posixio?
AC_MSG_CHECKING([whether a NCIO_MINBLOCKSIZE was specified])
AC_ARG_WITH([minblocksize],
              [AS_HELP_STRING([--with-minblocksize=<integer>],
                              [Specify minimum I/O blocksize for netCDF classic and 64-bit offset format files.])],
            [NCIO_MINBLOCKSIZE=$with_minblocksize], [NCIO_MINBLOCKSIZE=256])
AC_MSG_RESULT([$NCIO_MINBLOCKSIZE])
AC_DEFINE_UNQUOTED([NCIO_MINBLOCKSIZE], [$NCIO_MINBLOCKSIZE], [min blocksize for posixio.])

AC_MSG_CHECKING([whether udunits is to be built])
AC_ARG_WITH([udunits],
              [AS_HELP_STRING([--with-udunits],
                              [Build udunits2 package.])])
test "x$with_udunits" = xyes || with_udunits=no
AC_MSG_RESULT($with_udunits)

AC_MSG_CHECKING([if fsync support is enabled])
AC_ARG_ENABLE([fsync],
              [AS_HELP_STRING([--disable-fsync],
                              [disable fsync support])])
test "x$enable_fsync" = xno || enable_fsync=yes
AC_MSG_RESULT($enable_fsync)
if test "x$enable_fsync" = xyes ; then
AC_DEFINE([USE_FSYNC], [1], [if true, include experimental fsync code])
fi

# Does the user want to run extra tests with valgrind?
AC_MSG_CHECKING([whether extra valgrind tests should be run])	
AC_ARG_ENABLE([valgrind-tests],
              [AS_HELP_STRING([--enable-valgrind-tests],
                              [build with valgrind-tests (valgrind is required)])])
test "x$enable_valgrind_tests" = xyes || enable_valgrind_tests=no
AC_MSG_RESULT($enable_valgrind_tests)

# Does the user want to build netcdf-4?
AC_ARG_ENABLE([netcdf-4], [AS_HELP_STRING([--enable-netcdf-4],
              [build with netcdf-4 (HDF5 and zlib required)])])
# Synonym
# --enable-netcdf-4 is the controlling enable switch
if test "x$enable_netcdf_4" = "x" ; then
  AC_ARG_ENABLE([netcdf4], [AS_HELP_STRING([--enable-netcdf4],
              [build with netcdf-4 (HDF5 and zlib required)])])
  enable_netcdf_4="$enable_netcdf4"
else
  enable_netcdf4="$enable_netcdf_4"
fi

# Does the user want to also build the libcf library?
AC_MSG_CHECKING([whether libcf is to be built])
AC_ARG_WITH([libcf],
              [AS_HELP_STRING([--with-libcf],
                              [build and install libcf library, a library for \
	      handling data in conformance with the Climate and \
	      Forecast conventions. (Requires netCDF-4 and HDF5)])])
test "x$with_libcf" = xyes || with_libcf=no
AC_MSG_RESULT($with_libcf)

# Does the user want to turn on HDF4 read ability?
AC_MSG_CHECKING([whether reading of HDF4 SD files is to be enabled])
AC_ARG_ENABLE([hdf4], [AS_HELP_STRING([--enable-hdf4],
              [build netcdf-4 with HDF4 read capability (HDF4, HDF5 and zlib required)])])
test "x$enable_hdf4" = xyes || enable_hdf4=no
AC_MSG_RESULT($enable_hdf4)

# Does the user want to turn on extra HDF4 file tests?
AC_MSG_CHECKING([whether to fetch some sample HDF4 files from Unidata ftp site to test HDF4 reading (requires wget)])
AC_ARG_ENABLE([hdf4-file-tests], [AS_HELP_STRING([--enable-hdf4-file-tests],
              [get some HDF4 files from Unidata ftp site and test that they can be read])])
test "x$enable_hdf4" = xyes -a "x$enable_hdf4_file_tests" = xyes || enable_hdf4_file_tests=no
if test "x$enable_hdf4_file_tests" = xyes; then
   AC_DEFINE([USE_HDF4_FILE_TESTS], 1, [If true, use use wget to fetch some sample HDF4 data, and then test against it.])
fi
AC_MSG_RESULT($enable_hdf4_file_tests)

# Does the user want to turn on PNETCDF read ability?
AC_MSG_CHECKING([whether parallel I/O for classic and 64-bit offset files using parallel-netcdf is to be enabled])
AC_ARG_ENABLE([pnetcdf], [AS_HELP_STRING([--enable-pnetcdf],
              [build netcdf-4 with parallel I/O for classic and 64-bit offset files using parallel-netcdf])])
test "x$enable_pnetcdf" = xyes || enable_pnetcdf=no
AC_MSG_RESULT($enable_pnetcdf)

# Did the user specify a location for the PNETCDF library?
AC_MSG_CHECKING([whether a location for the parallel-netcdf library was specified])
AC_ARG_WITH([pnetcdf],
              [AS_HELP_STRING([--with-pnetcdf=<directory>],
                              [Specify location of parallel-netcdf library. Configure will \
			      expect to find subdirs include and lib.])],
            [PNETCDFDIR=$with_pnetcdf])
if test "x$PNETCDFDIR" = "x" ; then
AC_MSG_RESULT([no])
else
AC_MSG_RESULT([$PNETCDFDIR])
fi
AC_SUBST(PNETCDFDIR, [$PNETCDFDIR])

# Does the user want to build cxx-4?
AC_MSG_CHECKING([whether new netCDF-4 C++ API is to be built])
AC_ARG_ENABLE([cxx-4],
              [AS_HELP_STRING([--enable-cxx-4],
                              [build with new netcdf-4 C++ API])])
test "x$enable_cxx_4" = xyes || enable_cxx_4=no
if test "x$enable_netcdf_4" = xno; then
   enable_cxx_4=no
fi
AC_MSG_RESULT($enable_cxx_4)

# Should netcdf-4 do data conversions, or should HDF5?
# (Note: netCDF-4 should always do conversions, because HDF5 has different handling of extreme numbers.
dnl AC_MSG_CHECKING([whether HDF5 should do data conversions])
dnl AC_ARG_ENABLE([hdf5-convert],
dnl               [AS_HELP_STRING([--enable-hdf5-convert],
dnl                               [have HDF5 do data conversions instead of netCDF-4])])
dnl test "x$enable_hdf5_convert" = xyes || enable_hdf5_convert=no
dnl AC_MSG_RESULT($enable_hdf5_convert)
dnl if test "x$enable_hdf5_convert" = xyes; then
dnl    AC_DEFINE([HDF5_CONVERT], 1, [if true, use HDF5 data conversions])
dnl fi

# Does the user want to run extra example tests
AC_MSG_CHECKING([whether extra example tests should be run])
AC_ARG_ENABLE([extra-example-tests],
              [AS_HELP_STRING([--enable-extra-example-tests],
                              [Run extra example tests; requires GNU sed. Ignored if \
                               netCDF-4 is not enabled.])])
test "x$enable_extra_example_tests" = xyes || enable_extra_example_tests=no
AC_MSG_RESULT($enable_extra_example_tests)

# Does the user want to run extra parallel tests when parallel netCDF-4 is built?
AC_MSG_CHECKING([whether parallel IO tests should be run])
AC_ARG_ENABLE([parallel-tests],
              [AS_HELP_STRING([--enable-parallel-tests],
                              [Run extra parallel IO tests. Ignored if \
                               netCDF-4 is not enabled, or built on a system \
                               without parallel I/O support.])])
test "x$enable_parallel_tests" = xyes || enable_parallel_tests=no
AC_MSG_RESULT($enable_parallel_tests)

# Did the user specify a location for the HDF5 library?
AC_MSG_CHECKING([whether a location for the HDF5 library was specified])
AC_ARG_WITH([hdf5],
              [AS_HELP_STRING([--with-hdf5=<directory>],
                              [Specify location of HDF5 library. Configure will \
			      expect to find subdirs include and lib.])],
            [HDF5DIR=$with_hdf5])
AC_MSG_RESULT([$HDF5DIR])
AC_SUBST(HDF5DIR, [$HDF5DIR])

# Did the user specify a location for the ZLIB library?
AC_MSG_CHECKING([whether a location for the ZLIB library was specified])
AC_ARG_WITH([zlib],
              [AS_HELP_STRING([--with-zlib=<directory>],
                              [Specify location of ZLIB library. ZLIB is required \
			      for netCDF-4. Configure will expect to find subdirs \
			      include and lib.])],
            [ZLIBDIR=$with_zlib])
AC_MSG_RESULT([$ZLIBDIR])
AC_SUBST(ZLIBDIR, [$ZLIBDIR])

# Did the user specify a location for the SZLIB library?
AC_MSG_CHECKING([whether a location for the SZLIB library was specified])
AC_ARG_WITH([szlib],
              [AS_HELP_STRING([--with-szlib=<directory>],
                              [Specify location of SZLIB library. SZLIB is not \
			      required for netCDF-4, but you must specify its location \
			      if HDF5 was built with SZLIB. Configure will expect to \
			      find subdirs include and lib.])],
            [SZLIBDIR=$with_szlib])
AC_MSG_RESULT([$SZLIBDIR])
AC_SUBST(SZLIBDIR, [$SZLIBDIR])

# Did the user specify a location for the HDF4 library?
AC_MSG_CHECKING([whether a location for the HDF4 library was specified])
AC_ARG_WITH([hdf4],
              [AS_HELP_STRING([--with-hdf4=<directory>],
                              [Specify location of HDF4 library. Configure will \
			      expect to find subdirs include and lib.])],
            [HDF4DIR=$with_hdf4])
AC_MSG_RESULT([$HDF4DIR])
AC_SUBST(HDF4DIR, [$HDF4DIR])

# Did the user specify a default chunk size?
AC_MSG_CHECKING([whether a default chunk size in bytes was specified])
AC_ARG_WITH([default-chunk-size],
              [AS_HELP_STRING([--with-default-chunk-size=<integer>],
                              [Specify default size of chunks in bytes.])],
            [DEFAULTT_CHUNK_SIZE=$with_default_chunk_size], [DEFAULT_CHUNK_SIZE=4194304])
AC_MSG_RESULT([$DEFAULT_CHUNK_SIZE])
AC_DEFINE_UNQUOTED([DEFAULT_CHUNK_SIZE], [$DEFAULT_CHUNK_SIZE], [default chunk size in bytes])

# Did the user specify a max per-var cache size?
AC_MSG_CHECKING([whether a maximum per-variable cache size for HDF5 was specified])
AC_ARG_WITH([max-default-cache-size],
              [AS_HELP_STRING([--with-max-default-cache-size=<integer>],
                              [Specify maximum size (in bytes) for the default per-var chunk cache.])],
            [MAX_DEFAULT_CACHE_SIZE=$with_max_default_cache_size], [MAX_DEFAULT_CACHE_SIZE=67108864])
AC_MSG_RESULT([$MAX_DEFAULT_CACHE_SIZE])
AC_DEFINE_UNQUOTED([MAX_DEFAULT_CACHE_SIZE], [$MAX_DEFAULT_CACHE_SIZE], [max size of the default per-var chunk cache.])

# Did the user specify a number of chunks in default per-var cache size?
AC_MSG_CHECKING([whether a number of chunks for the default per-variable cache was specified])
AC_ARG_WITH([default-chunks-in-cache],
              [AS_HELP_STRING([--with-default-chunks-in-cache=<integer>],
                              [Specify the number of chunks to store in default per-variable cache.])],
            [DEFAULT_CHUNKS_IN_CACHE=$with_default_chunks_in_cache], [DEFAULT_CHUNKS_IN_CACHE=10])
AC_MSG_RESULT([$DEFAULT_CHUNKS_IN_CACHE])
AC_DEFINE_UNQUOTED([DEFAULT_CHUNKS_IN_CACHE], [$DEFAULT_CHUNKS_IN_CACHE], [num chunks in default per-var chunk cache.])

# Did the user specify a default cache size?
AC_MSG_CHECKING([whether a default file cache size for HDF5 was specified])
AC_ARG_WITH([chunk-cache-size],
              [AS_HELP_STRING([--with-chunk-cache-size=<integer>],
                              [Specify default file cache chunk size for HDF5 files in bytes.])],
            [CHUNK_CACHE_SIZE=$with_chunk_cache_size], [CHUNK_CACHE_SIZE=4194304])
AC_MSG_RESULT([$CHUNK_CACHE_SIZE])
AC_DEFINE_UNQUOTED([CHUNK_CACHE_SIZE], [$CHUNK_CACHE_SIZE], [default file chunk cache size in bytes.])

# Did the user specify a default cache nelems?
AC_MSG_CHECKING([whether a default file cache maximum number of elements for HDF5 was specified])
AC_ARG_WITH([chunk-cache-nelems],
              [AS_HELP_STRING([--with-chunk-cache-nelems=<integer>],
                              [Specify default maximum number of elements in the file chunk cache chunk for HDF5 files (should be prime number).])],
            [CHUNK_CACHE_NELEMS=$with_chunk_cache_nelems], [CHUNK_CACHE_NELEMS=1009])
AC_MSG_RESULT([$CHUNK_CACHE_NELEMS])
AC_DEFINE_UNQUOTED([CHUNK_CACHE_NELEMS], [$CHUNK_CACHE_NELEMS], [default file chunk cache nelems.])

# Did the user specify a default cache preemption?
AC_MSG_CHECKING([whether a default cache preemption for HDF5 was specified])
AC_ARG_WITH([chunk-cache-preemption],
              [AS_HELP_STRING([--with-chunk-cache-preemption=<float between 0 and 1 inclusive>],
                              [Specify default file chunk cache preemption policy for HDF5 files (a number between 0 and 1, inclusive).])],
            [CHUNK_CACHE_PREEMPTION=$with_chunk_cache_preemption], [CHUNK_CACHE_PREEMPTION=0.75])
AC_MSG_RESULT([$CHUNK_CACHE_PREEMPTION])
AC_DEFINE_UNQUOTED([CHUNK_CACHE_PREEMPTION], [$CHUNK_CACHE_PREEMPTION], [default file chunk cache preemption policy.])

# Does the user want to enable netcdf-4 logging?
AC_MSG_CHECKING([whether netCDF-4 logging is enabled])
AC_ARG_ENABLE([logging],
              [AS_HELP_STRING([--enable-logging],
                              [enable logging capability (only applies when netCDF-4 is built). \
			      This debugging features is only of interest to netCDF developers. \
			      Ignored if netCDF-4 is not enabled.])])   
test "x$enable_logging" = xyes || enable_logging=no
AC_MSG_RESULT([$enable_logging])

# Figure out if DAP support should be enabled.
# The primary goal is to first locate curl-config
# or the curl library location
# The rules we implement are applied in order:
# 1. no:  if --disable-dap is set
# 2. no:  if curl library cannot be located
# 3. yes: if --enable-dap is set
# 4. yes: if neither --enable-dap nor --disable-dap is specified
#         and the curl library can be located

# Try to locate curl
curlsrc=
AC_MSG_CHECKING([whether a path for curl-config was specified])
AC_ARG_WITH([curl-config], [AS_HELP_STRING([--with-curl-config=<path>],
            [Specify path (or the containing directory) of the curl-config program; libcurl is required for opendap.])],
            [curlsrc="withconfig"],[])
if test "x$curlsrc" = "x" ; then
  AC_MSG_RESULT([no])
else
  AC_MSG_RESULT([$with_curl_config])
  # Perhaps the user specified the containing directory
  if test -f $with_curl_config/curl-config ; then
    with_curl_config="$with_curl_config/curl-config"
  fi  
  #verify
  if $with_curl_config --prefix >/dev/null 2>&1 ; then ignore=1; else
    AC_MSG_ERROR([Erroneous --with-curl-config value])
  fi
fi

# Alternatively, allow specification of the curl installation directory
AC_MSG_CHECKING([whether a location for curl installation was specified])
AC_ARG_WITH([curl], [AS_HELP_STRING([--with-curl=<directory>.],
            [Specify location of CURL library; libcurl is required for opendap. Configure will expect to find subdirs bin, include and lib.])],
            [curlsrc="with"],[])
if test "x$curlsrc" = "xwith" ; then
  AC_MSG_RESULT([$with_curl])
  # look for curl-config
  if $with_curl/bin/curl-config --version >/dev/null 2>&1 ; then
    # pretend that --with-curl-config was specified
    curlsrc="withconfig"
    with_curl_config="$with_curl/bin/curl-config"
    AC_MSG_NOTICE([checking for curl-config using --with-curl... yes])
  else
    AC_MSG_NOTICE([checking for curl-config using --with-curl... no])
  fi
else
  AC_MSG_RESULT([no])
fi

if test "x$curlsrc" = "x" ; then
  # look for curl-config in the path
  if curl-config --version >/dev/null 2>&1 ; then
    with_curl_config="curl-config"
    curlsrc="withconfig"
    AC_MSG_NOTICE([checking whether a location for curl-config is in PATH... yes])
  else
    AC_MSG_NOTICE([checking whether a location for curl-config is in PATH... no])
  fi
fi

# compute curl values
if test -n "$with_curl_config" -a -z "$with_curl" ; then
    # pretend that --with-curl-config was specified
    with_curl=`$with_curl_config --prefix`
fi

# Compute the curl flags and libs
if test -n "$with_curl_config" ; then
  curl_cflags=`$with_curl_config --cflags`
  curl_libs=`$with_curl_config --libs`
  curl_dir="$with_curl"
elif test -n "$with_curl" ; then
  curl_libs="-L$with_curl/lib -lcurl"
  curl_cflags="-I$with_curl/include"
  curl_dir="$with_curl"
else
  curl_cflags=-lcurl
  curl_libs=
  curl_dir=
fi

AC_SUBST([CURL_CFLAGS],[${curl_cflags}])
AC_SUBST([CURL_FCFLAGS],[${curl_cflags}])
AC_SUBST([CURL_FFLAGS],[${curl_cflags}])
AC_SUBST([CURL_LIBS],[${curl_libs}])
AC_SUBST([CURLDIR],[${curl_dir}])

# Capture the state of the --enable-dap flag
AC_MSG_CHECKING([whether DAP client is to be built])
AC_ARG_ENABLE([dap],
                 [AS_HELP_STRING([--disable-dap],
                                 [build without DAP client support.])])

# Apply rules and report result
if test "x$enable_dap" = "xno" ; then
  enable_dap=no
elif test "x$curlsrc" = "x" ; then
  if test "x$enable_dap" = "xyes" ; then
     AC_MSG_ERROR("--enable-dap requires curl library")
  fi
  enable_dap=no
elif test "x$enable_dap" = "xyes" ; then
  enable_dap=yes
elif test "x$enable_dap" = "x" ; then
  enable_dap=yes
else
  enable_dap=no
fi
AC_MSG_RESULT($enable_dap)
if test "x$enable_dap" = xyes; then
   AC_DEFINE([USE_DAP], [1], [if true, build DAP Client])
fi

# Default is now to always do the short remote tests
AC_MSG_CHECKING([whether dap remote testing should be enabled (default on)])
AC_ARG_ENABLE([dap-remote-tests],
              [AS_HELP_STRING([--disable-dap-remote-tests],
                                 [disable dap remote tests])])
test "x$enable_dap_remote_tests" = xno || enable_dap_remote_tests=yes
if test "x$enable_dap" = "xno" ; then
  enable_dap_remote_tests=no
fi
AC_MSG_RESULT($enable_dap_remote_tests)

AC_MSG_CHECKING([whether the time-consuming dap tests should be enabled (default off)])
AC_ARG_ENABLE([dap-long-tests],
              [AS_HELP_STRING([--enable-dap-long-tests],
                                 [enable dap long tests])])
test "x$enable_dap_long_tests" = xyes || enable_dap_long_tests=no
if test "x$enable_dap_remote_tests" = "xno" ; then
  enable_dap_long_tests=no
fi
AC_MSG_RESULT([$enable_dap_long_tests])

# Did user must specify a location for liboc?
AC_MSG_CHECKING([whether a location for liboc was specified])
AC_ARG_WITH([oclib], [AS_HELP_STRING([--with-oclib=<directory>],
            [Specify location of liboc. Configure will expect to find subdirs include and lib. This is required for opendap; if not specified, then the internal version will be used.])])
if test -n "$with_oclib" ; then
  AC_MSG_RESULT([$with_oclib])
else
  AC_MSG_RESULT([no])
fi
oclibdir="$with_oclib"

# -loc is required if BUILD_DAP is true
if test -n "${oclibdir}" ; then
  AC_MSG_NOTICE([external oclib library not specified; using internal version])
  oclib_cflags=
  oclib_libs=
else
  oclib_cflags="-I${oclibdir}/include"
  oclib_libs="-L${oclibdir}/lib -loc"
fi
AC_SUBST([OCLIB_CFLAGS],[${oclib_cflags}])
AC_SUBST([OCLIB_LIBS],[${oclib_libs}])
AM_CONDITIONAL(INTERNAL_OCLIB,[test "x${with_oclib}" = "x"])

# Does the user want to do some extra tests?
AC_MSG_CHECKING([whether netCDF extra tests should be run (developers only)])
AC_ARG_ENABLE([extra-tests],
              [AS_HELP_STRING([--enable-extra-tests],
                              [run some extra tests that may not pass because of known issues])])
test "x$enable_extra_tests" = xyes || enable_extra_tests=no
AC_MSG_RESULT($enable_extra_tests)
if test "x$enable_extra_tests" = xyes; then
   AC_DEFINE([EXTRA_TESTS], [1], [if true, run extra tests which may not work yet])
fi
AM_CONDITIONAL(EXTRA_TESTS, [test x$enable_extra_tests = xyes])

# Does the user want to test f90 compiler.
AC_MSG_CHECKING([whether Fortran compiler(s) should be tested during configure])
AC_ARG_ENABLE([fortran-compiler-check],
              [AS_HELP_STRING([--disable-fortran-compiler-check],
                              [disable check of F77/F90 compiler during configure])])
test "x$enable_fortran_compiler_check" = xno || enable_fortran_compiler_check=yes
AC_MSG_RESULT($enable_fortran_compiler_check)

# Does the user want to use the ffio module?
AC_MSG_CHECKING([whether FFIO will be used])
AC_ARG_ENABLE([ffio],
              [AS_HELP_STRING([--enable-ffio],
                              [use ffio instead of posixio (ex. on the Cray)])])
test "x$enable_ffio" = xyes || enable_ffio=no
AC_MSG_RESULT($enable_ffio)
AM_CONDITIONAL(USE_FFIO, [test x$enable_ffio = xyes])

# Does the user want to enable the user-provided NEC-SX vectorization
# patch.
dnl AC_MSG_CHECKING([whether netCDF NEC-SX vectorization patch is enabled])
dnl AC_ARG_ENABLE([sx-vectorization],
dnl               [AS_HELP_STRING([--enable-sx-vectorization],
dnl                               [enable a user-provided performance patch to allow \	
dnl 			      vectorization of type conversions on NEC SX machines.])])   
dnl test "x$enable_sx_vectorization" = xyes || enable_sx_vectorization=no
dnl AC_MSG_RESULT([$enable_sx_vectorization])
dnl if test "x$enable_sx_vectorization" = xyes; then
dnl    AC_DEFINE([SX_VECTORIZATION], 1, [if true, turn on vectorization patch for NEC SX])
dnl fi

# Do we want to recover from a bad C++, F77, or F90 compiler?
nc_compiler_recover=yes
AC_MSG_CHECKING([whether to skip C++, F77, or F90 APIs if compiler is broken])
AC_ARG_ENABLE([compiler-recover],
              [AS_HELP_STRING([--disable-compiler-recover],
                              [don't continue if a desired C++, F77, or F90 \
                              compiler is missing; this is useful for testing \
                              netCDF, but is not very useful for most netCDF \
                              users])])
test "x$enable_compiler_recover" = "xno" || enable_compiler_recover=yes
AC_MSG_RESULT([$enable_compiler_recover])

# If --enable-c-only is specified only the C lib will be built.
AC_MSG_CHECKING([whether only the C library is desired])
AC_ARG_ENABLE([c-only],
              [AS_HELP_STRING([--enable-c-only],
                              [build only the netCDF-3 C library; same as \
                              --disable-f77 --disable-cxx --disable-v2 --disable-examples \
                              --disable-utilities])])
test "x$enable_c_only" = xyes || enable_c_only=no
AC_MSG_RESULT([$enable_c_only])
if test "x$enable_c_only" = xyes; then
nc_build_f77=no
nc_build_f90=no
nc_build_cxx=no
nc_build_v2=no
nc_build_utilities=no
nc_build_examples=no
else
nc_build_f77=yes
nc_build_f90=yes
nc_build_cxx=yes
nc_build_v2=yes
nc_build_utilities=yes
nc_build_examples=yes
fi

# Does the user want to build examples?
AC_MSG_CHECKING([whether examples should be built])
AC_ARG_ENABLE([examples],
              [AS_HELP_STRING([--disable-examples],
                              [don't build the netCDF examples during make check \
                              (examples are treated as extra tests by netCDF)])])
test "x$enable_examples" = xno && nc_build_examples=no
AC_MSG_RESULT($nc_build_examples)
AM_CONDITIONAL(BUILD_EXAMPLES, [test x$nc_build_examples = xyes])

# Do we want to build the fortran 77 API? Check for --disable-f77.
AC_MSG_CHECKING([whether F77 API is desired])
AC_ARG_ENABLE([f77],
              [AS_HELP_STRING([--disable-f77],
                              [don't try to build the netCDF Fortran 77 (or \   
                              Fortran 90) API])])
test "x$enable_f77" = "xno" && nc_build_f77=no
test "x$nc_null_fc" = xyes && nc_build_f77=no
AC_MSG_RESULT([$nc_build_f77])

# Do we want to build the fortran 77 API? Check for --disable-fortran
# for Russ, who can never remember --disable-f77..
AC_MSG_CHECKING([whether any Fortran API is desired])
AC_ARG_ENABLE([f77],
              [AS_HELP_STRING([--disable-fortran],
                              [don't try to build the netCDF Fortran 77 or \   
                              Fortran 90 APIs (same effect as --disable-f77)])])
test "x$enable_fortran" = "xno" && nc_build_f77=no
AC_MSG_RESULT([$nc_build_f77])

# Do we want to build the fortran 90 API?
nc_build_f90=yes
AC_MSG_CHECKING([whether F90 API is desired])
AC_ARG_ENABLE([f90],
              [AS_HELP_STRING([--disable-f90],
                              [don't try to build the netCDF Fortran 90 API])])
test "x$enable_f90" = xno && nc_build_f90=no
test "x$nc_null_fc" = xyes || test "x$nc_null_f90" = xyes || \
     test "x$nc_build_f77" = xno && nc_build_f90=no
AC_MSG_RESULT([$nc_build_f90])

# Does the user want to check into fortran type information?
AC_MSG_CHECKING([whether fortran type sizes should be checked])
AC_ARG_ENABLE([fortran-type-check],
              [AS_HELP_STRING([--disable-fortran-type-check],
                              [cause the Fortran type sizes checks to be skipped])])
test "x$enable_fortran_type_check" = xno || enable_fortran_type_check=yes
AC_MSG_RESULT($enable_fortran_type_check)

# Do we want to build the C API? If not, the location of an existing C
# library must be specified with --with-netcdf=
nc_build_c=yes
AC_MSG_CHECKING([whether C API is desired])
AC_ARG_ENABLE([c],
              [AS_HELP_STRING([--disable-c],
                              [don't try to build the netCDF C API (if this option \
                               is specified, the --with-netcdf-c-lib option must \
                               also be used)])])
test "x$enable_c" = xno && nc_build_c=no
AC_MSG_RESULT([$nc_build_c])

# If building without the C API (for example to build some Fortran API
# for a new Fortran compiler), the --with-netcdf-c-lib= option must be
# used to tell where to find the netCDF C API. This must point to a
# netCDF C library which does NOT contain the F77 or F90 functions.
AC_MSG_CHECKING([where to get netCDF C-only library for separate fortran libraries])
AC_ARG_WITH([netcdf-c-lib],
            [AS_HELP_STRING([--netcdf-c-lib=<directory>],
                            [Specify location of netCDF C-only library (only \
                            valid when --disable-c is also specified).])],
            [NETCDF_C_LIB=$with_netcdf_c_lib])
AM_CONDITIONAL(USE_NETCDF_C_LIB_DIR, [test ! "x$NETCDF_C_LIB" = x])
AC_MSG_RESULT($NETCDF_C_LIB)
AC_SUBST(NETCDF_C_LIB)

# If the C API is turned off, a C-only library must be specified.
if test "x$nc_build_c" = xno -a "x$NETCDF_C_LIB" = x; then
   AC_MSG_ERROR([If the C API is disabled, you must specify a location of \
   a netCDF C-only library with --with-netcdf-c-lib.])
fi

# Do we want to build the C++ API? Check for --disable-cxx as a
# configure argument.
AC_MSG_CHECKING([whether CXX API is desired])
AC_ARG_ENABLE([cxx],
              [AS_HELP_STRING([--disable-cxx],
                              [don't try to build C++ API])])
test "x$enable_cxx" = xno && nc_build_cxx=no
test "x$nc_null_cxx" = xyes && nc_build_cxx=no
AC_MSG_RESULT([$nc_build_cxx])

# If the C API is turned off, a C++ must also be disabled.
if test "x$nc_build_c" = xno -a "x$nc_build_cxx" = xyes; then
   AC_MSG_ERROR([If the C API is disabled, you must also disable C++ API with --disable-cxx.])
fi

# Does the user want to disable the V2 API?
AC_MSG_CHECKING([whether v2 netCDF API should be built])
AC_ARG_ENABLE([v2],
              [AS_HELP_STRING([--disable-v2],
                              [turn off the netCDF version 2 API])])
test "x$enable_v2" = xno && nc_build_v2=no
AC_MSG_RESULT($nc_build_v2)
AM_CONDITIONAL(BUILD_V2, [test x$nc_build_v2 = xyes])
if test "x$nc_build_v2" = xno; then
   AC_DEFINE_UNQUOTED(NO_NETCDF_2, 1, [do not build the netCDF version 2 API])
fi

# Does the user want to disable ncgen/ncdump?
AC_MSG_CHECKING([whether the ncgen/ncdump should be built])
AC_ARG_ENABLE([utilities],
              [AS_HELP_STRING([--disable-utilities],
                              [don't build netCDF utilities ncgen and ncdump])])
test "x$nc_build_c" = xno && enable_utilities=no
test "x$enable_utilities" = xno && nc_build_utilities=no
AC_MSG_RESULT($nc_build_utilities)
AM_CONDITIONAL(BUILD_UTILITIES, [test x$nc_build_utilities = xyes])

# Does the user want to run tests for large files (> 2GiB)?
AC_MSG_CHECKING([whether large file (> 2GB) tests should be run])
AC_ARG_ENABLE([large-file-tests],
              [AS_HELP_STRING([--enable-large-file-tests],
                              [Run tests which create very large data files (~13 GB disk space
                              required, but it will be recovered when tests are complete). See
                              option --with-temp-large to specify temporary directory])])
test "x$enable_large_file_tests" = xyes || enable_large_file_tests=no
AC_MSG_RESULT($enable_large_file_tests)
AM_CONDITIONAL(LARGE_FILE_TESTS, [test x$enable_large_file_tests = xyes])
if test "x$enable_large_file_tests" = xyes; then
   AC_DEFINE([LARGE_FILE_TESTS], [1], [do large file tests])
fi

# Does the user want to run benchmarks?
AC_MSG_CHECKING([whether benchmaks should be run (experimental)])
AC_ARG_ENABLE([benchmarks],
              [AS_HELP_STRING([--enable-benchmarks],
                              [Run benchmarks. This is an experimental feature. You must fetch 
                              sample data files from the Unidata ftp site to use these benchmarks. 
                              The benchmarks are a bunch of extra tests, which are timed. What the 
                              times really mean, if anything, is something we are still trying to 
                              figure out.])])
test "x$enable_benchmarks" = xyes || enable_benchmarks=no
AC_MSG_RESULT($enable_benchmarks)
AM_CONDITIONAL(BUILD_BENCHMARKS, [test x$enable_benchmarks = xyes])

# Does the user want to use extreme numbers in testing.
AC_MSG_CHECKING([whether extreme numbers should be used in tests])
AC_ARG_ENABLE([extreme-numbers],
              [AS_HELP_STRING([--disable-extreme-numbers],
                              [don't use extreme numbers during testing, such as MAX_INT - 1])])
case "$host_cpu $host_os" in
     *386*solaris*)
        test "x$enable_extreme_numbers" = xyes || enable_extreme_numbers=no
        ;;
     *)
        test "x$enable_extreme_numbers" = xno || enable_extreme_numbers=yes
        ;;
esac     
AC_MSG_RESULT($enable_extreme_numbers)

if test "x$enable_extreme_numbers" = xyes; then
   AC_DEFINE(USE_EXTREME_NUMBERS, 1, [set this to use extreme numbers in tests])
fi

# If the env. variable TEMP_LARGE is set, or if
# --with-temp-large=<directory>, use it as a place for the large
# (i.e. > 2 GiB) files created during the large file testing.
AC_MSG_CHECKING([where to put large temp files if large file tests are run])
AC_ARG_WITH([temp-large],
            [AS_HELP_STRING([--with-temp-large=<directory>],
                            [specify directory where large files (i.e. >2 GB) \
                            will be written, if large files tests are run with
                            --enable-large-file-tests])],
            [TEMP_LARGE=$with_temp_large])
TEMP_LARGE=${TEMP_LARGE-.}
AC_MSG_RESULT($TEMP_LARGE)
#AC_SUBST(TEMP_LARGE)
AC_DEFINE_UNQUOTED([TEMP_LARGE], ["$TEMP_LARGE"], [Place to put very large netCDF test files.])

# If --enable-dll is specified the DLL will be built. This only works
# on mingw.
AC_MSG_CHECKING([whether a win32 DLL is desired])
AC_ARG_ENABLE([dll],
              [AS_HELP_STRING([--enable-dll],
                              [build a win32 DLL (only works on mingw)])])
test "x$enable_dll" = xyes || enable_dll=no
AC_MSG_RESULT([$enable_dll])
AM_CONDITIONAL(BUILD_DLL, [test x$enable_dll = xyes])
if test "x$enable_dll" = xyes; then
   AC_DEFINE(DLL_NETCDF, 1, [set this only when building a DLL under MinGW])
fi

# If --enable-separate-fortran is selected the F77 and F90 APIs will
# each get their own libraray instead of being crammed in with the C
# functions. If shared libraries are being built, this is turned
# on. (Otherwise C programmers using the shared netcdf library would
# not be able to run C/netCDF programs without the F90 shared
# libraries.)
AC_MSG_CHECKING([whether separate fortran libs are desired])
AC_ARG_ENABLE([separate-fortran],
              [AS_HELP_STRING([--enable-separate-fortran],
                              [build F77 and F90 libraries separate from C library \
                              (turned on automatically when shared libraries are built)])])
test "x$enable_separate_fortran" != xno && test "x$enable_shared" = xyes && enable_separate_fortran=yes
test "x$enable_separate_fortran" = xyes || enable_separate_fortran=no
AC_MSG_RESULT([$enable_separate_fortran])
AM_CONDITIONAL(BUILD_SEPARATE_FORTRAN, [test x$enable_separate_fortran = xyes])

# If the C API is turned off, separate fortran must be turned on.
if test "x$nc_build_c" = xno -a x$enable_separate_fortran != xyes; then
   AC_MSG_ERROR([If the C API is disabled, you must use --enable-separate-fortran.])
fi

## Does the user want to allow UTF8 characters
#AC_MSG_CHECKING([whether UTF8 is allowed in identifiers])
#AC_ARG_ENABLE([utf8],
#              [AS_HELP_STRING([--enable-utf8],
#                              [Allow UTF8.])])
#test "x$enable_utf8" = xyes || enable_utf8=no
#AC_MSG_RESULT($enable_utf8)
#if test "x$enable_utf8" = xyes; then
#   AC_DEFINE([ALLOW_UTF8], 1, [set this to allow UTF8 characters in identifiers])
#fi
#AM_CONDITIONAL(ALLOW_UTF8, [test x$enable_utf8 = xyes])

# According to the autoconf mailing list gurus, we must test for
# compilers unconditionally. That is, we can't skip looking for the
# fortran compilers, just because the user doesn't want fortran. This
# is due to a limitation in autoconf.

# Find the C compiler.
AC_MSG_NOTICE([finding C compiler])

AC_PROG_CC(xlc cc c89 acc gcc)
AM_PROG_CC_C_O
AC_C_CONST

# If the user set environment var FC, but not F77, then set F77=FC
if test "x${FC+set}" = xset -a "x${F77+set}" != xset; then
   F77=${FC}
fi

# If F90 is set, and FC isn't, then set FC to whatever F90 was set to.
if test "${F90+set}" = set -a "${FC+set}" != set; then
   FC=$F90
fi

# If F77 is set, and FC (still) isn't, then set FC to whatever F77 was
# set to.
if test "${F77+set}" = set -a "${FC+set}" != set; then
   FC=$F77
fi

# Find fortran compiler. If FC isn't set after AC_PROG_FC, we couldn't
# find a fortran compiler. If the user requested the fortran API, and
# has told us not to recover from a missing compiler, then bail out
# right here.
AC_MSG_NOTICE([finding Fortran compiler (will not be used if Fortran API is not desired)])

# On solaris, f95 comes before gfortran.
case "$host" in
  *solaris*) 
             F77S="xlf fort xlf95 ifort ifc efc pgf77 lf95 f95 f90 g95 gfortran f77 frt pgf95 fort77 fl32 af77 f90 xlf90 pgf90 epcf90 g77"
             F90S="xlf90 fort xlf95 ifort ifc efc pgf95 lf95 f95 f90 g95 gfortran xlf90 pgf90 epcf90"
             ;;
  *) 
             F77S="xlf fort xlf95 ifort ifc efc pgf77 lf95 g95 gfortran f77 frt pgf95 f95 f90 fort77 fl32 af77 f90 xlf90 pgf90 epcf90 g77"
             F90S="xlf90 fort xlf95 ifort ifc efc pgf95 lf95 g95 gfortran f95 f90 f90 xlf90 pgf90 epcf90"
             ;;
esac

# Find a fortran 90 compiler, if possible.
# MingW has a wierd problem. If arguments are passed to AC_PROG_FC,
# the link of tests won't work (you get a multiply defined atexit
# function.) This is annoying, but there it is.
if test "$host" = "i686-pc-mingw32"; then
   AC_PROG_FC
else
   AC_PROG_FC($F90S)
fi

# If we couldn't find an F90 compiler, deal with it.
if test "${FC+set}" = set && test "x$FC" = x; then
   if test "x$enable_compiler_recover" = xno && test "x$nc_build_f90" = xyes; then
      AC_MSG_ERROR([Can't find F90 compiler, and compiler recovery disabled.])
   fi
   AC_MSG_WARN([No F90 compiler found. F90 API will not be built.])
   nc_build_f90=no
fi

# We must call AC_PROG_F77 here, otherwise the LIBTOOL macro below
# will call it, and reset F77 to g77.
AC_PROG_F77($F77S)
if test "${F77+set}" = set && test "x$F77" = x; then
   if test "x$enable_compiler_recover" = xno && test "x$nc_build_f77" = xyes; then
      AC_MSG_ERROR([Can't find F77 compiler, and compiler recovery disabled.])
   fi
   AC_MSG_WARN([No F77 compiler found. F77 API will not be built.])
   nc_build_f77=no
fi

if test "x$nc_build_f90" = xno; then
   F90=
   # If F77 is set, and FC isn't, then set FC to whatever F77 was set to.
   FC=$F77
fi

# See if the fortran 90 build is desired. If so, set some stuff
if test "x$nc_build_f90" = xyes; then
   AC_MSG_NOTICE([setting up Fortran 90])
   if test "${F90+set}" != set; then 
      F90=$FC 
   fi

   # Some f90 compilers change the case of the mod file names. Others
   # require special flags to be set to dea with .f90 files. Find out
   # about this compiler.
   AC_LANG_PUSH(Fortran)
   AC_FC_SRCEXT(f90)
   AC_LANG_POP(Fortran)
   AX_F90_MODULE_FLAG
   AC_SUBST(MOD_FLAG, ${ax_cv_f90_modflag})

   # Set the FCLIBS flag to help with shared libraries.
#   AC_FC_LIBRARY_LDFLAGS
#   AC_F77_LIBRARY_LDFLAGS      

   # Check the fortran 90 compiler to make sure it can compile a
   # little test file.
   if test "x$enable_fortran_compiler_check" = xyes; then
      AC_MSG_CHECKING([if Fortran compiler can handle Fortran-90])
      cat <<EOF >conftest.f90
      program foo
      REAL, PARAMETER        :: A = 3.141592654
      end program
EOF
      doit='$FC  ${FCFLAGS} ${FCFLAGS_f90} conftest.f90'
      if AC_TRY_EVAL(doit); then
         nc_ans=yes
      else
         nc_ans=no
         if test "x$nc_build_f90" = xyes && test "x$enable_compiler_recover" = xno; then
            AC_MSG_ERROR([Can't use F90 compiler, F90 API desired, and compiler recovery disabled.])
         fi
         unset F90
         nc_build_f90=no
      fi
      rm -f conftest.*
      AC_MSG_RESULT([$nc_ans])
      if test "x$nc_ans" = xno; then
         AC_MSG_WARN([No F90 compiler found. F90 API will not be built.])
      fi
   fi
fi

AC_MSG_NOTICE([finding C++ compiler (will not be used if C++ API is not desired)])

# Don't use gcc as a C++ compiler, it won't work unless -lstdc++ is also used.
# On cygwin, osf, and apple, go to g++ first.
case "$host" in
  *cygwin*|*osf*|*apple*) CCS="g++ cl KCC CC cxx cc++ xlC aCC c++ g++ egcs";;
  *) CCS="cl KCC CC cxx cc++ xlC aCC c++ g++ egcs"
esac

AC_PROG_CXX($CCS)

if test "${CXX+set}" = set && test "x$CXX" = x; then
   if test "x$enable_compiler_recover" = xno && test "x$nc_build_cxx" = xyes; then
      AC_MSG_ERROR([Can't find CXX compiler, and compiler recovery disabled.])
   fi
   AC_MSG_WARN([No CXX compiler found. The C++ API will not be built.])
   nc_build_cxx=no
fi

# Set these conditionals for automake. For some reason, it can't keep
# it's SUBDIR order straight if I use BUILD_F77 to attach the nf_test
# directory, so also define an automake conditional TEST_F77 every
# time BUILD_F77 is defined, and set it to the same value.
AM_CONDITIONAL(BUILD_C, [test "x$nc_build_c" = xyes])
AM_CONDITIONAL(BUILD_F77, [test "x$nc_build_f77" = xyes])
AM_CONDITIONAL(TEST_F77, [test "x$nc_build_f77" = xyes])
AM_CONDITIONAL(BUILD_F90, [test "x$nc_build_f90" = xyes])
AM_CONDITIONAL(BUILD_CXX, [test "x$nc_build_cxx" = xyes])

# Set up libtool.
AC_MSG_NOTICE([setting up libtool])
LT_PREREQ([2.2.2])
#LT_INIT(win32-dll)
AC_PROG_LIBTOOL

AC_MSG_NOTICE([finding other utilities])

AC_PROG_INSTALL

if test "x$nc_build_f77" = xyes; then
   AC_MSG_NOTICE([trying to set fortran flags for this platform])
   # Set fortran flag if the user has not already set it in CPPFLAGS.
   case "$CPPFLAGS" in
    *pgiFortran*|*NAGf90Fortran*|*f2cFortran*|*hpuxFortran*|*apolloFortran*|*sunFortran*|*IBMR2Fortran*|*CRAYFortran*|*PATHSCALE_COMPILER*|*gFortran*|*mipsFortran*|*DECFortran*|*vmsFortran*|*CONVEXFortran*|*PowerStationFortran*|*AbsoftUNIXFortran*|*AbsoftProFortran*|*SXFortran*)
       ;;
      *)
         case "$FC" in
            pgf95|pgf90|pgf77|ifort*|lf90|lf95)
               AC_DEFINE(pgiFortran, [1], [Turned on by netCDF configure.])
               ;;
             *gfortran) 
                if (gfortran --version | grep '(GCC 4.1.' || gfortran --version | grep '(GCC 4.0.'); then
                   AC_DEFINE(gFortran, [1], [Turned on by netCDF configure.])    
                else
                   AC_DEFINE(pgiFortran, [1], [Turned on by netCDF configure.])    
                fi
                ;;
             *g77) 
                AC_DEFINE(f2cFortran, [1], [Turned on by netCDF configure.])
                ;;
             *)
                # If it's a gnu compiler, guess f2c.
                if test "x$ac_cv_fc_compiler_gnu" = xyes; then
                   AC_DEFINE(f2cFortran, [1], [Turned on by netCDF configure.])
                fi
                ;;
         esac
   esac
fi 

# AC_SYS_LARGEFILE has problems on AIX when OBJECT_MODE is 64. 
AC_MSG_CHECKING([if we need to set _LARGE_FILES despite results of previous tests])
if test "$enable_largefile" != no; then
case "$host_os" in
     *aix*)
        if test "$OBJECT_MODE" = 64; then
           AC_DEFINE(_LARGE_FILES, [1], [Turned on by netCDF configure.])
        fi
        case "$CFLAGS" in
             *q64*)
                AC_DEFINE(_LARGE_FILES, [1], [Turned on by netCDF configure.])
                ;;
        esac
        ;;
     *hpux*)
        AC_DEFINE(_FILE_OFFSET_BITS, [64], [Turned on by netCDF configure.])
        ;;
esac
fi
AC_MSG_RESULT(_LARGE_FILES=${_LARGE_FILES})

AC_SYS_LARGEFILE

AC_MSG_NOTICE([displaying some results])

## This next macro just prints some results for debugging
## support issues.
UD_DISPLAY_RESULTS

# For nightly build testing, output CC, FC, etc.
if test x$enable_compiler_recover = xno; then
   echo "CPPFLAGS=$CPPFLAGS CC=$CC CFLAGS=$CFLAGS CXX=$CXX CXXFLAGS=$CXXFLAGS FC=$FC FCFLAGS=$FCFLAGS F77=$F77 FFLAGS=$FFLAGS LDFLAGS=$LDFLAGS LIBS=$LIBS FLIBS=$FLIBS F90LIBS=$F90LIBS" >> comps.txt
fi

ac_cv_prog_f90_uppercase_mod=no

# This will test the f77 compiler, as well as check some fortran types.
if test "x$nc_build_f77" = xyes; then

   if test "$enable_fortran_type_check" = yes; then
      UD_FORTRAN_TYPES
      if test "x$ud_fc_failed" = xyes -a "x$enable_compiler_recover" = xno -a "x$nc_build_f77" = xyes; then
         AC_MSG_ERROR([F77 compiler doesn't work, and compiler recovery disabled.])
      fi
   else
      AC_DEFINE(NCBYTE_T, byte)
      AC_DEFINE(NCSHORT_T, integer*2)
      AC_DEFINE(NF_INT1_T, byte)
      AC_DEFINE(NF_INT2_T, integer*2)
      AC_DEFINE(NF_INT1_IS_C_SIGNED_CHAR, 1, [default])
      AC_DEFINE(NF_INT2_IS_C_SHORT, 1, [default])
      AC_DEFINE(NF_INT_IS_C_INT, 1, [default])
      AC_DEFINE(NF_REAL_IS_C_FLOAT, 1, [default])
      AC_DEFINE(NF_DOUBLEPRECISION_IS_C_DOUBLE, 1, [default])
   fi

   # Test whether F90 module names get capitalized.
   if test "x$nc_build_f90" = xyes; then
      AC_PROG_FC_UPPERCASE_MOD
   fi
fi
AM_CONDITIONAL(UPPER_CASE_MOD, [test "x$ac_cv_prog_f90_uppercase_mod" = xyes])

AC_MSG_NOTICE([checking types, headers, and functions])

AC_CHECK_HEADER(stdlib.h, ,AC_DEFINE([NO_STDLIB_H], [], [no stdlib.h]))
AC_CHECK_HEADER(sys/types.h, ,AC_DEFINE([NO_SYS_TYPES_H], [], [no sys_types.h]))
AC_CHECK_HEADERS([locale.h])
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_HEADER_TIME
AC_CHECK_HEADERS([stdio.h stdarg.h errno.h ctype.h fcntl.h malloc.h stdlib.h string.h strings.h unistd.h sys/stat.h getopt.h])
AC_FUNC_VPRINTF

# Check for <stdbool.h> that conforms to C99 requirements
AC_HEADER_STDBOOL

# Check for these functions...
AC_CHECK_FUNCS([strlcat strerror snprintf strchr strrchr mktemp strcat strcpy strdup strcasecmp])
AC_CHECK_FUNCS([getrlimit])
AC_CHECK_FUNCS([gettimeofday])
AC_CHECK_FUNCS([fsync])
AC_CHECK_FUNCS([MPI_Comm_f2c])
AC_FUNC_ALLOCA
AC_CHECK_DECLS([isnan, isinf, isfinite, signbit],,,[#include <math.h>])
#UD_CHECK_LIB_MATH

AC_STRUCT_ST_BLKSIZE
UD_CHECK_IEEE
AC_TYPE_SIZE_T
AC_TYPE_OFF_T
AC_CHECK_TYPES([ssize_t, ptrdiff_t, uchar])
#AC_CHECK_TYPE(ssize_t, int)
#AC_CHECK_TYPE(ptrdiff_t, int)
#AC_CHECK_TYPE(uchar, unsigned char)
AC_C_CHAR_UNSIGNED
AC_C_BIGENDIAN
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)
AC_CHECK_SIZEOF(float)
AC_CHECK_SIZEOF(double)
AC_CHECK_SIZEOF(off_t)
AC_CHECK_SIZEOF(size_t)
UD_MAKEWHATIS

# Save the old value of LDFLAGS, it will be restored later.
OLD_LDFLAGS=${LDFLAGS}
OLD_LIBS=${LIBS}
OLD_CPPFLAGS=${CPPFLAGS}

# Add the HDF5 directory and the math library info for the linker
# to use in the tests below.
if test ! "x$ZLIBDIR" = x; then
   LDFLAGS="${LDFLAGS} -L${ZLIBDIR}/lib"
   CPPFLAGS="${CPPFLAGS} -I${ZLIBDIR}/include"
fi    
if test ! "x$HDF5DIR" = x; then
   LDFLAGS="${LDFLAGS} -L${HDF5DIR}/lib"
   CPPFLAGS="${CPPFLAGS} -I${HDF5DIR}/include"
fi    
LIBS="${LIBS} -lm -lz -lhdf5"

# Add the HDF4 directory and library if needed.
if test "x$enable_hdf4" = xyes; then
   if test ! "x$HDF4DIR" = x; then
      LDFLAGS="${LDFLAGS} -L${HDF4DIR}/lib"
      CPPFLAGS="${CPPFLAGS} -I${HDF4DIR}/include"
   fi    
   LIBS="${LIBS} -lmfhdf -ldf -ljpeg -lz"
fi

# Add the szlib dir and library if needed.
nc_has_szlib=no
if test ! "x$SZLIBDIR" = x; then
   LDFLAGS="${LDFLAGS} -L${SZLIBDIR}/lib"
   LIBS="-lsz ${LIBS}"
   CPPFLAGS="${CPPFLAGS} -I${SZLIBDIR}/include"
   nc_has_szlib=yes
fi

# Add the parallel-netcdf dir and library if needed.
if test ! "x$PNETCDFDIR" = x; then
   LDFLAGS="${LDFLAGS} -L${PNETCDFDIR}/lib"
   LIBS="-lpnetcdf ${LIBS}"
   CPPFLAGS="${CPPFLAGS} -I${PNETCDFDIR}/include"
fi

# Check for the header.
AC_CHECK_HEADERS([hdf5.h], [], [nc_hdf5_h_missing=yes])

# If HDF5 is not found, error out if the user specifically asked for
# netcdf-4, otherwise just turn netCDF-4 off.
if test "x$nc_hdf5_h_missing" = xyes; then
   if test "x$enable_netcdf_4" = xyes -o "x$HDF5DIR" != x; then
      AC_MSG_ERROR([NetCDF-4 requires HDF5, but hdf5.h cannot be found. Try using \
      the --with-hdf5 option to specify location of HDF5.])
   else	  
      enable_netcdf_4=no
   fi
else
   if test ! "x$enable_netcdf_4" = xno; then
      enable_netcdf_4=yes
   fi
fi

AC_MSG_CHECKING([whether we should try to build netCDF-4])
AC_MSG_RESULT([$enable_netcdf_4])

if test "x$enable_netcdf_4" = xyes; then
   AC_DEFINE([USE_NETCDF4], [1], [if true, build netCDF-4])
   AC_DEFINE([H5_USE_16_API], [1], [use HDF5 1.6 API])

   # Check for the main hdf5 library.
   AC_CHECK_LIB([hdf5], [H5Fflush], [], [nc_hdf5_lib_missing=yes])
   if test "x$nc_hdf5_h_missing" = xyes || test "x$nc_hdf5_lib_missing" = xyes; then
      AC_MSG_ERROR([Can't find or link to the hdf5 library. Configure with --disable-netcdf-4 or see config.log for errors.])
   fi
   
   # We also require the HDF5 High Level library.
   AC_CHECK_LIB([hdf5_hl], [H5DSis_scale], [], [nc_hdf5_hl_lib_missing=yes])
   if test "x$nc_hdf5_hl_lib_missing" = xyes; then
      AC_MSG_ERROR([Can't find or link to the hdf5 high-level. Configure with --disable-netcdf-4 or see config.log for errors.])
   fi
   
   # The user may have parallel HDF5 based on MPI POSIX.
   AC_CHECK_LIB([hdf5], [H5Pget_fapl_mpiposix], [enable_parallel_posix=yes], [])
   if test "x$enable_parallel_posix" = xyes; then
      AC_DEFINE([USE_PARALLEL_POSIX], [1], [if true, compile in parallel netCDF-4 based on MPI/POSIX])
   fi
   
   # The user may have parallel HDF5 based on MPI mumble mumble.
   AC_CHECK_LIB([hdf5], [H5Pget_fapl_mpio], [enable_parallel_mpio=yes], [])
   if test "x$enable_parallel_mpio" = xyes; then
      AC_DEFINE([USE_PARALLEL_MPIO], [1], [if true, compile in parallel netCDF-4 based on MPI/IO])
   fi

   # If parallel is in use, enable it in the C code. Also add some stuff to netcdf.h.
   enable_parallel=no
   AC_MSG_CHECKING([how to modify netcdf.h to deal with parallel I/O])
   if test "x$enable_parallel_mpio" = xyes -o "x$enable_parallel_posix" = xyes; then
      enable_parallel=yes
      AC_DEFINE([USE_PARALLEL], [1], [if true, parallel netCDF-4 is in use])
   
      # Modify netcdf.h to include parallel stuff.
      sed '/#include <errno.h>/{G;s/$/\/* mpi.h included by configure. *\//;G;s/$/#include <mpi.h>/;}' libsrc4/netcdf_base.h > libsrc4/netcdf.h

      # Using pnetcdf for classic parallel I/O?
      if test "x$enable_pnetcdf" = xyes; then
         AC_CHECK_LIB([pnetcdf], [ncmpi_create], [nc4_have_pnetcdf_lib=yes])
	 if test "x$nc4_have_pnetcdf_lib" != xyes; then
            AC_MSG_ERROR([Cannot link to pnetcdf library, yet --enable-pnetcdf was used.])
	 fi
dnl          AC_CHECK_HEADERS([pnetcdf.h], [], [nc4_pnetcdf_h_missing=yes)	   
dnl 	 if test ! "x$HAVE_PNETCDF" = x1; then
dnl             AC_MSG_ERROR([Cannot find pnetcdf header, yet --enable-pnetcdf was used.])
dnl 	 fi
         AC_DEFINE([USE_PNETCDF], [1], [if true, parallel netCDF is used])      	 
      fi
      AC_MSG_RESULT([with parallel])         
   else
      AC_MSG_RESULT([without parallel])         
      # Modify netcdf.h to include defs for missing parallel stuff.
      sed '/#include <errno.h>/{G;s/$/\/* These defs added by netCDF configure because parallel HDF5 is not present. *\//;G;s/$/#ifndef MPI_INCLUDED/;G;s/$/#define MPI_Comm int/;G;s/$/#define MPI_Info int/;G;s/$/#define MPI_COMM_WORLD 0/;G;s/$/#define MPI_INFO_NULL 0/;G;s/$/#endif/;}' libsrc4/netcdf_base.h > libsrc4/netcdf.h
   fi
   
   AC_MSG_CHECKING([whether parallel I/O features are to be included])
   AC_MSG_RESULT([$enable_parallel])         
   
   # The user must have build HDF5 with the ZLIB library.
   AC_CHECK_LIB([hdf5], [H5Pset_deflate], [enable_zlib=yes], [])
   if test "x$enable_zlib" = xyes; then
      AC_DEFINE([USE_ZLIB], [1], [if true, compile in zlib compression in netCDF-4 variables])
   fi

   # The user must have build HDF5 with the ZLIB library.
   AC_CHECK_LIB([hdf5], [H5Z_can_apply_szip], [enable_szip=yes], [])
   if test "x$enable_szip" = xyes; then
      AC_DEFINE([USE_SZIP], [1], [if true, compile in szip compression in netCDF-4 variables])
   fi

   # If the user wants hdf4 built in, check it out.
   if test "x$enable_hdf4" = xyes; then
      AC_CHECK_HEADERS([mfhdf.h], [], [nc_mfhdf_h_missing=yes])	   
      if test "x$nc_mfhdf_h_missing" = xyes; then
         AC_MSG_ERROR([Cannot find mfhdf.h, yet --enable-hdf4 was used.])
      fi		      
      AC_CHECK_LIB([mfhdf], [SDcreate], [], [])
      AC_DEFINE([USE_HDF4], [1], [if true, use HDF4 too])
   fi
else
# !netcdf-4
   # Modify netcdf.h to include defs for missing parallel stuff.
   sed '/#include <errno.h>/{G;s/$/\/* These defs added by netCDF configure because parallel HDF5 is not present. *\//;G;s/$/#ifndef MPI_INCLUDED/;G;s/$/#define MPI_Comm int/;G;s/$/#define MPI_Info int/;G;s/$/#define MPI_COMM_WORLD 0/;G;s/$/#define MPI_INFO_NULL 0/;G;s/$/#endif/;}' libsrc4/netcdf_base.h > libsrc4/netcdf.h
fi

# Restore LDFLAGS to it's old value.
LDFLAGS=${OLD_LDFLAGS}
LIBS=${OLD_LIBS}
CPPFLAGS=${OLD_CPPFLAGS}

# Do Renaming if netcdf4 or dap support is enabled
if test "x$enable_netcdf_4" = xyes -o "x$enable_dap" = xyes ; then
   AC_DEFINE([USE_RENAMEV3], [1], [if true, build libsrc code with renamed API functions])
fi

# No logging for netcdf-3.
if test "x$enable_netcdf_4" = xno; then
   enable_logging=no
fi
if test "x$enable_logging" = xyes; then
   AC_DEFINE([LOGGING], 1, [If true, turn on logging.])
fi

AC_MSG_CHECKING([if API dispatch is enabled])
AC_ARG_ENABLE([dispatch],
              [AS_HELP_STRING([--enable-dispatch],
                              [enable experimental dispatch support])])
test "x$enable_dispatch" = xyes || enable_dispatch=no
AC_MSG_RESULT($enable_dispatch)
if test "x$enable_dispatch" = xyes ; then
AC_DEFINE([USE_DISPATCH], [1], [if true, include experimental API dispatch code])
fi

# Automake conditionals need to be called, whether the answer is yes
# or no.
AM_CONDITIONAL(BUILD_PARALLEL, [test x$enable_parallel = xyes])
AM_CONDITIONAL(TEST_PARALLEL, [test "x$enable_parallel" = xyes -a "x$enable_parallel_tests" = xyes])
AM_CONDITIONAL(BUILD_DAP, [test "x$enable_dap" = xyes])
AM_CONDITIONAL(USE_DAP, [test "x$enable_dap" = xyes]) # Alias
AM_CONDITIONAL(ENABLE_DAP_REMOTE_TESTS, [test "x$enable_dap_remote_tests" = xyes])
AM_CONDITIONAL(ENABLE_DAP_LONG_TESTS, [test "x$enable_dap_long_tests" = xyes])
AM_CONDITIONAL(BUILD_CXX4, [test x$enable_cxx_4 = xyes])
AM_CONDITIONAL(EXTRA_EXAMPLE_TESTS, [test "x$enable_extra_example_tests" = xyes])
AM_CONDITIONAL(USE_HDF5_DIR, [test ! "x$HDF5DIR" = x])
AM_CONDITIONAL(USE_HDF4_DIR, [test ! "x$HDF4DIR" = x])
AM_CONDITIONAL(USE_ZLIB_DIR, [test ! "x$ZLIBDIR" = x])
AM_CONDITIONAL(USE_SZLIB_DIR, [test ! "x$SZLIBDIR" = x])
AM_CONDITIONAL(USE_SZIP, [test "x$enable_szip" = xyes])
AM_CONDITIONAL(USE_PNETCDF_DIR, [test ! "x$PNETCDFDIR" = x])
AM_CONDITIONAL(USE_LOGGING, [test "x$enable_logging" = xyes])
AM_CONDITIONAL(BUILD_UDUNITS, [test "x$with_udunits" = xyes])
AM_CONDITIONAL(BUILD_LIBCF, [test "x$with_libcf" = xyes])
AM_CONDITIONAL(CROSS_COMPILING, [test "x$cross_compiling" = xyes])
AM_CONDITIONAL(USE_VALGRIND_TESTS, [test "x$enable_valgrind_tests" = xyes])
AM_CONDITIONAL(USE_NETCDF4, [test x$enable_netcdf_4 = xyes])
AM_CONDITIONAL(USE_HDF4, [test x$enable_hdf4 = xyes])
AM_CONDITIONAL(USE_HDF4_FILE_TESTS, [test x$enable_hdf4_file_tests = xyes])
AM_CONDITIONAL(USE_RENAMEV3, [test x$enable_netcdf_4 = xyes -o x$enable_dap = xyes])
AM_CONDITIONAL(USE_PNETCDF, [test x$enable_pnetcdf = xyes])
AM_CONDITIONAL(USE_DISPATCH, [test "x$enable_dispatch" = xyes])

# If the machine doesn't have a long long, and we want netCDF-4, then
# we've got problems!
if test "x$enable_netcdf_4" = xyes; then
   AC_TYPE_LONG_LONG_INT
   AC_TYPE_UNSIGNED_LONG_LONG_INT
dnl    if test ! "x$ac_cv_type_long_long_int" = xyes -o ! "x$ac_cv_type_unsigned_long_long_int" = xyes; then
dnl       AC_MSG_ERROR([This platform does not support long long types. These are required for netCDF-4.])
dnl    fi
fi

# Create the file name for a "make ftpbin" which is used to generate a
# binary distribution. For each release we generate binary releases on
# the thousands of machines in Unidata's vast underground complex at
# an undisclosed location in the Rocky Mountains. The binary
# distributions, along with the 25-foot thick cement slabs and the
# giant springs, will help distribute netCDF even after a catastrophic
# meteor strike.
AC_MSG_CHECKING([what to call the output of the ftpbin target])
BINFILE_NAME=binary-netcdf-$PACKAGE_VERSION
test "x$enable_netcdf_4" = xno && BINFILE_NAME=${BINFILE_NAME}_nc3
test "x$nc_build_f77" = xyes && BINFILE_NAME=${BINFILE_NAME}_${F77}
test "x$nc_build_f90" = xyes && BINFILE_NAME=${BINFILE_NAME}_${F90}
test "x$nc_build_cxx" = xyes && BINFILE_NAME=${BINFILE_NAME}_${CXX}
BINFILE_NAME=${BINFILE_NAME}.tar
AC_SUBST(BINFILE_NAME)
AC_MSG_RESULT([$BINFILE_NAME $FC $CXX])

UD_FTPBINDIR

##################################################
# DRNO and OC configuration

# Require some version of xdr/rpc
# See if autoconf can find it; check libc, librpc, librpcsvc and libnsl
AC_SEARCH_LIBS([xdr_void],[c rpc nsl rpcsvc],
		  [dap_xdrlib=$ac_res],[dap_xdrlib=])
if test -z "$dap_xdrlib" -a "x$enable_dap" = "xyes"; then
  AC_MSG_ERROR(Cannot locate library containing xdr functions.)
else
  if test "$dap_xdrlib" = "lc" ; then
    AC_MSG_NOTICE(XDR Functions appear to be in libc.)	
  elif test "$dap_xdrlib" = "none required" ; then
    AC_MSG_NOTICE(XDR Functions appear to be in libc.)	
  else
    dap_xdrlib=`echo $dap_xdrlib | sed -e s/-l//g`
    AC_MSG_NOTICE(XDR Functions appear to be in lib${dap_xdrlib}.)	
    # Add to library list
    AC_CHECK_LIB($dap_xdrlib,xdr_void)
  fi
# Fix to get working on OS X (thanks to Fedor Baart)
AC_CHECK_HEADERS([rpc/types.h rpc/xdr.h], [
     AC_DEFINE([HAVE_RPC_TYPES_H], [], [no rpc/types.h])
     AC_DEFINE([HAVE_RPC_XDR_H], [], [no rpc/xdr.h])
     ], [], [[#ifdef HAVE_RPC_TYPES_H
# include <rpc/types.h>
#endif]])
fi

# Flags for nc-config script; by design $prefix, $includir, $libdir,
# etc.  are left as shell variables in the script so as to facilitate
# relocation
if test "x$with_netcdf_c_lib" = x ; then 
   NC_LIBS="-lnetcdf"
else
   NC_LIBS="$with_netcdf_c_lib"
fi
if test "x$enable_shared" != xyes; then
   if test "x$enable_netcdf_4" = xyes ; then 
      if test "x$HDF5DIR" != x ; then 
      	 NC_LIBS="$NC_LIBS -L$HDF5DIR/lib"
      fi	 
      NC_LIBS="$NC_LIBS -lhdf5_hl -lhdf5 $SZLIB_LIBS"
      if test "x$ZLIBDIR" != x ; then 
         NC_LIBS="$NC_LIBS -L$ZLIBDIR/lib"
      fi
      NC_LIBS="$NC_LIBS -lz -lm $LIBS"
      if test "x$SZLIBDIR" != x ; then 
         NC_LIBS="$NC_LIBS -L$SZLIBDIR/lib"
         NC_LIBS="$NC_LIBS -lsz $LIBS"
      fi
      if test "x$enable_hdf4" = xyes; then
         if test "x$HDF4DIR" != x ; then 
            NC_LIBS="$NC_LIBS -L$HDF4DIR/lib"
         fi	 
         NC_LIBS="$NC_LIBS -lmfhdf -ldf"
      fi
   fi
   if test "x$enable_dap" = xyes ; then 
       if test "x$with_daplib" != x ; then
          NC_LIBS="$NC_LIBS -L$with_daplib/lib"
       fi
       NC_LIBS="$NC_LIBS $CURL_LIBS"
   fi
fi

if test "x$enable_separate_fortran" = xyes ; then 
   NC_FLIBS="-lnetcdff $NC_LIBS"
else
   NC_FLIBS="$NC_LIBS"
fi

AC_SUBST(NC_LIBS,[$NC_LIBS])
AC_SUBST(NC_FLIBS,[$NC_FLIBS])
AC_SUBST(HAS_DAP,[$enable_dap])
AC_SUBST(HAS_NC2,[$nc_build_v2])
AC_SUBST(HAS_NC4,[$enable_netcdf_4])
AC_SUBST(HAS_HDF4,[$enable_hdf4])
AC_SUBST(HAS_HDF5,[$enable_netcdf_4])
AC_SUBST(HAS_F77,[$nc_build_f77])
AC_SUBST(HAS_F90,[$nc_build_f90])
AC_SUBST(HAS_CXX,[$nc_build_cxx])
AC_SUBST(HAS_SZLIB,[$nc_has_szlib])

AC_CONFIG_SUBDIRS([udunits libcf])

##################################################

# Uncomment this to keep a copy of autoconf defines at this point, for
# debugging purposes.
# cp confdefs.h my_config.h

AC_MSG_NOTICE([generating header files and makefiles])

# Took these out from the list below for the 4.1 release.
dnl                  win32/Makefile
dnl                  win32/NET/Makefile
dnl                  win32/NET/libsrc/Makefile
dnl                  win32/NET/ncdump/Makefile
dnl                  win32/NET/ncgen/Makefile
dnl                  win32/NET/examples/Makefile
dnl                  win32/NET/nctest/Makefile
dnl                  win32/NET/nc_test/Makefile

dnl                  cxx4/d/Makefile
dnl                  cxx4/d/html/Makefile


AC_CONFIG_FILES([Makefile
                 nc-config
                 netcdf.pc
                 fortran/Makefile
                 libsrc/Makefile
                 libsrc4/Makefile
                 nctest/Makefile
                 nc_test4/Makefile
                 nc_test/Makefile
                 ncdump/Makefile
                 ncgen3/Makefile
                 ncgen/Makefile
                 nf_test/Makefile
                 cxx/Makefile
                 cxx4/Makefile
                 f90/Makefile
                 examples/Makefile
                 examples/C/Makefile
                 examples/CDL/Makefile
                 examples/CXX/Makefile
                 examples/CXX4/Makefile
                 examples/F90/Makefile
                 examples/F77/Makefile
                  libncdap3/Makefile
                  libncdap3/oc/Makefile
                  libncdap4/Makefile
                  ncdap_test/Makefile
                  ncdap_test/testdata3/Makefile
                  ncdap_test/expected3/Makefile
                  ncdap_test/expected4/Makefile
                  ncdap_test/expectremote3/Makefile
                  ncdap_test/expectremote4/Makefile
                  ncdump/cdl4/Makefile
                  ncdump/expected4/Makefile],
                 [test -f  nc-config && chmod 755 nc-config ])

AC_OUTPUT()

