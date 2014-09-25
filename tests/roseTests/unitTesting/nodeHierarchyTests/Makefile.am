include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS=nodeHierarchyTest

nodeHierarchyTest_SOURCES= \
  symbolTableUnitTests.cpp \
  functionTypeTests.cpp

nodeHierarchyTest_CXXFLAGS= \
  -I$(top_srcdir)/tests/roseTests/unitTesting/testSupport \
  -I$(top_srcdir)/tests/roseTests/unitTesting/testSupport/gtest/include \
  $(ROSE_INCLUDES)

nodeHierarchyTest_LDADD= \
  $(top_builddir)/tests/roseTests/unitTesting/testSupport/libroseUnitTestSupport.la \
  $(top_builddir)/tests/roseTests/unitTesting/testSupport/gtest/lib/libgtest_main.la \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)
