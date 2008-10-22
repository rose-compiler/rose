include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

if ROSE_USE_QT

lib_LTLIBRARIES = libqrose.la
libqrose_la_LIBADD = ../Framework/libqrframework.la ../Widgets/libqrwidgets.la
libqrose_la_SOURCES = qroselib.cpp
if QROSE_USE_MINGW32
   libqrose_la_LDFLAGS = -no-undefined @LIB_ROSE@ @ROSE_LDFLAGS@  @LIB_QT@ @QT_LDFLAGS@
else
   libqrose_la_LDFLAGS = @LIB_ROSE@ @ROSE_LDFLAGS@  @LIB_QT@ @QT_LDFLAGS@
endif

endif
