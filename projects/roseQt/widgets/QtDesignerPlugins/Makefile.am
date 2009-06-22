#------------ QtDesignerPlugins Makefile.am -----------
include $(top_srcdir)/projects/roseQt/Make.inc 

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libQtDesignerPlugins.la 


# Contents of Makefile.am.pre
INCLUDES += -I$(srcdir)/../RoseCodeEdit/QCodeEdit -I$(srcdir)/../RoseCodeEdit/QCodeEdit/document

# End of Makefile.am.pre Content

libQtDesignerPlugins_la_SOURCES = AsmViewPlugin.cpp\
                                  AstBrowserWidgetPlugin.cpp\
                                  AstGraphWidgetPlugin.cpp\
                                  BAstViewPlugin.cpp\
                                  KiviatViewPlugin.cpp\
                                  MetricsKiviatPlugin.cpp\
                                  NodeInfoWidgetPlugin.cpp\
                                  ProjectViewPlugin.cpp\
                                  PropertyTreeWidgetPlugin.cpp\
                                  RoseCodeEditPlugin.cpp\
                                  RoseFileComboBoxPlugin.cpp\
                                  RoseWidgetsCollection.cpp

nodist_libQtDesignerPlugins_la_SOURCES = moc_AsmViewPlugin.cxx\
                                         moc_AstBrowserWidgetPlugin.cxx\
                                         moc_AstGraphWidgetPlugin.cxx\
                                         moc_BAstViewPlugin.cxx\
                                         moc_KiviatViewPlugin.cxx\
                                         moc_MetricsKiviatPlugin.cxx\
                                         moc_NodeInfoWidgetPlugin.cxx\
                                         moc_ProjectViewPlugin.cxx\
                                         moc_PropertyTreeWidgetPlugin.cxx\
                                         moc_RoseCodeEditPlugin.cxx\
                                         moc_RoseFileComboBoxPlugin.cxx\
                                         moc_RoseWidgetsCollection.cxx\
                                         qrc_QtDesignerPlugins.cpp



# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsIncPath)/
headerTarget_HEADERS = AsmViewPlugin.h\
                       AstBrowserWidgetPlugin.h\
                       AstGraphWidgetPlugin.h\
                       BAstViewPlugin.h\
                       KiviatViewPlugin.h\
                       MetricsKiviatPlugin.h\
                       NodeInfoWidgetPlugin.h\
                       ProjectViewPlugin.h\
                       PropertyTreeWidgetPlugin.h\
                       RoseCodeEditPlugin.h\
                       RoseFileComboBoxPlugin.h\
                       RoseWidgetsCollection.h


endif
