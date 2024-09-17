# Library target for libroseATerm
add_library(roseATerm STATIC
  ATermTraversal.C
)

target_include_directories(roseATerm PRIVATE
  ${ATERM_ROOT}/include
  ${CMAKE_SOURCE_DIR}/src/frontend/Experimental_General_Language_Support
)
