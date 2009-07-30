#------------ FlopCounter Makefile.am -----------
include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libFlopCounter.la 



libFlopCounter_la_SOURCES = FlopCounter.cpp\
                            Flops.cpp





# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsHeaderInstallPath)/
headerTarget_HEADERS = FlopCounter.h\
                       Flops.h


endif

EXTRA_DIST = 

