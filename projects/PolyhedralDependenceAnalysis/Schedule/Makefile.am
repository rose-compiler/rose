include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# . is necessary to ensure building the translator before building the tests
SUBDIRS = . #tests

CXXFLAGS += $(PIP_CPPFLAGS) $(FADA_CPPFLAGS) $(PPL_CPPFLAGS)

INCLUDES = $(ROSE_INCLUDES) -I$(srcdir)/../Common/ -I$(srcdir)/../PMDAtoMDA/ -I$(srcdir)/../RoseToFada/ -I$(srcdir)/../RoseToPPL/

bin_PROGRAMS = TestSchedulePPL

TestSchedulePPL_SOURCES = TestSchedulePPL.cpp Schedule.cpp ../RoseToPPL/FadaToPPL.cpp ../RoseToFada/RoseToFadaAST.cpp ../Common/RoseToFadaCommon.cpp ../PMDAtoMDA/PMDAtoMDA.cpp
TestSchedulePPL_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(PIP_LDFLAGS) $(FADA_LDFLAGS) $(PPL_LDFLAGS)

EXTRA_DIST = Schedule.hpp

clean-local:
	rm -rf TestRoseToFadaAST *.o
