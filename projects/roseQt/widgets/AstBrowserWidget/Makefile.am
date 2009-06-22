#------------ AstBrowserWidget Makefile.am -----------
include $(top_srcdir)/projects/roseQt/Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libAstBrowserWidget.la 



libAstBrowserWidget_la_SOURCES = AstBrowserWidget.cpp\
                                 QtAstModel.cpp

nodist_libAstBrowserWidget_la_SOURCES = moc_AstBrowserWidget.cxx\
                                        moc_QtAstModel.cxx



# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsIncPath)/
headerTarget_HEADERS = AstBrowserWidget.h\
                       QtAstModel.h


endif
