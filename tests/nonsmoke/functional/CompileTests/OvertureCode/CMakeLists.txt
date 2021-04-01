set(INCLUDES
  -I${CMAKE_CURRENT_BINARY_DIR}
  -I${CMAKE_CURRENT_SOURCE_DIR}
  -I${CMAKE_CURRENT_SOURCE_DIR}/../A++Code)

add_test(
  NAME overtureBug_01
  COMMAND testTranslator ${INCLUDES}
    -c ${CMAKE_CURRENT_SOURCE_DIR}/overtureBug_01.C)
  set_tests_properties(overtureBug_01 PROPERTIES LABELS OVERTURETEST)
