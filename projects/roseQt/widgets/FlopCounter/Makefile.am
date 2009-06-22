#------------ FlopCounter Makefile.am -----------
include $(top_srcdir)/projects/roseQt/Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libFlopCounter.la 



libFlopCounter_la_SOURCES = FlopCounter.cpp\
                            Flops.cpp





# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsIncPath)/
headerTarget_HEADERS = FlopCounter.h\
                       Flops.h


endif
