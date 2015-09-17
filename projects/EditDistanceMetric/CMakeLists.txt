
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(astDiff_SRCS
    astDiff.C
    sequenceGeneration.C
    treeIsomorphism.C
    sequenceAlignment.C
    heatMap.C
    attributeSupport.C
    treeEditDistance.C)

kde4_add_executable(astDiff ${astDiff_SRCS})

target_link_libraries(astDiff ${KDE4_KDECORE_LIBS})


########### install files ###############
