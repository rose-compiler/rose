include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

BUILT_SOURCES=testSupport

testSupport:
	$(MAKE) -C $(top_builddir)/tests/testSupport/

bin_PROGRAMS=nodeHierarchyTest

nodeHierarchyTest_SOURCES= \
  symbolTableUnitTests.cpp \
  functionTypeTests.cpp

nodeHierarchyTest_CXXFLAGS= \
  -I$(top_srcdir)/tests/testSupport \
  -I$(top_srcdir)/tests/testSupport/gtest/include \
  $(ROSE_INCLUDES)

nodeHierarchyTest_LDADD= \
  $(top_builddir)/tests/testSupport/libroseUnitTestSupport.la \
  $(top_builddir)/tests/testSupport/gtest/lib/libgtest.la \
  $(top_builddir)/tests/testSupport/gtest/lib/libgtest_main.la \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)

unit-tests:
	./nodeHierarchyTest

check-local: unit-tests
