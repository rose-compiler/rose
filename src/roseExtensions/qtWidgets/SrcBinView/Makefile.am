#------------ AsmView Makefile.am -----------
include ../Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libSrcBinView.la 

libSrcBinView_la_SOURCES = SrcBinView.cpp

nodist_libSrcBinView_la_SOURCES = moc_SrcBinView.cxx \
                                  ui_SrcBinView.h

BUILT_SOURCES = ui_SrcBinView.h

EXTRA_DIST = SrcBinView.ui

# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsHeaderInstallPath)/
headerTarget_HEADERS = SrcBinView.h

endif
