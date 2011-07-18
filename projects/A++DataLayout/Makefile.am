include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = \
	InterleaveAcrossArrays

INCLUDES = $(ROSE_INCLUDES) -I$(srcdir)/includes \
	 -I$(top_srcdir)/projects/A++DataLayout/include \
	 -I$(top_srcdir)/projects/A++DataLayout/InterleaveAcrossArrays

CXX_TEMPLATE_REPOSITORY_PATH = .
CXXFLAGS += -DDEBUG

bin_PROGRAMS = ArrayLayoutTransformation

ArrayLayoutTransformation_SOURCES = layoutTransformation.C \
				transformationOptions.C \
				transformationWorklist.C

LDADD = \
     $(LIBS_WITH_RPATH) \
     $(ROSE_LIBS) \
     $(top_builddir)/projects/A++DataLayout/interleaveAcrossArrays/libinterleaveacrossarrays.a

include_HEADERS = 

EXTRA_DIST = README

clean-local:
	rm -f *.o a.out rose_*.C

