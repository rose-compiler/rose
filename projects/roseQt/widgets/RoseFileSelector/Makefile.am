#------------ RoseFileSelector Makefile.am -----------
include $(top_srcdir)/projects/roseQt/Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libRoseFileSelector.la 



libRoseFileSelector_la_SOURCES = RoseFileComboBox.cpp

nodist_libRoseFileSelector_la_SOURCES = moc_RoseFileComboBox.cxx



# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsIncPath)/
headerTarget_HEADERS = RoseFileComboBox.h


endif
