include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/projects/roseQt/UseQtWidgets.inc


if ROSE_WITH_ROSEQT

#noinst_LTLIBRARIES = libRtedQt.la

#libRtedQt_la_SOURCES = CustomListModels.cpp\
#                       DebugDialog.cpp\
#                       rted_qt.cpp 

#nodist_libRtedQt_la_SOURCES = moc_DebugDialog.cxx\
#                              ui_DebugDialog.h\
#                              qrc_res.cpp
BUILT_SOURCES = ui_DebugDialog.h


INCLUDES = $(ROSE_INCLUDES) $(ROSEQT_INCLUDES)
#libRtedQt_la_LIBADD = $(top_builddir)/projects/roseQt/widgets/libRoseQtWidgets.la



bin_PROGRAMS = rtedQt

rtedQt_SOURCES = CustomListModels.cpp\
                 DebugDialog.cpp\
                 rted_qt.cpp 

nodist_rtedQt_SOURCES = moc_DebugDialog.cxx\
                        ui_DebugDialog.h\
                        qrc_res.cpp

rtedQt_LDADD = $(top_builddir)/projects/roseQt/widgets/libRoseQtWidgets.la $(ROSE_LIBS) $(QT_LDFLAGS)                              
                              
endif
