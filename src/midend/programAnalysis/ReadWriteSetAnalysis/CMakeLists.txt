include_directories(${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/ReadWriteSetAnalysis)


########### next target ###############


########### install files ###############

install(FILES  LocalRWSetGenerator.h  sageUtility.h VxUtilFuncs.h Name2Node.h SageUtils.h ReadWriteSetRecords.h  TestabilityGrader.h  DESTINATION ${INCLUDE_INSTALL_DIR})

