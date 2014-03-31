include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

# DQ (6/8/2014): Added header files and source files below.
install(FILES RoseAst.h matcherparser_decls.h AstMatching.h MatchOperation.h DESTINATION ${INCLUDE_INSTALL_DIR})

#${CMAKE_SOURCE_DIR}/src/midend/astMatching/AstMatching.C
#${CMAKE_SOURCE_DIR}/src/midend/astMatching/matcherlexer.ll
#${CMAKE_SOURCE_DIR}/src/midend/astMatching/matcherparser.yy
#${CMAKE_SOURCE_DIR}/src/midend/astMatching/MatchOperation.C

########### next target ###############

#add_library( midend_processing  ${libastprocessing_SRC} )
#add_dependencies( midend_processing ROSETTA )

