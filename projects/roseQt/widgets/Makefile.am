# Rose Widgets Makefile.am


INCLUDES = .

SUBDIRS = AstBrowserWidget\
          AsmInstructionBar\
          AstGraphWidget\
          AstProcessing\
          BeautifiedAst\
          FlopCounter\
          KiviatView\
          MetricsKiviat\
          MetricsConfig\
          MetricFilter\
          NodeInfoWidget\
          PropertyTreeWidget\
          QtDesignerPlugins\
          RoseCodeEdit\
          RoseFileSelector\
          util\
          InstructionCountAnnotator           

if ROSE_USE_QT

RoseQtWidgetslibdir = $(libdir)
RoseQtWidgetslib_LTLIBRARIES = libRoseQtWidgets.la


buildDir = $(top_builddir)/projects/roseQt/widgets


libRoseQtWidgets_la_SOURCES  =  DummyForAutotools.cpp 
  


libRoseQtWidgets_la_LIBADD   =  $(top_builddir)/src/librose.la\
                                $(buildDir)/AstBrowserWidget/libAstBrowserWidget.la \
                                $(buildDir)/AsmInstructionBar/libAsmInstructionBar.la \
                                $(buildDir)/AstGraphWidget/libAstGraphWidget.la \
                                $(buildDir)/AstProcessing/libAstProcessing.la \
                                $(buildDir)/BeautifiedAst/libBeautifiedAst.la \
                                $(buildDir)/FlopCounter/libFlopCounter.la \
                                $(buildDir)/KiviatView/libKiviatView.la \
                                $(buildDir)/MetricsKiviat/libMetricsKiviat.la \
                                $(buildDir)/MetricsConfig/libMetricsConfig.la \
                                $(buildDir)/MetricFilter/libMetricFilter.la \
                                $(buildDir)/NodeInfoWidget/libNodeInfoWidget.la \
                                $(buildDir)/PropertyTreeWidget/libPropertyTreeWidget.la \
                                $(buildDir)/QtDesignerPlugins/libQtDesignerPlugins.la \
                                $(buildDir)/RoseCodeEdit/libRoseCodeEdit.la \
                                $(buildDir)/RoseFileSelector/libRoseFileSelector.la \
                                $(buildDir)/util/libutil.la \
                                $(buildDir)/InstructionCountAnnotator/libInstructionCountAnnotator.la
                               

endif
