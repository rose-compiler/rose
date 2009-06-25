#------------ AstGraphWidget Makefile.am -----------
include $(top_srcdir)/projects/roseQt/Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libAstGraphWidget.la 



libAstGraphWidget_la_SOURCES = AstGraphWidget.cpp\
                               DisplayEdge.cpp\
                               DisplayGraphNode.cpp\
                               DisplayNode.cpp\
                               TreeLayoutGenerator.cpp\
                               treeTest.cpp

nodist_libAstGraphWidget_la_SOURCES = moc_AstGraphWidget.cxx\
                                      moc_DisplayGraphNode.cxx\
                                      ui_LayoutControl.h

BUILT_SOURCES = ui_LayoutControl.h

# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsIncPath)/
headerTarget_HEADERS = AstGraphWidget.h\
                       DisplayEdge.h\
                       DisplayGraphNode.h\
                       DisplayNode.h\
                       TreeLayoutGenerator.h


endif
