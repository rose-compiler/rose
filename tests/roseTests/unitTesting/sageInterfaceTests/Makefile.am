include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS=sageInterfaceTest

sageInterfaceTest_SOURCES= typesAreEqualUnitTests.cpp 

sageInterfaceTest_CXXFLAGS= \
  -I$(top_srcdir)/tests/roseTests/unitTesting/testSupport/gtest/include \
  $(ROSE_INCLUDES)

sageInterfaceTest_LDADD= $(top_builddir)/tests/roseTests/unitTesting/testSupport/gtest/lib/libgtest.la \
                       $(top_builddir)/tests/roseTests/unitTesting/testSupport/gtest/lib/libgtest_main.la \
                       $(LIBS_WITH_RPATH) $(ROSE_LIBS)
