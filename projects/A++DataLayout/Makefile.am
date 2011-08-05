include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = \
	InterleaveAcrossArrays \
	Preprocessor \
	tests 

INCLUDES = $(ROSE_INCLUDES) -I$(srcdir)/includes \
	 -I$(top_srcdir)/projects/A++DataLayout/include \
	 -I$(top_srcdir)/projects/A++DataLayout/InterleaveAcrossArrays \
	 -I$(top_srcdir)/projects/A++DataLayout/Preprocessor \
	 -I$(top_srcdir)/projects/A++DataLayout/Preprocessor/ArrayAssignmentStatementQuery \
	 -I$(top_srcdir)/projects/A++DataLayout/Preprocessor/IndexOffsetQuery

CXX_TEMPLATE_REPOSITORY_PATH = .
CXXFLAGS += -DDEBUG

bin_PROGRAMS = ArrayLayoutTransformation

ArrayLayoutTransformation_SOURCES = \
				layoutTransformation.C \
				layoutOptions.C \
				transformationWorklist.C

LDADD = \
     $(LIBS_WITH_RPATH) \
     $(ROSE_LIBS) \
     $(top_builddir)/projects/A++DataLayout/InterleaveAcrossArrays/libinterleaveacrossarrays.a \
     $(top_builddir)/projects/A++DataLayout/Preprocessor/libarraypreprocessor.a \
     $(top_builddir)/projects/A++DataLayout/Preprocessor/ArrayAssignmentStatementQuery/libarrayassignmentquery.a \
     $(top_builddir)/projects/A++DataLayout/Preprocessor/IndexOffsetQuery/libindexoffsetquery.a


EXTRA_DIST = README

clean-local:
	rm -f *.o a.out rose_*.C

