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

# Link to what librose.so links on Windows, Mac, or Linux using link_with_libraries
target_link_libraries(rosebud ${link_with_libraries} util_sawyer)
install(TARGETS rosebud DESTINATION bin)
