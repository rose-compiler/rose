# Rose Widgets Makefile.am


INCLUDES = .

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
          QtDesignerPlugins\
          RoseCodeEdit\
          RoseFileSelector\
          TaskSystem\
          util
                    

if ROSE_WITH_ROSEQT

RoseQtWidgetslibdir = $(libdir)
RoseQtWidgetslib_LTLIBRARIES = libRoseQtWidgets.la


buildDir = $(top_builddir)/projects/roseQt/widgets


libRoseQtWidgets_la_SOURCES  =  DummyForAutotools.cpp 
  


libRoseQtWidgets_la_LIBADD   =  $(top_builddir)/src/librose.la\
                                $(buildDir)/AsmInstructionBar/libAsmInstructionBar.la \
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
                                $(buildDir)/PropertyTreeWidget/libPropertyTreeWidget.la \
                                $(buildDir)/QtDesignerPlugins/libQtDesignerPlugins.la \
                                $(buildDir)/RoseCodeEdit/libRoseCodeEdit.la \
                                $(buildDir)/RoseFileSelector/libRoseFileSelector.la \
                                $(buildDir)/TaskSystem/libTaskSystem.la \
                                $(buildDir)/util/libutil.la
                         

endif
