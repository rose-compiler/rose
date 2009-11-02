
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(partialRedundancyElimination)
add_subdirectory(finiteDifferencing)
add_subdirectory(functionCallNormalization)
add_subdirectory(constantFolding)
add_subdirectory(implicitCodeGeneration)


########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#SUBDIRS = partialRedundancyElimination finiteDifferencing functionCallNormalization constantFolding implicitCodeGeneration 
#
#
#
