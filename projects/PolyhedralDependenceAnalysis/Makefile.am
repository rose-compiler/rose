include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# . is necessary to ensure building the translator before building the tests
SUBDIRS = . tests

CXXFLAGS += $(PIP_CPPFLAGS) $(FADA_CPPFLAGS) $(PPL_CPPFLAGS)

INCLUDES = $(ROSE_INCLUDES)

bin_PROGRAMS = TestRoseToFada TestRoseToPPL

TestRoseToFada_SOURCES = TestRoseToFada.cpp RoseToFadaCommon.cpp RoseToFadaAST.cpp
TestRoseToFada_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(PIP_LDFLAGS) $(FADA_LDFLAGS)

TestRoseToPPL_SOURCES = TestRoseToPPL.cpp RoseToFadaCommon.cpp RoseToFadaAST.cpp FadaToPPL.cpp
TestRoseToPPL_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(PIP_LDFLAGS) $(FADA_LDFLAGS) $(PPL_LDFLAGS)

EXTRA_DIST = RoseToFadaAST.hpp RoseToFadaCommon.hpp FadaToPPL.hpp

clean-local:
	rm -rf TestRoseToFada TestRoseToPPL *.o

