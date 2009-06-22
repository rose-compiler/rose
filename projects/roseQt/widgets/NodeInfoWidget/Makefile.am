#------------ NodeInfoWidget Makefile.am -----------
include $(top_srcdir)/projects/roseQt/Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libNodeInfoWidget.la 



libNodeInfoWidget_la_SOURCES = NodeInfoWidget.cpp

nodist_libNodeInfoWidget_la_SOURCES = moc_NodeInfoWidget.cxx



# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsIncPath)/
headerTarget_HEADERS = NodeInfoWidget.h


endif
