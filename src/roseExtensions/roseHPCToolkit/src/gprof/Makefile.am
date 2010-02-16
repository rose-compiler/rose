# ROSE-HPCT Sage IR interface -- source files

include ../../Make.inc

if ROSE_BUILD_ROSEHPCT

noinst_LTLIBRARIES = libgprof.la

# DQ (2/10/2010): ROSE currently fails to generate code for this file (it does however compile for analysis only).
# libgprof_la_SOURCES = gprof_info.cc
libgprof_la_SOURCES = 
if !ROSE_USING_ROSE_CODE_GENERATION
   libgprof_la_SOURCES += gprof_info.cc
endif

else # !ROSE_BUILD_ROSEHPCT

EXTRA_DIST = CMakeLists.txt gprof_info.cc

endif

# eof
