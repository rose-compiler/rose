include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS = icons22

if ROSE_USE_QROSE
INCLUDES = -I$(top_srcdir)/src/roseIndependentSupport/graphicalUserInterface/src/QRGui @QT_INCLUDES@ $(ROSE_INCLUDES)

# AM_LDFLAGS = @LIB_ROSE@ @ROSE_LDFLAGS@  @LIB_QT@ @QT_LDFLAGS@

include_HEADERS = QRGui.h qrose.h QRLayout.h QRAttribute.h QRPixmap.h \
	QRCoords.h QRGroup.h QRoseGroup.h QRMacros.h QRAstInfo.h \
	QRToolBar.h QRCallback.h
noinst_LTLIBRARIES = libqrgui.la
libqrgui_la_SOURCES = QRAstInfo.cpp QRGui.cpp QRLayout.cpp QRAttribute.cpp \
	QRPixmap.cpp QRToolBar.cpp QRCoords.cpp QRGroup.cpp QRoseGroup.cpp \
	QRCallback.cpp

EXTRA_DIST = QRCallback.moc QRGroup.moc QRLayout.moc QRToolBar.moc

else # !ROSE_USE_QROSE

EXTRA_DIST = QRGui.h qrose.h QRLayout.h QRAttribute.h QRPixmap.h \
	QRCoords.h QRGroup.h QRoseGroup.h QRMacros.h QRAstInfo.h \
	QRToolBar.h QRCallback.h \
	\
	QRAstInfo.cpp QRGui.cpp QRLayout.cpp QRAttribute.cpp \
	QRPixmap.cpp QRToolBar.cpp QRCoords.cpp QRGroup.cpp \
	QRoseGroup.cpp QRCallback.cpp \
	\
	QRCallback.moc QRGroup.moc QRLayout.moc QRToolBar.moc

endif

# eof
