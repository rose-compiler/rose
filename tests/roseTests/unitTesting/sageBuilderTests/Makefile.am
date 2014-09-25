include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS=sageBuilderTest

sageBuilderTest_SOURCES= \
  buildTypeUnitTests.cpp \
  buildExpressionUnitTests.cpp

sageBuilderTest_CXXFLAGS= \
  -I$(top_srcdir)/tests/roseTests/unitTesting/testSupport \
  -I$(top_srcdir)/tests/roseTests/unitTesting/testSupport/gtest/include \
  $(ROSE_INCLUDES)

sageBuilderTest_LDADD= \
  $(top_builddir)/tests/roseTests/unitTesting/testSupport/libroseUnitTestSupport.la \
  $(top_builddir)/tests/roseTests/unitTesting/testSupport/gtest/lib/libgtest_main.la \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)
