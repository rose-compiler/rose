
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

#${CMAKE_SOURCE_DIR}/src/midend/astMatching/AstMatching.C
#${CMAKE_SOURCE_DIR}/src/midend/astMatching/matcherlexer.ll
#${CMAKE_SOURCE_DIR}/src/midend/astMatching/matcherparser.yy
#${CMAKE_SOURCE_DIR}/src/midend/astMatching/MatchOperation.C

########### next target ###############

#add_library( midend_processing  ${libastprocessing_SRC} )
#add_dependencies( midend_processing ROSETTA )


########### install files ###############

if (WIN32)
#tps commented out AstSharedMemoryParallelProcessing.h for Windows 
install(FILES AstMatching.h matcherparser_decls.h MatchOperation.h RoseAst.h DESTINATION ${INCLUDE_INSTALL_DIR})
else (WIN32)
install(FILES AstMatching.h matcherparser_decls.h MatchOperation.h RoseAst.h DESTINATION ${INCLUDE_INSTALL_DIR})
endif (WIN32)


