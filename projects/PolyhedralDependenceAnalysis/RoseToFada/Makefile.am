include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# . is necessary to ensure building the translator before building the tests
SUBDIRS = . #tests

CXXFLAGS += $(PIP_CPPFLAGS) $(FADA_CPPFLAGS)

INCLUDES = $(ROSE_INCLUDES) -I$(srcdir)/../Common/ -I$(srcdir)/../PMDAtoMDA/

bin_PROGRAMS = TestRoseToFadaAST

TestRoseToFadaAST_SOURCES = TestRoseToFadaAST.cpp RoseToFadaAST.cpp ../Common/RoseToFadaCommon.cpp ../PMDAtoMDA/PMDAtoMDA.cpp
TestRoseToFadaAST_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(PIP_LDFLAGS) $(FADA_LDFLAGS)

EXTRA_DIST = RoseToFadaAST.hpp

clean-local:
	rm -rf TestRoseToFadaAST *.o
