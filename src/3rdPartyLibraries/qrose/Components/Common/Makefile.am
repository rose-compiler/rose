include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

SUBDIRS=icons

if ROSE_USE_QT

AM_CPPFLAGS = $(ROSE_INCLUDES) $(QT_INCLUDES) 

METASOURCES = auto
pkginclude_HEADERS = QRAstInfo.h QRCoords.h  QRIcons.h QRPixmap.h QRAttribute.h  QRoseComponent.h

noinst_LTLIBRARIES = libqrcommon.la

# DQ (2/11/2009): Using nodist_ rule to support specification of files to be built at compile time and not distrivuted in the distribution.
libqrcommon_la_SOURCES = QRAstInfo.C QRCoords.C  QRIcons.C  QRPixmap.C QRAttribute.C  QRoseComponent.C
 
CLEANFILES = 

endif

EXTRA_DIST = CMakeLists.txt QRAstInfo.h QRCoords.h  QRIcons.h QRPixmap.h QRAttribute.h  QRoseComponent.h
