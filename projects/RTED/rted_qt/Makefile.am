include $(top_srcdir)/src/roseExtensions/qtWidgets/Make.inc

if ROSE_WITH_ROSEQT

INCLUDES += -I$(srcdir)/.. $(ROSE_INCLUDES)

noinst_LTLIBRARIES = libRtedQt.la

libRtedQt_la_SOURCES = CustomListModels.cpp\
                       DebugDialog.cpp\
                       RtedDebug.cpp\
                       rted_qt.cpp 

nodist_libRtedQt_la_SOURCES = moc_DebugDialog.cxx\
                              ui_DebugDialog.h\
                              qrc_res.cpp
BUILT_SOURCES = ui_DebugDialog.h


#libRtedQt_la_LIBADD = $(top_builddir)/src/roseExtensions/qtWidgets/libRoseQtWidgets.la
libRtedQt_la_LIBADD = $(top_builddir)/src/librose.la



#bin_PROGRAMS = rtedQt

#rtedQt_SOURCES = CustomListModels.cpp\
#                 DebugDialog.cpp\
#                 rted_qt.cpp 

#nodist_rtedQt_SOURCES = moc_DebugDialog.cxx\
#                        ui_DebugDialog.h\
#                        qrc_res.cpp

#rtedQt_LDADD = $(ROSE_LIBS) $(QT_LDFLAGS)                              
                              
endif
