#------------ AsmView Makefile.am -----------
include $(top_srcdir)/projects/roseQt/Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libAsmView.la 



libAsmView_la_SOURCES = AsmTreeNode.cpp\
                        AsmView.cpp

nodist_libAsmView_la_SOURCES = moc_AsmView.cxx\
                               ui_ConfigureAsmViewDelegate.h

BUILT_SOURCES = ui_ConfigureAsmViewDelegate.h

# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsIncPath)/
headerTarget_HEADERS = AsmTreeNode.h\
                       AsmView.h


endif
