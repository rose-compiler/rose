# TO DO: only make Codethorn if make tools/make install-tools
option(ENABLE-RWSET-ANALYSIS "Enable rwset analysis" ON)
if(ENABLE-RWSET-ANALYSIS)
  include_directories(${ROSE_INCLUDES}
                      ${CMAKE_SOURCE_DIR}/src/midend/programAnalysis/ReadWriteSetAnalysis) 

  add_executable(LocalRWSetGenerator LocalRWSetGeneratorMain.C)
  target_link_libraries(LocalRWSetGenerator ROSE_DLL)
  install(TARGETS LocalRWSetGenerator RUNTIME DESTINATION bin) 

  add_executable(Name2Node Name2NodeMain.C)
  target_link_libraries(Name2Node ROSE_DLL)
  install(TARGETS Name2Node RUNTIME DESTINATION bin)

  add_executable(TestabilityGrader TestabilityGraderMain.C)
  target_link_libraries(TestabilityGrader ROSE_DLL)
  install(TARGETS TestabilityGrader RUNTIME DESTINATION bin)
endif()
