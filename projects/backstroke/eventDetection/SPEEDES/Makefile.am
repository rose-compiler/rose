include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = SPEEDES_event_detector

# simpleComputationReverser_SOURCES = simpleComputationReverser.C
SPEEDES_event_detector_SOURCES = SPEEDES_event_detector.cpp

AM_CPPFLAGS = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST = speedes_test_code.cpp

CLEANFILES = 

include_HEADERS =

check-local:
	@echo "*************************************************************************************************************"
	@echo "****** ROSE/projects/backstroke/eventDetection/SPEEDES: make check rule complete (terminated normally) ******"
	@echo "*************************************************************************************************************"
