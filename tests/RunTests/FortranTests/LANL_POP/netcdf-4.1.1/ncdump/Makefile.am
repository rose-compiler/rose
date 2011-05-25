## This is a automake file, part of Unidata's netCDF package.
# Copyright 2005, see the COPYRIGHT file for more information.
# This file builds and runs the ncdump program.

# $Id: Makefile.am,v 1.141 2010/03/25 20:43:12 dmh Exp $

LDADD = -lm

AM_LDFLAGS =

CLEANFILES = test0.nc test1.cdl test1.nc test2.cdl ctest1.cdl ctest.c	\
ctest64.c ctest0.nc ctest0_64.nc c1.cdl ctest1_64.cdl c0.nc small.nc	\
small2.nc c0tmp.nc c1.ncml utf8.cdl utf8_64.cdl utf8.nc utf8_64.nc	\
tmp.cdl \
tst_vlen_data.nc tst_utf8.nc tst_special_atts.nc tst_unicode.nc \
tst_solar_2.nc tst_string_data.nc tst_calendars.nc tst_nans.nc \
tst_opaque_data.nc tst_solar_cmp.nc tst_enum_data.nc tst_solar_1.nc \
tst_mslp_64.nc tst_mslp.nc tst_comp2.nc tst_ncml.nc \
tst_fillbug.nc tst_group_data.nc tst_small.nc tst_comp.nc \
tst_unicode.cdl tst_group_data.cdl tst_compounds2.cdl tst_comp.cdl \
tst_enum_data.cdl tst_small.cdl tst_times.cdl tst_solar_2.cdl \
tst_string_data.cdl tst_fillbug.cdl tst_opaque_data.cdl tst_compounds4.cdl \
tst_utf8.cdl tst_compounds3.cdl tst_special_atts.cdl tst_nans.cdl \
tst_format_att_64.cdl tst_vlen_data.cdl tst_solar_1.cdl tst_format_att.cdl

# These tests are run for both netCDF-4 and non-netCDF-4 builds.
check_PROGRAMS = rewrite-scalar ctest ctest64 ncdump tst_utf8
TESTS = run_tests.sh tst_64bit.sh ctest ctest64 tst_output.sh	\
tst_lengths.sh tst_calendars.sh tst_utf8 run_utf8_tests.sh

AM_CPPFLAGS = -I$(top_srcdir)/nc_test 

# Is the user building netCDF-4?
if USE_NETCDF4

# Link to the netCDF-4, (optionally) HDF4, and the HDF5 libraries.
LDADD += ../libsrc4/libnetcdf.la 
if USE_HDF4
LDADD += -lmfhdf -ldf -ljpeg
endif # USE_HDF4
LDADD += -lhdf5_hl -lhdf5 -lz 

if USE_PNETCDF
LDADD += -lpnetcdf
endif

# If the user specified a prefix for the HDF4 library location, use it.
if USE_HDF4_DIR
AM_CPPFLAGS += -I@HDF4DIR@/include
AM_LDFLAGS += -L@HDF4DIR@/lib
endif

# Find netcdf.h in libsrc4.
AM_CPPFLAGS += -I../libsrc4 -I$(top_srcdir)/libsrc4

# If the user specified a root for HDF5, use it.
if USE_HDF5_DIR
AM_CPPFLAGS += -I@HDF5DIR@/include
AM_LDFLAGS += -L@HDF5DIR@/lib
endif

# If the user specified a root location for ZLIB, use it.
if USE_ZLIB_DIR
AM_CPPFLAGS += -I@ZLIBDIR@/include
AM_LDFLAGS += -L@ZLIBDIR@/lib
endif

# If the user specified a root location for SZLIB, use it.
if USE_SZLIB_DIR
AM_CPPFLAGS += -I@SZLIBDIR@/include
AM_LDFLAGS += -L@SZLIBDIR@/lib
endif

if USE_PNETCDF_DIR
AM_CPPFLAGS += -I@PNETCDFDIR@/include
AM_LDFLAGS += -L@PNETCDFDIR@/lib
endif

# NetCDF-4 has some extra tests.
check_PROGRAMS += tst_create_files tst_h_rdc0 tst_group_data		\
tst_enum_data tst_opaque_data tst_string_data tst_vlen_data tst_comp	\
tst_comp2 tst_nans tst_special_atts tst_unicode tst_fillbug

TESTS += tst_create_files tst_group_data tst_enum_data tst_opaque_data	\
tst_string_data tst_vlen_data tst_comp tst_comp2 tst_nans		\
tst_special_atts tst_netcdf4.sh tst_h_rdc0 tst_unicode tst_fillbug	\
tst_fillbug.sh tst_netcdf4_4.sh tst_nccopy4.sh 

if EXTRA_TESTS
TESTS += run_back_comp_tests.sh
endif # EXTRA_TESTS

tst_h_rdc0_CPPFLAGS = -I../nc_test ${AM_CPPFLAGS}

else # not USE_NETCDF4
TESTS += tst_nccopy3.sh

# Find netcdf.h in libsrc.
AM_CPPFLAGS += -I../libsrc -I$(top_srcdir)/libsrc

# Link to netcdf-3 library.
LDADD +=  ../libsrc/libnetcdf.la

endif # USE_NETCDF4

if BUILD_DAP
AM_CPPFLAGS += @CURL_CFLAGS@
AM_LDFLAGS += @CURL_LIBS@
endif

# This is the program we're building, and it's sources.
bin_PROGRAMS = ncdump 
ncdump_SOURCES = ncdump.c vardata.c dumplib.c indent.c nctime.c	\
ncdump.h vardata.h dumplib.h indent.h isnan.h nctime.h cdl.h

# Another utility program that copies any netCDF file using only the
# netCDF API
bin_PROGRAMS += nccopy
nccopy_SOURCES = nccopy.c nciter.c nciter.h

# This is the man page.
man_MANS = ncdump.1 nccopy.1

# These files all have to be included with the distribution.
EXTRA_DIST = run_tests.sh tst_64bit.sh tst_output.sh test0.cdl		\
ref_ctest1_nc4.cdl ref_ctest1_nc4c.cdl ref_tst_solar_1.cdl		\
ref_tst_solar_2.cdl tst_netcdf4.sh tst_netcdf4_4.sh ref_tst_small.cdl	\
tst_lengths.sh tst_ncml.cdl ref1.ncml ref_tst_group_data.cdl		\
ref_tst_enum_data.cdl ref_tst_opaque_data.cdl ref_tst_string_data.cdl	\
ref_tst_vlen_data.cdl ref_tst_comp.cdl ref_tst_unicode.cdl		\
ref_tst_nans.cdl small.cdl small2.cdl $(man_MANS) run_utf8_tests.sh	\
ref_tst_utf8.cdl ref_tst_fillbug.cdl tst_fillbug.sh tst_calendars.cdl	\
tst_calendars.sh ref_times.cdl ref_tst_special_atts.cdl			\
ref_tst_noncoord.cdl ref_tst_compounds2.nc ref_tst_compounds2.cdl	\
ref_tst_compounds3.nc ref_tst_compounds3.cdl ref_tst_compounds4.nc	\
ref_tst_compounds4.cdl ref_tst_group_data_v23.cdl tst_mslp.cdl		\
ref_tst_format_att.cdl ref_tst_format_att_64.cdl tst_nccopy3.sh		\
tst_nccopy4.sh ref_nc_test_netcdf4_4_0.nc run_back_comp_tests.sh	\
ref_nc_test_netcdf4.cdl

BUILT_SOURCES = ctest.c ctest64.c

# Can't run ncgen to generate ctest.c and ctest64.c on cross-compiles.
if CROSS_COMPILING
	cp ref_ctest.c ctest.c
	cp ref_ctest64.c ctest64.c
else
ctest.c:
	$(top_builddir)/ncgen/ncgen -c -o ctest0.nc $(srcdir)/../ncgen/c0.cdl > $(srcdir)/ctest.c

ctest64.c:
	$(top_builddir)/ncgen/ncgen -v2 -c -o ctest0_64.nc $(srcdir)/../ncgen/c0.cdl > $(srcdir)/ctest64.c
endif # CROSS_COMPILING

# NCGEN4 additions
SUBDIRS=cdl4 expected4
EXTRA_DIST += tst_ncgen4_shared.sh tst_ncgen4.sh tst_ncgen4_classic.sh	\
tst_ncgen4_diff.sh tst_ncgen4_cycle.sh ref_ctest.c ref_ctest64.c

CLEANFILES += results/*.nc results/*.dmp results/*.dmp2 tmp*.cdl
TESTS += tst_ncgen4_classic.sh
if USE_NETCDF4
TESTS += tst_ncgen4.sh
endif

test: check
