
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = pocc_post_processor

LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

CXXFLAGS=-g

pocc_post_processor_SOURCES         = \
  pocc-post-processor.cpp \
  rose-utils.cpp
pocc_post_processor_CXXFLAGS        = $(CXXFLAGS) \
  -I $(srcdir) \
  -I $(srcdir)/../../src/ \
  -I$(PPL_PATH)/include \
  $(ROSE_INCLUDES)
pocc_post_processor_LDADD           = $(LDADD)
pocc_post_processor_includedir      = $(includedir)/polyhedral-model/projects
pocc_post_processor_include_HEADERS = rose-utils.hpp

../../src/libPolyModel.la:
	make -C ../../src

