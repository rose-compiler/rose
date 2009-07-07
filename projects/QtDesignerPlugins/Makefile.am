#------------ QtDesignerPlugins Makefile.am -----------
include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc 

if ROSE_WITH_ROSEQT

lib_LTLIBRARIES = libQtDesignerPlugins.la 

INCLUDES += -I$(srcdir)/../RoseCodeEdit/QCodeEdit -I$(srcdir)/../RoseCodeEdit/QCodeEdit/document

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

libQtDesignerPlugins_la_LIBADD = $(top_builddir)/src/librose.la $(QT_LDFLAGS)

libQtDesignerPlugins_la_DEPENDENCIES = $(top_builddir)/src/roseExtensions/qtWidgets/libRoseQtWidgets.la





# Target for installing header 
headerTargetdir    =  $(roseQtWidgetsHeaderInstallPath)/
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
