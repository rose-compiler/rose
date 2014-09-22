include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS=sageInterfaceTest

sageInterfaceTest_SOURCES= typesAreEqualUnitTests.cpp 

sageInterfaceTest_CXXFLAGS= \
  -I$(srcdir)/../sageBuilderTests/gtest/include \
  $(ROSE_INCLUDES)

sageInterfaceTest_LDADD= $(builddir)/../sageBuilderTests/gtest/lib/libgtest.la \
                       $(builddir)/../sageBuilderTests/gtest/lib/libgtest_main.la \
                       $(LIBS_WITH_RPATH) $(ROSE_LIBS)
