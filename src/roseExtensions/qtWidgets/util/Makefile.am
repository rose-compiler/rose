#------------ util Makefile.am -----------
include ../Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libutil.la 



libutil_la_SOURCES = AsmToSourceMapper.cpp\
                     AstDisplayInfo.cpp\
                     AstFilters.cpp\
                     ItemModelHelper.cpp\
                     ItemTreeModel.cpp\
                     ItemTreeNode.cpp\
                     LCS.cpp\
                     MetricAttributeCollector.cpp\
                     MetricBgDelegate.cpp\
                     RoseTreeView.cpp\
                     SageMimeData.cpp\
                     SgNodeUtil.cpp

nodist_libutil_la_SOURCES = moc_ItemTreeModel.cxx\
                            moc_MetricAttributeCollector.cxx\
                            moc_MetricBgDelegate.cxx\
                            moc_RoseTreeView.cxx\
                            qrc_util.cpp\
                            ui_MetricBgDelegateConfig.h

BUILT_SOURCES = ui_MetricBgDelegateConfig.h

# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsIncPath)/
headerTarget_HEADERS = AsmToSourceMapper.h\
                       AstDisplayInfo.h\
                       AstFilters.h\
                       ItemModelHelper.h\
                       ItemTreeModel.h\
                       ItemTreeNode.h\
                       LCS.h\
                       MetricAttributeCollector.h\
                       MetricBgDelegate.h\
                       RoseTreeView.h\
                       SageMimeData.h\
                       SgNodeUtil.h\
                       StreamRedirector.h


endif
