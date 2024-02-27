include_directories(. ${ROSE_INCLUDES})

add_executable(rosebud
  rosebud.C
  Ast.C
  BoostSerializer.C
  CerealSerializer.C
  CxxGenerator.C
  Generator.C
  NoneGenerator.C
  NoneSerializer.C
  RosettaGenerator.C
  SawyerGenerator.C
  Serializer.C
  Utility.C
  YamlGenerator.C)

add_dependencies(rosebud util_sawyer)

set(link_libs ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})
if(WIN32)
  list(APPEND link_libs shlwapi.lib)
endif()

target_link_libraries(rosebud ${link_libs} util_sawyer)
