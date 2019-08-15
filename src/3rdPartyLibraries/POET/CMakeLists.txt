if(enable-poet)
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
  )

  add_definitions(-DPOET_VERSION="1.03.06")

  add_library(poet
    AutoTuningInterface.cpp
    codegen.cpp
    tokenize.cpp
    driver.cpp
    eval.cpp
    evalOp.cpp
    eval_ext.cpp
    parse.cpp
    poet_AST.cpp
    timing.cpp
    poet_lex.c
    poet_yacc.c)

  add_executable(pcg main.cpp)
  target_link_libraries(pcg ROSE_DLL ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})


  ########### install files ###############
  install(TARGETS pcg DESTINATION bin)

  install(
  FILES poet_ASTfactory.h poet_SymbolTable.h poet_AST.h poet_ASTinterface.h 
  poet_ASTvisitor.h poet_error.h poet_config.h poet_ASTeval.h timing.h
  poetAST.h ASTfactory.h
  DESTINATION ${INCLUDE_INSTALL_DIR})
endif()
