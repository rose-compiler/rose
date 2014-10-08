include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#SUBDIRS=gtest

lib_LTLIBRARIES = libroseUnitTestSupport.la


$(builddir)/gtest/lib/libgtest.la :
	$(MAKE) -C  $(builddir)/gtest lib/libgtest.la



libroseUnitTestSupport_la_SOURCES= \
  testSupport.cpp

libroseUnitTestSupport_la_CXXFLAGS= \
  -I$(srcdir) \
  -I$(srcdir)/gtest/include \
  $(ROSE_INCLUDES)

libroseUnitTestSupport_la_LIBADD= \
  $(builddir)/gtest/lib/libgtest.la

