#------------ BeautifiedAst Makefile.am -----------
include ../Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libBeautifiedAst.la 



libBeautifiedAst_la_SOURCES = BAstModel.cpp\
                              BAstNode.cpp\
                              BAstView.cpp

nodist_libBeautifiedAst_la_SOURCES = moc_BAstModel.cxx\
                                     moc_BAstView.cxx\
                                     qrc_BeautifiedAst.cpp



# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsIncPath)/
headerTarget_HEADERS = BAstModel.h\
                       BAstNode.h\
                       BAstView.h


endif
