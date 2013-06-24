include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = test-loop-trees

test_loop_trees_SOURCES = test-loop-trees.cpp
test_loop_trees_CXXFLAGS = \
  -g \
  -I$(top_srcdir)/src/backend/KLT/include \
  $(ROSE_INCLUDES)
test_loop_trees_LDADD= \
  $(top_builddir)/src/backend/KLT/lib/core/libKLT-core.la \
  $(top_builddir)/src/backend/KLT/lib/sequential/libKLT-sequential.la \
  $(top_builddir)/src/backend/KLT/lib/opencl/libKLT-opencl.la \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)

LOOP_TREES_INPUTS = \
  zero-init.lt.in \
  scalar-product.lt.in \
  matrix-vector.lt.in \
  matrix-matrix.lt.in

LOOP_TREES_TARGETS = ${LOOP_TREES_INPUTS:.in=.out}

%.lt.out: %.lt.in test-loop-trees
	./test-loop-trees $< $@

check-local: test-loop-trees
	$(MAKE) $(LOOP_TREES_TARGETS)

