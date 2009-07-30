#------------ util Makefile.am -----------
include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libutil.la 



libutil_la_SOURCES = AsmToSourceMapper.cpp\
                     AstDisplayInfo.cpp\
                     AstFilters.cpp\
                     ItemModelHelper.cpp\
                     LCS.cpp\
                     MetricAttributeCollector.cpp\
                     MetricBgDelegate.cpp\
                     QRoseComponentProxy.cpp\
                     RoseGraphicsView.cpp\
                     RoseTreeView.cpp\
                     SageMimeData.cpp\
                     SgNodeUtil.cpp

nodist_libutil_la_SOURCES = moc_MetricAttributeCollector.cxx\
                            moc_MetricBgDelegate.cxx\
                            moc_QRoseComponentProxy.cxx\
                            moc_RoseGraphicsView.cxx\
                            moc_RoseTreeView.cxx\
                            qrc_util.cpp\
                            ui_MetricBgDelegateConfig.h

BUILT_SOURCES = ui_MetricBgDelegateConfig.h

# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsHeaderInstallPath)/
headerTarget_HEADERS = AsmToSourceMapper.h\
                       AstDisplayInfo.h\
                       AstFilters.h\
                       ItemModelHelper.h\
                       LCS.h\
                       MetricAttributeCollector.h\
                       MetricBgDelegate.h\
                       QRoseComponentProxy.h\
                       RoseGraphicsView.h\
                       RoseTreeView.h\
                       SageMimeData.h\
                       SgNodeUtil.h\
                       StreamRedirector.h


endif

EXTRA_DIST = MetricBgDelegateConfig.ui \
	util.qrc \
	AppIcons/info.png \
	AppIcons/document-properties.png \
	AppIcons/document-save.png \
	AppIcons/application-exit.png \
	AppIcons/document-save-as.png \
	AppIcons/document-open.png \
	NodeIcons/loop.png \
	NodeIcons/sourcefiles.gif \
	NodeIcons/sourcefile.gif \
	NodeIcons/class.gif \
	NodeIcons/namespace.gif \
	NodeIcons/project.gif \
	NodeIcons/binaryfiles.gif \
	NodeIcons/binaryfile.gif \
	NodeIcons/function.gif \
	NodeIcons/includes.gif \
	NodeIcons/include.gif \
	WidgetIcons/Kiviat.gif \
	WidgetIcons/PropTreeWidget.gif \
	WidgetIcons/Rose.gif \
	WidgetIcons/CallGraph.gif \
	WidgetIcons/Graph.gif

