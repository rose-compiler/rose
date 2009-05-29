#------------ BeautifiedAst Makefile.am -----------
include $(top_srcdir)/projects/roseQt/Make.inc 

if ROSE_USE_QT

noinst_LTLIBRARIES = libBeautifiedAst.la 



libBeautifiedAst_la_SOURCES = BAstModel.cpp\
                              BAstNode.cpp\
                              BAstView.cpp

nodist_libBeautifiedAst_la_SOURCES = moc_BAstModel.cpp\
                                     moc_BAstView.cpp\
                                     qrc_BeautifiedAst.cpp



endif
