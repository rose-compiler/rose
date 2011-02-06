include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# DQ (9/3/2006): Reorganized the lists of EXTRA_DIST and include_HEADERS.
# DQ (9/2/2006): Removed most of these files since I don't think that these are really required
include_HEADERS = \
     rose_stdarg.h \
     rose_varargs.h \
     rose-g++-headerfilefixup.h \
     rose_specific_cdefs.h

# DQ (9/15/2010): Added UPC header files.
# DQ (8/14/2010): Added modified header file iomanip-gnu-4.5 to fix code in new 4.5 header files 
# that ROSE can't compile and which appear to be C++0X specific code included by mistake.
# DQ (9/3/2006): Reorganized the lists of EXTRA_DIST and include_HEADERS.
# Note that without the file: config.guess, depend, install-sh, and dirincludes;
# the configuration will exit and the backend specific compiler header file are not built.
# DQ (10/22/2003) Support for GNU header file processing (including STL header file processing)
EXTRA_DIST = CMakeLists.txt \
     rose_edg_required_macros_and_functions.h.cmake \
     Makefile.for.ROSE.includes.and.libs \
     QMTest_makefile.inc \
     gnu-3.x-headerfile-fixup.pl \
     rose_edg_required_macros_and_functions.h.in \
     config.guess \
     depend \
     install-sh \
     dirincludes \
     create_system_headers \
     macro_names_to_remove \
     cleanConfigGuessOutput \
     get_compiler_header_dirs \
     iomanip-gnu-4.5 \
     upc.h upc_relaxed.h upc_strict.h upc_collective.h upc_io.h bupc_extensions.h \
     automake_conditional_display_makefile

# DQ (9/3/2006): I think that if we have config.guess, depend, install-sh, and dirincludes; then we don't need "missing"
# missing

# DQ (9/2/2006): These appear to only be required for development versions or ROSe (in CVS)
# RV (9/12/2005): Removed 'mkinstalldirs', unnecessary with recent AutoTools versions.
FILES_NOT_REQUIRED_FOR_DISTRIBUTIONS = \
     acx_libxml2.m4 \
     choose-backend-compiler.m4 \
     compiler-defs.m4 \
     EDG.m4 \
     enable-rosehpct.m4 \
     generate-backend-compiler-specific-headers.m4 \
     qrose.m4 \
     ROSE.m4 \
     support-doxygen.m4 \
     support-i386.m4 \
     support-insure.m4 \
     support-maple.m4 \
     support-omp.m4 \
     support-gcc-omp.m4 \
     support-purify.m4 \
     support-rpath.m4 \
     support-sqlite.m4 \
     support-visualization.m4 \
     use_edg_source_code.m4 \
     wave.m4 \
     config.guess \
     depend \
     install-sh \
     dirincludes \
     create_system_headers

# DQ (9/2/2006): I don't think these are required except in the development version of ROSE
REMOVED_FROM_EXTRA_DIST = \
     enable-rosehpct.m4 \
     acx_libxml2.m4

