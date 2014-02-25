
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

lib_LTLIBRARIES=libMDCG.la

libMDCG_la_SOURCES= \
  model.cpp \
  model-builder.cpp \
  code-generator.cpp
libMDCG_la_CXXFLAGS= \
  -g \
  -I$(srcdir)/../include \
  -I$(top_srcdir)/src/midend/MFB/include \
  -I$(top_srcdir)/src/util/Handles \
  $(ROSE_INCLUDES)
libMDCG_la_includedir=$(prefix)/include/MDCG
libMDCG_la_include_HEADERS= \
  $(srcdir)/../include/MDCG/model.hpp \
  $(srcdir)/../include/MDCG/model-base.hpp \
  $(srcdir)/../include/MDCG/model-variable.hpp \
  $(srcdir)/../include/MDCG/model-function.hpp \
  $(srcdir)/../include/MDCG/model-type.hpp\
  $(srcdir)/../include/MDCG/model-class.hpp \
  $(srcdir)/../include/MDCG/model-field.hpp \
  $(srcdir)/../include/MDCG/model-method.hpp \
  $(srcdir)/../include/MDCG/model-namespace.hpp \
  $(srcdir)/../include/MDCG/model-builder.hpp \
  $(srcdir)/../include/MDCG/code-generator.hpp


