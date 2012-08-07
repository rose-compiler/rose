
noinst_PROGRAMS=

if ROSE_HAVE_XOMP_LIB
  noinst_PROGRAMS += test
endif

test_SOURCES= \
  test.cpp \
  test-xomp-runtime.cpp
test_CXXFLAGS= \
  -I$(XOMP_INC_DIR) \
  -g -I$(top_srcdir)/include
test_LDADD= \
  ../../lib/libUPR-core.la \
  ../../lib/libUPR-Xomp.la \
  -L$(XOMP_LIB_DIR) -lxomp

../../lib/libUPR-core.la:
	make -C ../../lib libUPR-core.la
../../lib/libUPR-Xomp.la:
	make -C ../../lib libUPR-Xomp.la

if ROSE_HAVE_XOMP_LIB
check: test
	./test
endif

