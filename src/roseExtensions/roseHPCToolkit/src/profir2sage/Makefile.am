# ROSE-HPCT Profile IR--to-Sage IR conversion -- source files

include ../../Make.inc

if ROSE_BUILD_ROSEHPCT

noinst_LTLIBRARIES = libprofir2sage.la

# DQ (2/10/2010): ROSE currently fails to generate code for this file (it does however compile for analysis only).
# libprofir2sage_la_SOURCES = attach.cc
libprofir2sage_la_SOURCES = 
if !ROSE_USING_ROSE_CODE_GENERATION
   libprofir2sage_la_SOURCES += attach.cc
endif

else # !ROSE_BUILD_ROSEHPCT

EXTRA_DIST = CMakeLists.txt attach.cc

endif

# eof
