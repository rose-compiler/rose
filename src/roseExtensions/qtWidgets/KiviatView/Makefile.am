#------------ KiviatView Makefile.am -----------
include ../Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libKiviatView.la 



libKiviatView_la_SOURCES = KiviatView.cpp

nodist_libKiviatView_la_SOURCES = moc_KiviatView.cxx



# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsIncPath)/
headerTarget_HEADERS = KiviatView.h


endif
