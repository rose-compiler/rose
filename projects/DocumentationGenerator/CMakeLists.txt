
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_custom_command(
  OUTPUT doxygenComment.C
  COMMAND ${FLEX_EXECUTABLE} -Pdx -odoxygenComment.C
          ${CMAKE_CURRENT_SOURCE_DIR}/doxygenComment.lex
)

add_library(ROSEDoxygen
  commentObj.C doxygenComment.C doxygenGrammar.C sageDoxygen.C
)
add_dependencies(ROSEDoxygen rosetta_generated)

set(DocGenExes
  correctAllComments correctExternalComments classifyDeprecated
  createDocumentationForROSE docLint
)

#this one needs execvp
if(HAVE_UNISTD_H)
  list(APPEND DocGenExes correctExternalCommentsAndCompile)
endif()

foreach(DocGenExe ${DocGenExes})
  add_executable(${DocGenExe} ${DocGenExe}.C)
  target_link_libraries(${DocGenExe} ROSE_DLL ${Boost_LIBRARIES} ROSEDoxygen)
endforeach()

