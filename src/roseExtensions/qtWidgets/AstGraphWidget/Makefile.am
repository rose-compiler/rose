#------------ AstGraphWidget Makefile.am -----------
include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc 

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
headerTargetdir    =  $(roseQtWidgetsHeaderInstallPath)/
headerTarget_HEADERS = AstGraphWidget.h\
                       DisplayEdge.h\
                       DisplayGraphNode.h\
                       DisplayNode.h\
                       TreeLayoutGenerator.h


endif
