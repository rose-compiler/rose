include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc

if ROSE_WITH_ROSEQT

INCLUDES += . -I$(srcdir)/.. $(ROSE_INCLUDES)

noinst_LTLIBRARIES = libDebuggerQt.la

libDebuggerQt_la_SOURCES = MainWindow.cpp\
                           RtedDebug.cpp\
                           TypeInfoDisplay.cpp\
                           MemoryTypeDisplay.cpp\
                           VariablesTypeDisplay.cpp\
                           MemoryGraphicsView.cpp\
                           PointerDisplay.cpp


nodist_libDebuggerQt_la_SOURCES = moc_MainWindow.cxx\
                                  ui_MainWindow.h\
                                  qrc_res.cpp
BUILT_SOURCES = ui_MainWindow.h


libDebuggerQt_la_LIBADD = $(top_builddir)/src/roseExtensions/qtWidgets/TreeModel/libTreeModel.la\
                      $(top_builddir)/src/roseExtensions/qtWidgets/QCodeEditWidget/libQCodeEditWidget.la\
		      $(QT_LDFLAGS) 
		       

endif
