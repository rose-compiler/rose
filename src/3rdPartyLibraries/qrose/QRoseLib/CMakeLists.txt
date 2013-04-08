
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(qrose_LIB_SRCS qroselib.C)

add_library(qrose SHARED ${qrose_LIB_SRCS})

target_link_libraries(qrose
  qrsourcebox qrquerybox qrtreebox qrwidgets qrframework qrcommon
  ${QT_LIBRARIES} ${QT_QT3SUPPORT_LIBRARY})

set_target_properties(qrose PROPERTIES VERSION 1.0.0 SOVERSION 1)
install(TARGETS qrose ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#if ROSE_USE_QT
#
#lib_LTLIBRARIES = libqrose.la
#libqrose_la_LIBADD = ../Framework/libqrframework.la \
#                     ../Widgets/libqrwidgets.la \
#                     ../Components/Common/libqrcommon.la \
#                     ../Components/QueryBox/libqrquerybox.la \
#                     ../Components/SourceBox/libqrsourcebox.la \
#                     ../Components/TreeBox/libqrtreebox.la
#                     
#libqrose_la_SOURCES = qroselib.C
#if QROSE_USE_MINGW32
#   libqrose_la_LDFLAGS = -no-undefined @QT_LDFLAGS@
#else
#   libqrose_la_LDFLAGS = @QT_LDFLAGS@
#endif
#
#endif
