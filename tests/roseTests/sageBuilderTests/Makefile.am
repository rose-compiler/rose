include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = gtest

bin_PROGRAMS=sageBuilderTest

sageBuilderTest_SOURCES= buildTypeUnitTests.cpp

sageBuilderTest_CXXFLAGS= \
  -I$(srcdir)/gtest/include \
  $(ROSE_INCLUDES)

sageBuilderTest_LDADD= $(builddir)/gtest/lib/libgtest.la \
                       $(builddir)/gtest/lib/libgtest_main.la \
                       $(LIBS_WITH_RPATH) $(ROSE_LIBS)
