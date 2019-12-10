include_directories(${QT_INCLUDES} ${Boost_INCLUDE_DIRS})

add_executable(testTranslator testTranslator.C)
add_dependencies(testTranslator roseUtil)
target_link_libraries(testTranslator ROSE_DLL ${edg_lib} ${link_with_libraries})
if(enable-java)
  add_dependencies(testTranslator JavaTraversal)
endif()

add_executable(testGraphGeneration testGraphGeneration.C)
target_link_libraries(testGraphGeneration ROSE_DLL EDG ${link_with_libraries})

add_executable(testTokenGeneration testTokenGeneration.C)
target_link_libraries(testTokenGeneration ROSE_DLL EDG ${link_with_libraries})

add_executable(testAnalysis testAnalysis.C)
target_link_libraries(testAnalysis ROSE_DLL EDG ${link_with_libraries})

option(with-ROSE_LONG_MAKE_CHECK_RULE "Specify longer internal testing" ON)

add_subdirectory(CompileTests)
add_subdirectory(roseTests)
#add_subdirectory(RunTests)
#add_subdirectory(PerformanceTests)
#add_subdirectory(CompilerOptionsTests)
add_subdirectory(translatorTests)

find_program(bash bash)
if(bash)
  add_test(
    NAME check_policies
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${bash} ${CMAKE_SOURCE_DIR}/scripts/policies-checker.sh)
endif()

install(TARGETS testTranslator DESTINATION bin)
