include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = 

INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/projects/backstroke
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

# Build a library of all the reverser code
noinst_LTLIBRARIES =  libBackstrokeReverser.la

libBackstrokeReverser_ladir = .
libBackstrokeReverser_la_DEPENDENCIES = 
libBackstrokeReverser_la_SOURCES = eventReverser.C expAndStmtProcessor.C facilityBuilder.C CFGReverserProofOfConcept.C stateSaver.C 
libBackstrokeReverser_la_includedir = $(includedir)/backstroke
libBackstrokeReverser_la_include_HEADERS = eventReverser.h facilityBuilder.h CFGReverserProofOfConcept.h stateSaver.h

# Build the reverse code generator
noinst_PROGRAMS = reverse stateSaving 

reverse_SOURCES = reverseCodeGeneratorMain.C
reverse_LDADD =  libBackstrokeReverser.la $(top_builddir)/projects/backstroke/utilities/libBackstrokeUtilities.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

stateSaving_SOURCES = stateSavingMain.C
stateSaving_LDADD = libBackstrokeReverser.la $(top_builddir)/projects/backstroke/utilities/libBackstrokeUtilities.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST = 

CLEANFILES = 

ALL_TESTCODES =

TEST_Objects=test0.C test1.C test2.C test3.C 

$(TEST_Objects): reverse
	@cp $(srcdir)/reverseCodeTest/$(@:.o=.C) $(@:.o=.C)
#./reverse -I$(srcdir)/test -c $(@:.o=.C) -DROSE_LANGUAGE_MODE=0       		
	./reverse -I$(srcdir)/reverseCodeTest -c $(@:.o=.C)
	@g++ -I$(srcdir)/reverseCodeTest rose_$(@:.o=.C) -o $(@:.o=.C).o -w
	./$(@:.o=.C).o		

check-local:
	@echo "*********************************************************************************************************"
	@echo "****** ROSE/projects/backstroke/reverseComputation: make check rule complete (terminated normally) ******"
	@echo "*********************************************************************************************************"

mytest:
	@$(MAKE) $(TEST_Objects)
