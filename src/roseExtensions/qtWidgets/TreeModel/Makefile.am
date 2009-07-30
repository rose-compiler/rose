#------------ TreeModel Makefile.am -----------
include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libTreeModel.la 



libTreeModel_la_SOURCES = ItemTreeModel.cpp\
                          ItemTreeNode.cpp

nodist_libTreeModel_la_SOURCES = moc_ItemTreeModel.cxx



# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsHeaderInstallPath)/
headerTarget_HEADERS = ItemTreeModel.h\
                       ItemTreeNode.h


endif

EXTRA_DIST = 

