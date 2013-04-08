
add_executable(overloadedOperatorRepresentation
  overloadedOperatorRepresentation.C AbstractionAttribute.C)
target_link_libraries(overloadedOperatorRepresentation
  ROSE_DLL ${Boost_LIBRARIES})

add_executable(highLevelGrammarGenerator highLevelGrammarGenerator.C)
target_link_libraries(highLevelGrammarGenerator ROSE_DLL ${Boost_LIBRARIES})

add_executable(exampleTranslator exampleTranslator.C)
target_link_libraries(exampleTranslator ROSE_DLL ${Boost_LIBRARIES})
