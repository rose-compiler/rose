
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(partialRedundancyElimination)
add_subdirectory(finiteDifferencing)
add_subdirectory(functionCallNormalization)
add_subdirectory(constantFolding)
add_subdirectory(implicitCodeGeneration)
add_subdirectory(astInlining)
add_subdirectory(astOutlining)
add_subdirectory(loopProcessing)
add_subdirectory(ompLowering)




#add_library( midend_pt STATIC ${midend_pt_src}  )
#add_dependencies( midend_pt ROSETTA  )


########### install files ###############




#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#SUBDIRS = partialRedundancyElimination finiteDifferencing functionCallNormalization constantFolding implicitCodeGeneration 
#
#
#
