#------------ TextEditPainter Makefile.am -----------

#only included for the Qt compiler rules
include $(top_srcdir)/projects/roseQt/UseQtWidgets.inc

if ROSE_USE_QT


INCLUDES = $(QT_INCLUDES)
                         

bin_PROGRAMS = textEditPainterDemo

textEditPainterDemo_SOURCES        = mainwidget.cpp      customtextedit.cpp     main.cpp
nodist_textEditPainterDemo_SOURCES = moc_mainwidget.cpp  moc_customtextedit.cpp ui_DemoWidget.h

BUILT_SOURCES = ui_DemoWidget.h

textEditPainterDemo_LDADD = $(QT_LDFLAGS)
                   
endif
