include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc

if ROSE_WITH_ROSEQT

INCLUDES += . -I$(srcdir)/.. $(ROSE_INCLUDES)

noinst_LTLIBRARIES = libDebuggerQt.la

libDebuggerQt_la_SOURCES = MainWindow.cpp\
                           RtedDebug.cpp\
                           TypeInfoDisplay.cpp\
                           MemoryTypeDisplay.cpp\
                           VariablesTypeDisplay.cpp


nodist_libDebuggerQt_la_SOURCES = moc_MainWindow.cxx\
                                  ui_MainWindow.h\
                                  qrc_res.cpp
BUILT_SOURCES = ui_MainWindow.h


#libRtedQt_la_LIBADD = $(top_builddir)/src/roseExtensions/qtWidgets/libRoseQtWidgets.la
libDebuggerQt_la_LIBADD = $(ROSE_LIBS)



#bin_PROGRAMS = rtedQt

#rtedQt_SOURCES = CustomListModels.cpp\
#                 DebugDialog.cpp\
#                 rted_qt.cpp 

#nodist_rtedQt_SOURCES = moc_DebugDialog.cxx\
#                        ui_DebugDialog.h\
#                        qrc_res.cpp

#rtedQt_LDADD = $(ROSE_LIBS) $(QT_LDFLAGS)                              
                              
endif
