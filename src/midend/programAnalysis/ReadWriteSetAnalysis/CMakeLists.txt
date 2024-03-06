# Include the directories for header files
include_directories(
  ${PROJECT_SOURCE_DIR}/config
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  ${CMAKE_SOURCE_DIR}/src/midend/loopProcessing
  ${CMAKE_SOURCE_DIR}/src/midend/astUtil
  ${CMAKE_SOURCE_DIR}/src/util/support
  ${CMAKE_SOURCE_DIR}/src/util
  ${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/ReadWriteSetAnalysis
)

# This Breaks --  Cxx_Grammar.h not found 
#set(readWriteSetSources
#  TestabilityGrader.C
#  LocalRWSetGenerator.C
#  ReadWriteSetRecords.C
#  VxUtilFuncs.C
#  Name2Node.C
#  SageUtils.C
#)
# Define the library to be built from the source files
#add_library(ReadWriteSet STATIC ${readWriteSetSources})

# Not sure  if this is needed 
set(CXX_TEMPLATE_REPOSITORY_PATH ".")

########### next target ###############


########### install files ###############
install(FILES  LocalRWSetGenerator.h  sageUtility.h VxUtilFuncs.h Name2Node.h SageUtils.h ReadWriteSetRecords.h  TestabilityGrader.h  DESTINATION ${INCLUDE_INSTALL_DIR})

