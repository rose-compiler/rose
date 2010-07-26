include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

# . is necessary to ensure building the translator before building the tests
SUBDIRS = . #tests

INCLUDES = $(ROSE_INCLUDES) -I$(srcdir)/../Common/ -I$(srcdir)/../PMDAtoMDA/ -I$(srcdir)/../RoseToFada/ -I$(srcdir)/../RoseToPPL/ -I$(srcdir)/../Schedule/

bin_PROGRAMS = TestCodeGenerator

TestCodeGenerator_SOURCES = TestCodeGenerator.cpp CodeGenerator.cpp ../Schedule/Schedule.cpp ../Schedule/ConvexHullInterior.cpp ../RoseToPPL/FadaToPPL.cpp ../RoseToFada/RoseToFadaAST.cpp ../Common/RoseToFadaCommon.cpp ../PMDAtoMDA/PMDAtoMDA.cpp
TestCodeGenerator_LDADD = $(LIBS_WITH_RPATH) $(ROSE_LIBS) $(PIP_LDFLAGS) $(FADA_LDFLAGS) $(PPL_LDFLAGS)

EXTRA_DIST = CodeGenerator.hpp

clean-local:
	rm -rf TestCodeGenerator *.o
