
add_custom_command(
  OUTPUT output/RelationVisitor.cc output/RelationVisitor.hh
  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_SOURCE_DIR}/output
  COMMAND ${CMAKE_COMMAND} -D PYTHON_EXECUTABLE=${PYTHON_EXECUTABLE}
                           -D SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
                           -P ${CMAKE_CURRENT_SOURCE_DIR}/generate_relation_source_code.cmake
)

add_library(astBinaryRelations output/RelationVisitor.cc)

