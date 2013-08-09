include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = test-loop-trees test-sequential-generator test-opencl-generator

test_loop_trees_SOURCES = test-loop-trees.cpp
test_loop_trees_CXXFLAGS = \
  -g \
  -I$(top_srcdir)/src/backend/KLT/include \
  -I$(top_srcdir)/src/backend/MFB/include \
  $(ROSE_INCLUDES)
test_loop_trees_LDADD= \
  $(top_builddir)/src/backend/KLT/lib/core/libKLT-core.la \
  $(top_builddir)/src/backend/MFB/lib/sage/libMFB-sage.la \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)

test_sequential_generator_SOURCES = test-sequential-generator.cpp build-main.cpp
test_sequential_generator_CXXFLAGS = \
  -g \
  -I$(top_srcdir)/src/backend/KLT/include \
  -I$(top_srcdir)/src/backend/MFB/include \
  -DSRCDIR=\"$(abs_srcdir)/\" \
  $(ROSE_INCLUDES)
test_sequential_generator_LDADD = \
  $(top_builddir)/src/backend/KLT/lib/core/libKLT-core.la \
  $(top_builddir)/src/backend/KLT/lib/sequential/libKLT-sequential.la \
  $(top_builddir)/src/backend/MFB/lib/sage/libMFB-sage.la \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)

test_opencl_generator_SOURCES = test-opencl-generator.cpp #build-main.cpp
test_opencl_generator_CXXFLAGS = \
  -g \
  -I$(top_srcdir)/src/backend/KLT/include \
  -I$(top_srcdir)/src/backend/MFB/include \
  -DSRCDIR=\"$(abs_srcdir)/\" \
  $(ROSE_INCLUDES)
test_opencl_generator_LDADD = \
  $(top_builddir)/src/backend/KLT/lib/core/libKLT-core.la \
  $(top_builddir)/src/backend/KLT/lib/opencl/libKLT-opencl.la \
  $(top_builddir)/src/backend/MFB/lib/sage/libMFB-sage.la \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)

$(top_builddir)/src/backend/KLT/lib/core/libKLT-core.la:
	$(MAKE) -C $(top_builddir)/src/backend/KLT/lib/core/ libKLT-core.la

$(top_builddir)/src/backend/KLT/lib/sequential/libKLT-sequential.la:
	$(MAKE) -C $(top_builddir)/src/backend/KLT/lib/sequential/ libKLT-sequential.la

$(top_builddir)/src/backend/KLT/lib/opencl/libKLT-opencl.la:
	$(MAKE) -C $(top_builddir)/src/backend/KLT/lib/opencl/ libKLT-opencl.la

$(top_builddir)/src/backend/MFB/lib/sage/libMFB-sage.la:
	$(MAKE) -C $(top_builddir)/src/backend/MFB/lib/sage/ libMFB-sage.la

LOOP_TREES_INPUTS = \
  zero-init.lt.in \
  scalar-product.lt.in \
  matrix-vector.lt.in \
  matrix-matrix.lt.in

LOOP_TREES_PROBLEMATIC_INPUTS =

LOOP_TREES_TARGETS = ${LOOP_TREES_INPUTS:.in=.out}

SEQUENTIAL_KERNEL_TARGETS = ${LOOP_TREES_INPUTS:.lt.in=.kernel.cpp}

OPENCL_KERNEL_TARGETS = ${LOOP_TREES_INPUTS:.lt.in=.kernel.cl}

%.lt.out: %.lt.in test-loop-trees
	./test-loop-trees $< $@

%.kernel.cpp: %.lt.in test-sequential-generator
	rm -f $@ ${@:.kernel.cpp=.kernel.hpp} ${@:.kernel.cpp=.main.cpp}
	./test-sequential-generator $< ${@:.kernel.cpp=}

%.kernel.cl: %.lt.in test-opencl-generator
	rm -f $@  ${@:.kernel.cl=.main.cpp}
	./test-opencl-generator $< ${@:.kernel.cl=}

check-lt-read:
	$(MAKE) $(LOOP_TREES_TARGETS)

check-sequential:
	$(MAKE) $(SEQUENTIAL_KERNEL_TARGETS)

check-opencl:
	$(MAKE) $(OPENCL_KERNEL_TARGETS)

check-local:
	$(MAKE) check-lt-read
	$(MAKE) check-sequential
	$(MAKE) check-opencl

clean-local:
	rm -f *.lt.out *.kernel.* *.main.cpp

