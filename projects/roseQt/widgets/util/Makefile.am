#------------ util Makefile.am -----------
include $(top_srcdir)/projects/roseQt/Make.inc 

if ROSE_USE_QT

noinst_LTLIBRARIES = libutil.la 



libutil_la_SOURCES = AstDisplayInfo.cpp\
                     AstFilters.cpp\
                     ItemTreeModel.cpp\
                     ItemTreeNode.cpp\
                     MetricAttributeCollector.cpp

nodist_libutil_la_SOURCES = moc_ItemTreeModel.cpp\
                            moc_MetricAttributeCollector.cpp\
                            qrc_util.cpp



endif
