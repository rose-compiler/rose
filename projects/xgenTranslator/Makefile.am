# where to find rose and boost libraries 
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS =  $(ROSE_INCLUDES) -I$(srcdir)/src 

bin_PROGRAMS = xgenTranslator
xgenTranslator_SOURCES = xgenTranslator.cpp

xgenTranslator_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) 

## Test input testcode with manual instrumentation already
CXX_TESTCODE= \
    test1.cpp

CXX_TESTCODE_RESULTS = $(addprefix rose_, ${CXX_TESTCODE})

$(CXX_TESTCODE_RESULTS): xgenTranslator
	./xgenTranslator -c -rose:Cxx11_only -rose:unparse_template_ast $(srcdir)/$(@:rose_%=%)

check: $(CXX_TESTCODE_RESULTS) 
clean:
	rm -rf rose_*.cpp *.o xgenTranslator

EXTRA_DIST= matmulcpp11-gcc-E-v1.cxx orig.Makefile readme 
