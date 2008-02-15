include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_QROSE
INCLUDES = -I$(top_srcdir)/src/roseIndependentSupport/graphicalUserInterface/src/QRGui @QT_INCLUDES@ $(ROSE_INCLUDES)

# AM_LDFLAGS = @LIB_ROSE@ @ROSE_LDFLAGS@  @LIB_QT@ @QT_LDFLAGS@

include_HEADERS = QRTreeBox.h

noinst_LTLIBRARIES = libqrtree.la

EXTRA_DIST = QRTreeBox.moc QRTree.moc

%.moc: $(srcdir)/%.h 
	$(MOC) -o $@ $<

libqrtree_la_SOURCES = QRTreeBox.cpp

else # !ROSE_USE_QROSE

EXTRA_DIST = QRTreeBox.h QRTreeBox.cpp \
	QRTreeBox.moc QRTree.moc

endif

# eof
