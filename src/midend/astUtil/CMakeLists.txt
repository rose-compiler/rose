
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(astInterface)
add_subdirectory(astSupport)
add_subdirectory(symbolicVal)
add_subdirectory(annotation)


########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#SUBDIRS = astInterface astSupport  symbolicVal  annotation
#
### The grammar generator (ROSETTA) should use its own template repository
#CXX_TEMPLATE_REPOSITORY_PATH = .
#
#clean-local:
#
#distclean-local:
#
#
#
#
#
#
#
