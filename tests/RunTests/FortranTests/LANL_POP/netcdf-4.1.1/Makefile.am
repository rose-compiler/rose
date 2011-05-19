## This is a automake file, part of Unidata's netCDF package.
# Copyright 2005, see the COPYRIGHT file for more information.

# This is the main automake file for netCDF. It builds the different
# netcdf directories. Not all directories are built, depending on the
# options selected during configure.

# $Id: Makefile.am,v 1.130 2010/02/05 19:07:21 ed Exp $

# This directory stores libtool macros, put there by aclocal.
ACLOCAL_AMFLAGS = -I m4

# These files get added to the distribution.
EXTRA_DIST = README COPYRIGHT RELEASE_NOTES INSTALL test_prog.c	\
test_prog.F test_prog.f90

pkgconfigdir=$(libdir)/pkgconfig
pkgconfig_DATA = netcdf.pc

# This will go to the man directory and build, if necessary, the
# netcdf-install.txt file, which will be copied into the main
# directory, renamed "INSTALL".
INSTALL:

# If the user wants the F77 API this will cause it to be built and
# tested.
if BUILD_F77
F77_DIR = fortran
F77_TEST = nf_test
endif

# Does the user want C API?
if BUILD_C

LIBSRC_DIR = libsrc
NC_TEST_DIR = nc_test

# Does the user want to build the V2 API?
if BUILD_V2
V2_TEST = nctest
endif

# Does the user want C++ API?
if BUILD_CXX
CXX_DIR = cxx
endif

# Does the user want to build ncgen/ncdump?
if BUILD_UTILITIES
NCGEN3 = ncgen3
NCGEN = ncgen
NCDUMP = ncdump
endif

# Does the user want to build the examples?
if BUILD_EXAMPLES
EXAMPLES = examples
endif

# Is the user building netCDF-4?
if USE_NETCDF4
LIBSRC4_DIR = libsrc4
NC_TEST4 = nc_test4
endif

endif # BUILD_C

# Does the user want F90 API?
if BUILD_F90
F90_DIR = f90
endif

# Build the new netCDF-4 C++ API?
if BUILD_CXX4
CXX4 = cxx4
endif

# Build the opendap client?
if BUILD_DAP
NCDAP3=libncdap3
if BUILD_UTILITIES
NCDAPTESTDIR=ncdap_test
endif # BUILD_UTILITIES
NCDAP4=libncdap4
endif

# Build UDUNITS?
if BUILD_UDUNITS
UDUNITS=udunits
endif

# Build libcf?
if BUILD_LIBCF
LIBCF=libcf
endif

#WIN32=win32

# This is the list of subdirs for which Makefiles will be constructed
# and run. ncgen must come before ncdump and cxx, because their tests
# depend on it. nf_test depends upon ncgen as well. NC3DAPDIR must be
# built right before LIBSRC_DIR and NC4DAPDIR must be 
# built right before LIBSRC4_DIR.
if BUILD_SEPARATE_FORTRAN
SUBDIRS = $(UDUNITS) $(NCDAP3) $(LIBSRC_DIR) $(NCDAP4) $(LIBSRC4_DIR) $(F90_DIR)	\
$(F77_DIR) $(V2_TEST) $(NCGEN3) $(NCGEN) $(NCDUMP) $(F77_TEST)		\
${CXX_DIR} $(CXX4) $(NC_TEST_DIR) $(NC_TEST4) $(NCDAPTESTDIR)	\
$(EXAMPLES) $(WIN32) $(LIBCF)
else
SUBDIRS = $(UDUNITS) $(F90_DIR) $(F77_DIR)  $(NCDAP3) $(LIBSRC_DIR) 	\
$(NCDAP4) $(LIBSRC4_DIR) $(V2_TEST) $(NCGEN3) $(NCGEN) $(NCDUMP) $(F77_TEST)	\
${CXX_DIR} $(CXX4) $(NC_TEST_DIR) $(NC_TEST4) $(NCDAPTESTDIR)	\
$(EXAMPLES) $(WIN32) $(LIBCF)
endif

# make test should do the same as make check.
test: check

# Remove these generated files, for a distclean.
DISTCLEANFILES = VERSION comps.txt test_prog

# The nc-config script helps the user build programs with netCDF.
bin_SCRIPTS = nc-config

# What needs to go in the binrary dist?
BINFILES = README_BINARIES.txt
if BUILD_C
#BINFILES += include/netcdf.h share/man/man3/netcdf.3 lib/libnetcdf.a   # ROSE POP TEST
BINFILES += include/netcdf.h lib/libnetcdf.a
endif

if BUILD_UTILITIES
BINFILES += bin/ncgen3$(EXEEXT) bin/ncgen$(EXEEXT) bin/ncdump$(EXEEXT)	\
share/man/man1/ncgen.1 share/man/man1/ncdump.1
endif

if BUILD_F77
BINFILES += include/netcdf.inc share/man/man3/netcdf_f77.3 
if BUILD_SEPARATE_FORTRAN
BINFILES += lib/libnetcdff.a
endif
endif

if BUILD_F90
if UPPER_CASE_MOD
BINFILES += include/NETCDF.mod include/TYPESIZES.mod
else
BINFILES += include/netcdf.mod include/typesizes.mod
endif
BINFILES += share/man/man3/netcdf_f90.3
endif

if BUILD_CXX
BINFILES += include/netcdf.hh include/ncvalues.h  include/netcdfcpp.h lib/libnetcdf_c++.a 
endif

# At Unidata, package up binaries.
ftpbin: install
	echo "Getting binaries from ${prefix}"
	ls -l ${prefix}
	echo "These are the binaries for netCDF @PACKAGE_VERSION@." > ${prefix}/README_BINARIES.txt
	echo "For this build: CC=$(CC) CXX=$(CXX) FC=$(FC) F77=$(F77) $F90=$(F90)" >> ${prefix}/README_BINARIES.txt
	echo "CFLAGS=$(CFLAGS) CXXFLAGS=$(CXXFLAGS)" >> ${prefix}/README_BINARIES.txt
	echo "FCFLAGS=$(FCFLAGS) F77FLAGS=$(F77FLAGS) $F90FLAGS=$(F90FLAGS)" >> ${prefix}/README_BINARIES.txt
	which tar
	echo "PATH: $PATH"
	tar cf @BINFILE_NAME@ -C ${prefix} ${BINFILES}
	gzip -f @BINFILE_NAME@

check_nc_config:
	$(CC) `./nc-config --cflags` test_prog.c -o test_prog `./nc-config --libs`
	./test_prog
	$(F77) `./nc-config --fflags` test_prog.F -o test_prog `./nc-config --libs`
	./test_prog
	$(FC) `./nc-config --fflags` test_prog.f90 -o test_prog `./nc-config --libs`
	./test_prog

install-data-hook:
	@echo ''
	@echo '+-------------------------------------------------------------+'
	@echo '| Congratulations! You have successfully installed netCDF!    |'
	@echo '|                                                             |'
	@echo '| You can use script "nc-config" to find out the relevant     |'
	@echo '| compiler options to build your application. Enter           |'
	@echo '|                                                             |'
	@echo '|     nc-config --help                                        |'
	@echo '|                                                             |'
	@echo '| for additional information.                                 |'
	@echo '|                                                             |'
	@echo '| CAUTION:                                                    |'
	@echo '|                                                             |'
	@echo '| If you have not already run "make check", then we strongly  |'
	@echo '| recommend you do so. It does not take very long.            |'
	@echo '|                                                             |'
	@echo '| Before using netCDF to store important data, test your      |'
	@echo '| build with "make check".                                    |'
	@echo '|                                                             |'
	@echo '| NetCDF is tested nightly on many platforms at Unidata       |'
	@echo '| but your platform is probably different in some ways.       |'
	@echo '|                                                             |'
	@echo '| If any tests fail, please see the netCDF web site:          |'
	@echo '| http://www.unidata.ucar.edu/software/netcdf/                |'
	@echo '|                                                             |'
	@echo '| NetCDF is developed and maintained at the Unidata Program   |'
	@echo '| Center. Unidata provides a broad array of data and software |' 
	@echo '| tools for use in geoscience education and research.         |'
	@echo '| http://www.unidata.ucar.edu                                 |'
	@echo '+-------------------------------------------------------------+'
	@echo ''

# If building the dll on windows, with MingW, this extra target
# # packages the results.
# win32_bin:
# 	@echo 'This netCDF DLL was generated using MingW.' > README_DLL.txt
# 	@echo '' >> README_DLL.txt
# 	@echo './configure --enable-dll --enable-shared --disable-separate-fortram --disable-cxx --disable-f90' >> README_DLL.txt
# 	@echo 'To use the DLL from C, include netcdf.h and set pre-processor macro DLL_NETCDF.' >> README_DLL.txt
# 	@echo 'To use the DLL from Fortran, include netcdf.inc.' >> README_DLL.txt
# 	cd libsrc/.libs
# 	lib /machine:i386 /def:libnetcdf.def
# 	ls
# 	cd ../..
# 	cp libsrc/.libs/netcdf.lib .
# 	cp libsrc/netcdf.h .
# 	cp fortran/netcdf.inc .
# 	cp ncgen3/.libs/ncgen3.exe .
# 	cp ncgen/.libs/ncgen.exe .
# 	cp ncdump/.libs/ncdump.exe .
# 	cp libsrc/.libs/libnetcdf-4.dll .
# 	cp libsrc/.libs/libnetcdf.a .
# 	cp libsrc/.libs/libnetcdf.dll.a .
# 	tar -cf win32_dll_$(VERSION).tar netcdf.h libnetcdf-4.dll README_DLL.txt netcdf.inc ncgen3.exe ncgen.exe ncdump.exe netcdf.lib
# 	gzip -f win32_dll_$(VERSION).tar

# # If building the dll on windows, with Cygwin, using the -mno-cygwin
# # option, this extra target packages the results.
# win32_bin_nocygwin:
# 	@echo 'This netCDF DLL was generated using MingW.' > README_DLL.txt
# 	@echo '' >> README_DLL.txt
# 	@echo './configure --enable-dll --enable-shared --disable-separate-fortram --disable-cxx --disable-f90' >> README_DLL.txt
# 	@echo 'To use the DLL from C, include netcdf.h and set pre-processor macro DLL_NETCDF.' >> README_DLL.txt
# 	@echo 'To use the DLL from Fortran, include netcdf.inc.' >> README_DLL.txt
# 	cp libsrc/netcdf.h .
# 	cp fortran/netcdf.inc .
# 	cp ncgen3/.libs/ncgen3.exe .
# 	cp ncgen/.libs/ncgen.exe .
# 	cp ncdump/.libs/ncdump.exe .
# 	cp libsrc/.libs/cygnetcdf-4.dll libnetcdf-4.dll
# 	cp libsrc/.libs/libnetcdf.a .
# 	cp libsrc/.libs/libnetcdf.dll.a .
# 	tar -cf win32_dll_$(VERSION).tar netcdf.h libnetcdf-4.dll README_DLL.txt netcdf.inc ncgen3.exe ncgen.exe ncdump.exe
# 	gzip -f win32_dll_$(VERSION).tar

# # If building the dll on windows, with visual studio, this extra
# # target packages the results.
# win32_vs_bin:
# 	@echo 'This netCDF DLL was generated using visual studio.' > README_DLL.txt
# 	@echo '' >> README_DLL.txt
# 	@echo 'To use the DLL from C, include netcdf.h and set pre-processor macro DLL_NETCDF.' >> README_DLL.txt
# 	@echo 'To use the DLL from Fortran, include netcdf.inc.' >> README_DLL.txt
# 	cp libsrc4/netcdf.h .
# 	cp fortran/netcdf.inc .
# 	cp win32/NET/Debug/ncgen.exe .
# 	cp win32/NET/Debug/ncdump.exe .
# 	cp win32/NET/Debug/netcdf.dll .
# 	cp win32/NET/Debug/netcdf.exp .
# 	cp win32/NET/Debug/netcdf.lib .
# 	zip win32_vs_dll_$(VERSION).zip netcdf.h netcdf.dll netcdf.lib netcdf.exp README_DLL.txt netcdf.inc ncgen.exe ncdump.exe

