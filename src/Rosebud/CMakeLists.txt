include_directories(. ${ROSE_INCLUDES})

add_executable(rosebud
  rosebud.C
  Ast.C
  BoostSerializer.C
  CxxGenerator.C
  Generator.C
  RoseGenerator.C
  RosettaGenerator.C
  Utility.C
  YamlGenerator.C)

add_executable(unitTest01
  rosebud.C
  Ast.C
  BoostSerializer.C
  CxxGenerator.C
  Generator.C
  RoseGenerator.C
  RosettaGenerator.C
  Utility.C
  YamlGenerator.C)

add_dependencies(rosebud util_sawyer)
add_dependencies(unitTest01 util_sawyer)

set(link_libs ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
if(WIN32)
  list(APPEND link_libs shlwapi.lib)
endif()

target_link_libraries(rosebud ${link_libs} util_sawyer)
target_link_libraries(unitTest01 ${link_libs} util_sawyer)
