#------------ AstProcessing Makefile.am -----------
include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libAstProcessing.la 



libAstProcessing_la_SOURCES = NewAstProcessing.cpp





EXTRA_DIST = 

# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsHeaderInstallPath)/
headerTarget_HEADERS = AstProcessing_BottomUp_impl.h\
                       AstProcessing_TopDown_impl.h\
                       AstProcessing_TopDownBottomUp_impl.h\
                       NewAstProcessing.h


endif
