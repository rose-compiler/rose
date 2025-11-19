add_subdirectory(BinaryAnalysis)
add_subdirectory(checkFortranInterfaces)
add_subdirectory(classMemberVariablesInLambdas)
add_subdirectory(CodeThorn)
add_subdirectory(featureVector)
add_subdirectory(globalVariablesInLambdas)
add_subdirectory(PortabilityTesting)
add_subdirectory(ReadWriteSetGeneration)

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
