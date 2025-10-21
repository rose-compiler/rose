add_subdirectory(BinaryAnalysis)                              # Built by default
add_subdirectory(checkFortranInterfaces EXCLUDE_FROM_ALL)    # Not built by default
add_subdirectory(classMemberVariablesInLambdas EXCLUDE_FROM_ALL)
add_subdirectory(CodeThorn)
add_subdirectory(featureVector EXCLUDE_FROM_ALL)
add_subdirectory(globalVariablesInLambdas EXCLUDE_FROM_ALL)
add_subdirectory(PortabilityTesting EXCLUDE_FROM_ALL)
add_subdirectory(ReadWriteSetGeneration EXCLUDE_FROM_ALL)

#
# moveDeclarationToInnermostScope
#
set(tools_moveDeclarationToInnermostScope_SOURCES ./moveDeclarationToInnermostScope.C)

add_executable(tools_moveDeclarationToInnermostScope_exe
               ${tools_moveDeclarationToInnermostScope_SOURCES})

target_link_libraries(tools_moveDeclarationToInnermostScope_exe ROSE_DLL)

# TODO: When the refactor goes live, link libraries instead of doing this
# include hack.
target_include_directories(
  tools_moveDeclarationToInnermostScope_exe
  PRIVATE ${CMAKE_PREFIX_INSTALL}/include/rose
          ${ROSE_TOP_SRC_DIR}/src/midend/programTransformation/transformationTracking
          ${ROSE_INCLUDES})

set_target_properties(tools_moveDeclarationToInnermostScope_exe
                      PROPERTIES OUTPUT_NAME "moveDeclarationToInnermostScope")

install(TARGETS tools_moveDeclarationToInnermostScope_exe RUNTIME DESTINATION bin)

# Add the moveDecl tool to the custom build target.
add_dependencies(tools tools_moveDeclarationToInnermostScope_exe)
