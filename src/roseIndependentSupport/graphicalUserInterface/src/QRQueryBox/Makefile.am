include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_QROSE
INCLUDES = -I$(top_srcdir)/src/roseIndependentSupport/graphicalUserInterface/src/QRGui @QT_INCLUDES@ $(ROSE_INCLUDES)

include_HEADERS = QRQueryDomain.h QRQueryOp.h QRQueryRange.h \
	QRQueryCollection.h QRQueryBox.h

noinst_LTLIBRARIES = libqrquerybox.la
libqrquerybox_la_SOURCES = QRQueryBox.cpp QRQueryCollection.cpp \
	QRQueryDomain.cpp QRQueryOp.cpp QRQueryRange.cpp

EXTRA_DIST = querybox.ui querybox.uic querybox.uih

else # !ROSE_USE_QROSE

EXTRA_DIST = QRQueryDomain.h QRQueryOp.h QRQueryRange.h \
	QRQueryCollection.h QRQueryBox.h \
	\
	QRQueryBox.cpp QRQueryCollection.cpp \
	QRQueryDomain.cpp QRQueryOp.cpp QRQueryRange.cpp \
	\
	querybox.ui querybox.uic querybox.uih

endif

# eof
