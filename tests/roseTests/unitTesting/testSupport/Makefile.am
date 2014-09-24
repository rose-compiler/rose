include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS=gtest

bin_PROGRAMS= testSupportTest

testSupportTest_SOURCES=testSupportTest.cpp

testSupportTest_CXXFLAGS= \
  -I$(srcdir) \
  -I$(srcdir)/gtest/include \
  $(ROSE_INCLUDES)

testSupportTest_LDADD= \
  $(builddir)/gtest/lib/libgtest.la \
  $(builddir)/gtest/lib/libgtest_main.la \
  $(LIBS_WITH_RPATH) $(ROSE_LIBS)
