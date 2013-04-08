
set(output_dir ${CMAKE_CURRENT_SOURCE_DIR}/output)
add_custom_command(
  OUTPUT ${output_dir}/RelationVisitor.cc ${output_dir}/RelationVisitor.hh
  COMMAND ${CMAKE_COMMAND} -E make_directory ${output_dir}
  COMMAND ${CMAKE_COMMAND} -D PYTHON_EXECUTABLE=${PYTHON_EXECUTABLE}
                           -D SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
                           -P ${CMAKE_CURRENT_SOURCE_DIR}/generate_relation_source_code.cmake
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(astBinaryRelations output/RelationVisitor.cc)
add_dependencies(astBinaryRelations rosetta_generated)

