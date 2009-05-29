#------------ QtDesignerPlugins Makefile.am -----------
include $(top_srcdir)/projects/roseQt/Make.inc 

if ROSE_USE_QT

noinst_LTLIBRARIES = libQtDesignerPlugins.la 


# Contents of Makefile.am.pre
INCLUDES += -I$(srcdir)/../RoseCodeEdit/QCodeEdit -I$(srcdir)/../RoseCodeEdit/QCodeEdit/document

# End of Makefile.am.pre Content

libQtDesignerPlugins_la_SOURCES = AstBrowserWidgetPlugin.cpp\
                                  AstGraphWidgetPlugin.cpp\
                                  BAstViewPlugin.cpp\
                                  KiviatViewPlugin.cpp\
                                  MetricsKiviatPlugin.cpp\
                                  NodeInfoWidgetPlugin.cpp\
                                  PropertyTreeWidgetPlugin.cpp\
                                  RoseCodeEditPlugin.cpp\
                                  RoseFileComboBoxPlugin.cpp\
                                  RoseWidgetsCollection.cpp

nodist_libQtDesignerPlugins_la_SOURCES = moc_AstBrowserWidgetPlugin.cpp\
                                         moc_AstGraphWidgetPlugin.cpp\
                                         moc_BAstViewPlugin.cpp\
                                         moc_KiviatViewPlugin.cpp\
                                         moc_MetricsKiviatPlugin.cpp\
                                         moc_NodeInfoWidgetPlugin.cpp\
                                         moc_PropertyTreeWidgetPlugin.cpp\
                                         moc_RoseCodeEditPlugin.cpp\
                                         moc_RoseFileComboBoxPlugin.cpp\
                                         moc_RoseWidgetsCollection.cpp\
                                         qrc_QtDesignerPlugins.cpp



endif
