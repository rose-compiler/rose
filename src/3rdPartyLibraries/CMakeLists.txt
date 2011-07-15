
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(MSTL)
add_subdirectory(libharu-2.1.0)
#add_subdirectory(libharu-200910170404)
add_subdirectory(fortran-parser)
add_subdirectory(antlr-jars)
add_subdirectory(POET)
#add_subdirectory(qrose)


########### install files ###############




#original Makefile.am contents follow:

## This should not depend on ROSE in any way so I don't think we need this
## include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#SUBDIRS = MSTL libharu-2.1.0 fortran-parser antlr-jars qrose
## checkPointLibrary
#
