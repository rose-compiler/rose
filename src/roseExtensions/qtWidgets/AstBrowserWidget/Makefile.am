#------------ AstBrowserWidget Makefile.am -----------
include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libAstBrowserWidget.la 



libAstBrowserWidget_la_SOURCES = AstBrowserWidget.cpp\
                                 QtAstModel.cpp

nodist_libAstBrowserWidget_la_SOURCES = moc_AstBrowserWidget.cxx\
                                        moc_QtAstModel.cxx



# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsHeaderInstallPath)/
headerTarget_HEADERS = AstBrowserWidget.h\
                       QtAstModel.h


endif
