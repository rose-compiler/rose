# ROSE-HPCT Sage IR interface -- source files

include ../../Make.inc

if ROSE_BUILD_ROSEHPCT

noinst_LTLIBRARIES = libsage.la
libsage_la_SOURCES = collect.cc  depth.cc  metric.cc  propagate.cc

else # !ROSE_BUILD_ROSEHPCT

EXTRA_DIST = CMakeLists.txt collect.cc  depth.cc  metric.cc  propagate.cc

endif

# eof
