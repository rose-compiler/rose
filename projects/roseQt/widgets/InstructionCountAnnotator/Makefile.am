#------------ InstructionCountAnnotator Makefile.am -----------
include $(top_srcdir)/projects/roseQt/Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libInstructionCountAnnotator.la 



libInstructionCountAnnotator_la_SOURCES = InstructionCountAnnotator.cpp\
                                          PTracer.cpp





# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsIncPath)/
headerTarget_HEADERS = InstructionCountAnnotator.h\
                       PTracer.h


endif
