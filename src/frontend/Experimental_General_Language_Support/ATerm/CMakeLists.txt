# Library target for libroseATerm
add_library(roseATerm STATIC
  ATermTraversal.C
)

# Rosetta has to be built before this to find Cxx_Grammar.h 
add_dependencies(roseATerm rosetta_generated)

target_include_directories(roseATerm PRIVATE
  ${ATERM_ROOT}/include
  ${CMAKE_SOURCE_DIR}/src/frontend/Experimental_General_Language_Support
)
