
include_directories( 
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/driver
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/slicing
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/computation
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/depGraph
${CMAKE_SOURCE_DIR}/src/midend/programTransformation/loopProcessing/depInfo
${CMAKE_SOURCE_DIR}/src/midend/astUtil/astInterface
${CMAKE_SOURCE_DIR}/src/midend/astUtil/astSupport
${CMAKE_SOURCE_DIR}/src/midend/astUtil/symbolicVal
${CMAKE_SOURCE_DIR}/src/util/support 
${CMAKE_SOURCE_DIR}/src/util/graphs
${CMAKE_SOURCE_DIR}/src/frontend/SageIII
${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES})

add_definitions(-DPOET_VERSION="1.03.06")

########### next target ###############

#set(libpoet_SOURCES
#poet_AST.cpp codegen.cpp parse.cpp eval.cpp eval_ext.cpp
#evalOp.cpp driver.cpp tokenize.cpp timing.cpp AutoTuningInterface.cpp
#poet_yacc.c poet_lex.c)
#add_library(poet ${libpoet_SOURCES})

########### install files ###############
link_libraries(poet)
add_executable(pcg main.cpp)

install(
FILES poet_ASTfactory.h poet_SymbolTable.h poet_AST.h poet_ASTinterface.h 
poet_ASTvisitor.h poet_error.h poet_config.h poet_ASTeval.h timing.h
poetAST.h ASTfactory.h
DESTINATION ${INCLUDE_INSTALL_DIR})

