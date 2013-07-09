
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libMFB-sage.la

libMFB_sage_la_SOURCES= \
  driver.cpp \
  function-declaration.cpp \
  member-function-declaration.cpp \
  class-declaration.cpp \
  variable-declaration.cpp \
  namespace-declaration.cpp
libMFB_sage_la_CXXFLAGS= \
  -g -I$(srcdir)/../../include \
  $(ROSE_INCLUDES)
libMFB_sage_la_includedir=$(prefix)/include/KLT
libMFB_sage_la_include_HEADERS= \
  $(srcdir)/../../include/MFB/multi-file-builder.hpp \
  $(srcdir)/../../include/MFB/Sage/driver.hpp \
  $(srcdir)/../../include/MFB/Sage/driver.tpp \
  $(srcdir)/../../include/MFB/Sage/function-declaration.hpp \
  $(srcdir)/../../include/MFB/Sage/member-function-declaration.hpp \
  $(srcdir)/../../include/MFB/Sage/class-declaration.hpp \
  $(srcdir)/../../include/MFB/Sage/variable-declaration.hpp \
  $(srcdir)/../../include/MFB/Sage/namespace-declaration.hpp

