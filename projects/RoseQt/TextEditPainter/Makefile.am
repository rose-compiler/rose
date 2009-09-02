#------------ TextEditPainter Makefile.am -----------

#only included for the Qt compiler rules
include $(top_srcdir)/projects/roseQt/UseQtWidgets.inc

if ROSE_WITH_ROSEQT


INCLUDES = $(QT_INCLUDES)
                         

bin_PROGRAMS = textEditPainterDemo

textEditPainterDemo_SOURCES        = mainwidget.cpp      customtextedit.cpp     main.cpp
nodist_textEditPainterDemo_SOURCES = moc_mainwidget.cxx  moc_customtextedit.cxx ui_DemoWidget.h

BUILT_SOURCES = ui_DemoWidget.h

EXTRA_DIST = DemoWidget.ui

textEditPainterDemo_LDADD = $(QT_LDFLAGS)
                   
endif
