#------------ WidgetCreator Makefile.am -----------
include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libWidgetCreator.la 



libWidgetCreator_la_SOURCES = AsmInstructionBarCreator.cpp\
                              AsmViewCreator.cpp\
                              AstBrowserWidgetCreator.cpp\
                              AstGraphWidgetCreator.cpp\
                              BeautifiedAstCreator.cpp\
                              CallGraphWidgetCreator.cpp\
                              MetricsKiviatCreator.cpp\
                              NodeInfoWidgetCreator.cpp\
                              QRQueryBoxCreator.cpp\
                              QRSourceBoxCreator.cpp\
                              QRTreeBoxCreator.cpp\
                              RoseCodeEditCreator.cpp\
                              RoseFileSelectorCreator.cpp\
                              SrcBinViewCreator.cpp\
                              SubWindowFactory.cpp\
                              WidgetCreatorInterface.cpp

nodist_libWidgetCreator_la_SOURCES = moc_AsmInstructionBarCreator.cxx\
                                     moc_AsmViewCreator.cxx\
                                     moc_AstBrowserWidgetCreator.cxx\
                                     moc_AstGraphWidgetCreator.cxx\
                                     moc_BeautifiedAstCreator.cxx\
                                     moc_CallGraphWidgetCreator.cxx\
                                     moc_MetricsKiviatCreator.cxx\
                                     moc_NodeInfoWidgetCreator.cxx\
                                     moc_QRQueryBoxCreator.cxx\
                                     moc_QRSourceBoxCreator.cxx\
                                     moc_QRTreeBoxCreator.cxx\
                                     moc_RoseCodeEditCreator.cxx\
                                     moc_RoseFileSelectorCreator.cxx\
                                     moc_SrcBinViewCreator.cxx\
                                     moc_SubWindowFactory.cxx



# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsHeaderInstallPath)/
headerTarget_HEADERS = AsmInstructionBarCreator.h\
                       AsmViewCreator.h\
                       AstBrowserWidgetCreator.h\
                       AstGraphWidgetCreator.h\
                       BeautifiedAstCreator.h\
                       CallGraphWidgetCreator.h\
                       MetricsKiviatCreator.h\
                       NodeInfoWidgetCreator.h\
                       QRQueryBoxCreator.h\
                       QRSourceBoxCreator.h\
                       QRTreeBoxCreator.h\
                       RoseCodeEditCreator.h\
                       RoseFileSelectorCreator.h\
                       SrcBinViewCreator.h\
                       SubWindowFactory.h\
                       WidgetCreatorInterface.h


endif

EXTRA_DIST = 

