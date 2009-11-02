
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(unparser)
add_subdirectory(asmUnparser)


########### install files ###############




#original Makefile.am contents follow:

#include ../../config/Makefile.for.ROSE.includes.and.libs
#
#SUBDIRS = unparser asmUnparser
