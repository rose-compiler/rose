
# I'm not sure what this does, ignoring for now
#include_directories(${CMAKE_SOURCE_DIR}/src/midend/loopProcessing ${CMAKE_SOURCE_DIR}/src/midend/astUtil ${CMAKE_SOURCE_DIR}/src/midend/astUtil/astInterface ${CMAKE_SOURCE_DIR}/src/midend/astUtil/annotation ${CMAKE_SOURCE_DIR}/src/midend/astUtil/symbolicVal ${CMAKE_SOURCE_DIR}/src/midend/astUtil/astSupport ${CMAKE_SOURCE_DIR}/src/util/support ${CMAKE_SOURCE_DIR}/src/util/graphs ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############


########### install files ###############

install(FILES  LocalRWSetGenerator.h  sageUtility.h VxUtilFuncs.h Name2Node.h SageUtils.h ReadWriteSetRecords.h  TestabilityGrader.h  DESTINATION ${INCLUDE_INSTALL_DIR})

