include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs


AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(srcdir) \
	 -I$(top_srcdir)/projects/DataFaultTolerance/includes


CXX_TEMPLATE_REPOSITORY_PATH = .
#CXXFLAGS += -DDEBUG

bin_PROGRAMS = DataFaultToleranceTransformation

DataFaultToleranceTransformation_SOURCES = \
				faultToleranceArrayPass.C \
				AstFromString.C \
				pragmaHandling.C \
				arrayBase.C

include_HEADERS = \
		arrayBase.h \
		AstFromString.h \
		faultToleranceArrayLibUtility.h \
		pragmaHandling.h

noinst_LIBRARIES = libfaulttolerancearray.a

libfaulttolerancearray_a_SOURCES = \
			faultToleranceArrayLib.C \
			faultToleranceArrayLibUtility.C
			
libfaulttolerancearray_a_includedir = $(includedir)/src

libfaulttolerancearray_a_include_HEADERS = \
     faultToleranceArrayLibUtility.h

LDADD = \
     $(LIBS_WITH_RPATH) \
     $(ROSE_LIBS) 



clean-local:
	rm -f *.o a.out rose_*.C

