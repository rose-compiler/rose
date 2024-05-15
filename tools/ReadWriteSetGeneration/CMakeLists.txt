option(ENABLE-RWSET-ANALYSIS "Enable rwset analysis" OFF)
if(ENABLE-RWSET-ANALYSIS)
  include_directories(${ROSE_INCLUDES} ${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/ReadWriteSetAnalysis)

  add_executable(LocalRWSetGenerator LocalRWSetGeneratorMain.C)
  target_link_libraries(LocalRWSetGenerator ROSE_DLL)

  add_executable(Name2Node Name2NodeMain.C)
  target_link_libraries(Name2Node ROSE_DLL)

  add_executable(TestabilityGrader TestabilityGraderMain.C)
  target_link_libraries(TestabilityGrader ROSE_DLL)


endif()
