# Rose Widgets Makefile.am
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

EXTRA_DIST = CMakeLists.txt doc \
							Make.inc

SUBDIRS = AsmInstructionBar\
          AsmView\
          AstBrowserWidget\
          AstGraphWidget\
          AstProcessing\
          BeautifiedAst\
          FlopCounter\
          InstructionCountAnnotator\
          KiviatView\
          MetricFilter\
          MetricsConfig\
          MetricsKiviat\
          NodeInfoWidget\
          ProjectManager\
          PropertyTreeWidget\
	  QtGradientEditor\
          QCodeEditWidget\
          RoseCodeEdit\
          RoseFileSelector\
	  SrcBinView \
          TaskSystem\
          TreeModel\
          util \
	  WidgetCreator
                    

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libRoseQtWidgets.la

AM_CPPFLAGS = $(ROSE_INCLUDES)

buildDir = $(top_builddir)/src/roseExtensions/qtWidgets

libRoseQtWidgets_la_SOURCES  =  DummyForAutotools.cpp

libRoseQtWidgets_la_LIBADD   =  $(buildDir)/AsmInstructionBar/libAsmInstructionBar.la \
                                $(buildDir)/AsmView/libAsmView.la \
                                $(buildDir)/AstBrowserWidget/libAstBrowserWidget.la \
                                $(buildDir)/AstGraphWidget/libAstGraphWidget.la \
                                $(buildDir)/AstProcessing/libAstProcessing.la \
                                $(buildDir)/BeautifiedAst/libBeautifiedAst.la \
                                $(buildDir)/FlopCounter/libFlopCounter.la \
                                $(buildDir)/InstructionCountAnnotator/libInstructionCountAnnotator.la\
                                $(buildDir)/KiviatView/libKiviatView.la \
                                $(buildDir)/MetricFilter/libMetricFilter.la \
                                $(buildDir)/MetricsConfig/libMetricsConfig.la \
                                $(buildDir)/MetricsKiviat/libMetricsKiviat.la \
                                $(buildDir)/NodeInfoWidget/libNodeInfoWidget.la \
                                $(buildDir)/ProjectManager/libProjectManager.la \
                                $(buildDir)/QCodeEditWidget/libQCodeEditWidget.la\
				$(buildDir)/QtGradientEditor/libQtGradientEditor.la \
                                $(buildDir)/PropertyTreeWidget/libPropertyTreeWidget.la \
                                $(buildDir)/RoseCodeEdit/libRoseCodeEdit.la \
                                $(buildDir)/RoseFileSelector/libRoseFileSelector.la \
                                $(buildDir)/SrcBinView/libSrcBinView.la \
                                $(buildDir)/TaskSystem/libTaskSystem.la \
			        $(buildDir)/TreeModel/libTreeModel.la \
                                $(buildDir)/util/libutil.la \
				$(buildDir)/WidgetCreator/libWidgetCreator.la
#docs: 
#	rm -rf doc
#	mkdir doc 
#	cp  $(top_srcdir)/src/roseExtensions/qtWidgets/doc/* doc/
#	doxygen $(top_srcdir)/src/roseExtensions/qtWidgets/doc/doc.doxyfile


endif
