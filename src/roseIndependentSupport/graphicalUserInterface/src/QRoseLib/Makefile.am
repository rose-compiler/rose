include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_QROSE
INCLUDES = -I$(top_srcdir)/src/QRCodeBox 	-I$(top_srcdir)/src/QRGui -I$(top_srcdir)/src/QRTree 	-I$(top_srcdir)/src/QRQueryBox 	@ROSE_INCLUDES@ @QT_INCLUDES@

if DEV_LIBS
lib_LTLIBRARIES = libqrose.la
else
noinst_LTLIBRARIES = libqrose.la
endif
libqrose_la_LIBADD = $(top_builddir)/src/QRTree/libqrtree.la \
	$(top_builddir)/src/QRGui/libqrgui.la $(top_builddir)/src/QRCodeBox/libqrcodebox.la \
	$(top_builddir)/src/QRQueryBox/libqrquerybox.la
libqrose_la_SOURCES = qroselib.cpp

else # !ROSE_USE_QROSE

EXTRA_DIST = qroselib.cpp

endif

# eof
