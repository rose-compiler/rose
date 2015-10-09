include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc

EXTRA_DIST = MainWindow.ui \
			 res.qrc \
			 icons/allocation.gif \
			 icons/application-exit.png \
			 icons/array.gif \
			 icons/arrow.png \
			 icons/basic_type.gif \
			 icons/document-open.png \
			 icons/document-properties.png \
			 icons/document-save-as.png \
			 icons/document-save.png \
			 icons/file.png \
			 icons/scope.gif \
			 icons/variable.gif

AM_CPPFLAGS += . -I$(srcdir)/.. $(ROSE_INCLUDES)

if ROSE_WITH_ROSEQT

noinst_LTLIBRARIES = libDebuggerQt.la

libDebuggerQt_la_SOURCES = MainWindow.cpp\
                           RtedDebug.cpp\
                           TypeInfoDisplay.cpp\
                           MemoryTypeDisplay.cpp\
                           VariablesTypeDisplay.cpp\
                           MemoryGraphicsView.cpp\
                           PointerDisplay.cpp\
						 MainWindow.h\
						 MemoryGraphicsView.h\
						 MemoryTypeDisplay.h\
						 ModelRoles.h\
						 PointerDisplay.h\
						 RtedDebug.h\
						 TypeInfoDisplay.h\
						 VariablesTypeDisplay.h

nodist_libDebuggerQt_la_SOURCES = moc_MainWindow.cxx\
                                  ui_MainWindow.h\
                                  qrc_res.cpp


BUILT_SOURCES = ui_MainWindow.h


libDebuggerQt_la_LIBADD = $(top_builddir)/src/roseExtensions/qtWidgets/TreeModel/libTreeModel.la\
                      $(top_builddir)/src/roseExtensions/qtWidgets/QCodeEditWidget/libQCodeEditWidget.la\
		      $(QT_LDFLAGS) 

endif
