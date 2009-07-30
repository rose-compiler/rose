#------------ InstructionCountAnnotator Makefile.am -----------
include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libInstructionCountAnnotator.la 



libInstructionCountAnnotator_la_SOURCES = InstructionCountAnnotator.cpp\
                                          PTracer.cpp





# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsHeaderInstallPath)/
headerTarget_HEADERS = InstructionCountAnnotator.h\
                       PTracer.h


endif

EXTRA_DIST = 

