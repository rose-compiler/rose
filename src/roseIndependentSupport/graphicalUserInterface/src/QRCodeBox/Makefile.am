include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_QROSE
INCLUDES = -I$(top_srcdir)/src/roseIndependentSupport/graphicalUserInterface/src/QRGui \
   -I$(topc_srcdir)/src/roseIndependentSupport/graphicalUserInterface/src/QRCodeBox @QT_INCLUDES@ $(ROSE_INCLUDES)

# AM_LDFLAGS = @LIB_ROSE@ @ROSE_LDFLAGS@  @LIB_QT@ @QT_LDFLAGS@

include_HEADERS = QRCodeBox.h qeditor.h qsourcecolorizer.h cindent.h cpp_colorizer.h \
	paragdata.h parenmatcher.h qeditor_indenter.h simple_indent.h dlldefs.h \
	QRUnparser.h levelwidget.h linenumberwidget.h markerwidget.h

# noinst_LTLIBRARIES = libqrcodebox.la
noinst_LTLIBRARIES = libqrcodebox.la

libqrcodebox_la_SOURCES = cindent.cpp cpp_colorizer.cpp levelwidget.cpp \
	linenumberwidget.cpp markerwidget.cpp paragdata.cpp parenmatcher.cpp qeditor.cpp \
	qeditor_indenter.cpp QRCodeBox.cpp qsourcecolorizer.cpp simple_indent.cpp yyindent.cpp \
	QRUnparser.cpp

EXTRA_DIST = levelwidget.moc linenumberwidget.moc markerwidget.moc \
	     qeditor.moc QRCodeBox.moc

else # !ROSE_USE_QROSE

EXTRA_DIST = QRCodeBox.h qeditor.h qsourcecolorizer.h cindent.h \
	     cpp_colorizer.h paragdata.h parenmatcher.h \
	     qeditor_indenter.h simple_indent.h dlldefs.h \
	     QRUnparser.h levelwidget.h linenumberwidget.h markerwidget.h \
	     \
	     cindent.cpp cpp_colorizer.cpp levelwidget.cpp \
	     linenumberwidget.cpp markerwidget.cpp paragdata.cpp \
	     parenmatcher.cpp qeditor.cpp qeditor_indenter.cpp \
	     QRCodeBox.cpp qsourcecolorizer.cpp simple_indent.cpp \
	     yyindent.cpp \
	     \
	     levelwidget.moc linenumberwidget.moc markerwidget.moc \
	     qeditor.moc QRCodeBox.moc
endif

# eof
