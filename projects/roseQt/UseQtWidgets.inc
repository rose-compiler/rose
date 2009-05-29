include $(top_srcdir)/projects/roseQt/QtRules.inc

intern_widgetsDir = $(top_srcdir)/projects/roseQt/widgets
intern_codeEdit = $(top_srcdir)/projects/roseQt/widgets/RoseCodeEdit/QCodeEdit

ROSEQT_INCLUDES =   -I$(intern_widgetsDir)\
                    -I$(intern_widgetsDir)/AsmInstructionBar\
                    -I$(intern_widgetsDir)/AsmSourceMapper\
                    -I$(intern_widgetsDir)/AstBrowserWidget\
                    -I$(intern_widgetsDir)/AstGraphWidget\
                    -I$(intern_widgetsDir)/BeautifiedAst\
                    -I$(intern_widgetsDir)/NodeInfoWidget\
                    -I$(intern_widgetsDir)/PropertyTreeWidget\
                    -I$(intern_widgetsDir)/RoseFileSelector\
                    -I$(intern_widgetsDir)/MetricsKiviat\
                    -I$(intern_widgetsDir)/KiviatView\
                    -I$(intern_widgetsDir)/RoseCodeEdit\
                    -I$(intern_widgetsDir)/InstructionCountAnnotator\
                    -I$(intern_widgetsDir)/util\
                    -I$(intern_codeEdit)\
                    -I$(intern_codeEdit)/widgets\
                    -I$(intern_codeEdit)/document\
                    -I$(intern_codeEdit)/qnfa

