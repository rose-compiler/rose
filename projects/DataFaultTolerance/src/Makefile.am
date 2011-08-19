include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs


INCLUDES = $(ROSE_INCLUDES) -I$(srcdir)/includes \
	 -I$(top_srcdir)/projects/DataFaultTolerance/includes

Other_SOURCES = faultToleranceArrayLibUtility.C	pragmaHandling.C 


CXX_TEMPLATE_REPOSITORY_PATH = .
CXXFLAGS += -DDEBUG

bin_PROGRAMS = DataFaultToleranceTransformation

DataFaultToleranceTransformation_SOURCES = \
				faultToleranceArrayPass.C \
				AstFromString.C 

noinst_LIBRARIES = libfaulttolerancearray.a

libfaulttolerancearray_a_SOURCES = \
			faultToleranceArrayLib.C

LDADD = \
     $(LIBS_WITH_RPATH) \
     $(ROSE_LIBS) 


EXTRA_DIST = README

clean-local:
	rm -f *.o a.out rose_*.C

