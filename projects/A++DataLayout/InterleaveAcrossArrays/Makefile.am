include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

INCLUDES = $(ROSE_INCLUDES) -I$(srcdir)/includes \
	 -I$(top_srcdir)/projects/A++DataLayout \
	 -I$(top_srcdir)/projects/A++DataLayout/include \
	 -I$(top_srcdir)/projects/A++DataLayout/InterleaveAcrossArrays \
	 -I$(top_srcdir)/projects/A++DataLayout/InterleaveArray

CXX_TEMPLATE_REPOSITORY_PATH = .

CXXFLAGS += -DDEBUG

noinst_LIBRARIES = libinterleaveacrossarrays.a

libinterleaveacrossarrays_a_SOURCES = \
			interleaveAcrossArraysCheck.C \
			interleaveAcrossArraysTransformation.C 

libinterleaveacrossarrays_a_includedir = $(includedir)/InterleaveAcrossArrays

libinterleaveacrossarrays_a_include_HEADERS = \
	interleaveAcrossArraysCheck.h \
	interleaveAcrossArraysTransformation.h 

EXTRA_DIST = README

clean-local:
	rm -f *.o a.out rose_*.C

