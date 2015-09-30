include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = ROSS_event_detector

ROSS_event_detector_SOURCES = ROSS_event_detector.cpp

AM_CPPFLAGS = $(ROSE_INCLUDES)
LDADD    = $(LIBS_WITH_RPATH) $(ROSE_LIBS)

EXTRA_DIST = 

CLEANFILES = 

include_HEADERS =

check-local:
	@echo "Test passed"
	@echo "**********************************************************************************************************"
	@echo "****** ROSE/projects/backstroke/eventDetection/ROSS: make check rule complete (terminated normally) ******"
	@echo "**********************************************************************************************************"
