include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES)

noinst_LTLIBRARIES = libompLowering.la
libompLowering_la_SOURCES = omp_lowering.cpp omp_lowering.h
# avoid using libtool for libxomp.a since it will be directly linked to executable
lib_LIBRARIES = libxomp.a
libxomp_a_SOURCES = xomp.c run_me_task_defs.inc
#libxomp_a_CXXFLAGS = -pthreads

include_HEADERS = omp_lowering.h libgomp_g.h \
           libompc.h  libxomp.h libxompf.h \
	   run_me_defs.inc run_me_callers.inc

EXTRA_DIST = CMakeLists.txt README run_me_caller_generator.sh run_me_generator.sh

clean-local:
	rm -rf Templates.DB ii_files ti_files core
