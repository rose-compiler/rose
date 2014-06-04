
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libMFB-sage.la

libMFB_sage_la_SOURCES= \
  driver.cpp \
  function-declaration.cpp \
  member-function-declaration.cpp \
  class-declaration.cpp \
  variable-declaration.cpp \
  namespace-declaration.cpp \
  api.cpp
libMFB_sage_la_CXXFLAGS= \
  -g -I$(srcdir)/../../include \
  $(ROSE_INCLUDES)

