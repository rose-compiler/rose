include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# . is necessary to ensure building the translator before building the tests
SUBDIRS = . #tests

INCLUDES = $(ROSE_INCLUDES)

bin_PROGRAMS = TestPMDAtoMDA

TestPMDAtoMDA_SOURCES = TestPMDAtoMDA.cpp PMDAtoMDA.cpp
TestPMDAtoMDA_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST = PMDAtoMDA.hpp

clean-local:
	rm -rf TestPMDAtoMDA *.o
