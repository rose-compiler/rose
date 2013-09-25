set(COMMON_MOC_SOURCES
  QRAstInfo.h QRCoords.h  QRIcons.h QRPixmap.h QRAttribute.h
  QRoseComponent.h)
QT4_WRAP_CPP(COMMON_MOC_CPP ${COMMON_MOC_SOURCES})

set(COMMON_SOURCES
  QRAstInfo.C QRCoords.C  QRIcons.C  QRPixmap.C QRAttribute.C
  QRoseComponent.C)

add_library(qrcommon ${COMMON_SOURCES} ${COMMON_MOC_CPP})
add_dependencies(qrcommon rosetta_generated)

add_subdirectory(icons)


########### next target ###############


########### install files ###############

install(FILES  QRAstInfo.h QRCoords.h  QRIcons.h QRPixmap.h QRAttribute.h  QRoseComponent.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#SUBDIRS=icons
#
#if ROSE_USE_QT
#
#INCLUDES = $(ROSE_INCLUDES) $(QT_INCLUDES) 
#
#METASOURCES = auto
#include_HEADERS = QRAstInfo.h QRCoords.h  QRIcons.h QRPixmap.h QRAttribute.h  QRoseComponent.h
#
#noinst_LTLIBRARIES = libqrcommon.la
#
## DQ (2/11/2009): Using nodist_ rule to support specification of files to be built at compile time and not distrivuted in the distribution.
#libqrcommon_la_SOURCES = QRAstInfo.C QRCoords.C  QRIcons.C  QRPixmap.C QRAttribute.C  QRoseComponent.C
# 
#CLEANFILES = 
#
#endif
#
#EXTRA_DIST = QRAstInfo.h QRCoords.h  QRIcons.h QRPixmap.h QRAttribute.h  QRoseComponent.h
