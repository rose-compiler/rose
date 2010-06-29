include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# . is necessary to ensure building the translator before building the tests
SUBDIRS = . #tests

CXXFLAGS += $(PIP_CPPFLAGS) $(FADA_CPPFLAGS) $(PPL_CPPFLAGS)

INCLUDES = $(ROSE_INCLUDES) -I$(srcdir)/../Common/ -I$(srcdir)/../PMDAtoMDA/ -I$(srcdir)/../RoseToFada/

bin_PROGRAMS = TestRoseToPPL

TestRoseToPPL_SOURCES = TestRoseToPPL.cpp FadaToPPL.cpp ../RoseToFada/RoseToFadaAST.cpp ../Common/RoseToFadaCommon.cpp ../PMDAtoMDA/PMDAtoMDA.cpp
TestRoseToPPL_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(PIP_LDFLAGS) $(FADA_LDFLAGS) $(PPL_LDFLAGS)

EXTRA_DIST = FadaToPPL.hpp

clean-local:
	rm -rf TestRoseToPPL *.o
