find_path(XERCES_INCLUDE_DIR xerces DOC "Path to the xerces directory")
include_directories(${ROSE_SOURCE_DIR}/projects ${XERCES_INCLUDE_DIR})

add_executable(compass2
  bin/compass_main.cpp
  bin/checkers/variable_name_similarity/variable_name_similarity.cpp
  bin/checkers/keyword_macro/keyword_macro.cpp
  bin/checkers/non_static_array_size/non_static_array_size.cpp
  bin/checkers/function_prototype/function_prototype.cpp
  bin/checkers/function_pointer/function_pointer.cpp
  bin/checkers/non_global_cpp_directive/non_global_cpp_directive.cpp
  bin/checkers/default_argument/default_argument.cpp
  bin/checkers/function_with_multiple_returns/function_with_multiple_returns.cpp
  bin/checkers/global_variables/global_variables.cpp
  bin/checkers/dead_function/dead_function.cpp
  compass.cpp
)
