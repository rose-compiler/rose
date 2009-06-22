#------------ AstProcessing Makefile.am -----------
include $(top_srcdir)/projects/roseQt/Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libAstProcessing.la 



libAstProcessing_la_SOURCES = NewAstProcessing.cpp





# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsIncPath)/
headerTarget_HEADERS = AstProcessing_BottomUp_impl.h\
                       AstProcessing_TopDown_impl.h\
                       AstProcessing_TopDownBottomUp_impl.h\
                       NewAstProcessing.h


endif
