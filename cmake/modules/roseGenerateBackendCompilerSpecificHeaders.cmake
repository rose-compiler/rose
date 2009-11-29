# Liao 11/25/2009
# Converted from generate-backend-compiler-specific-headers.m4
#
# This should be called after roseChooseBackendCompiler.cmake is called
#macro generateBackendCxxCompilerSpecificHeaders 

  execute_process (
    COMMAND chmod u+x ${PROJECT_SOURCE_DIR}/config/create_system_headers
    # compilerName = ${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}
    COMMAND mkdir -p ${ROSE_TOP_BINARY_DIR}/include-staging/${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}_HEADERS
    COMMAND cd ${ROSE_TOP_BINARY_DIR}
    COMMAND ${ROSE_TOP_SRC_DIR}/config/create_system_headers ${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH} ./include-staging/${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}_HEADERS ${ROSE_TOP_BINARY_DIR}
    )

  set (ROSE_CXX_HEADERS_DIR "${CMAKE_INSTALL_PREFIX}/include/${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}_HEADERS")

  # copy another header 
  configure_file(${ROSE_TOP_SRC_DIR}/config/rose_edg_required_macros_and_functions.h.cmake ${ROSE_TOP_BINARY_DIR}/include-staging/${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}_HEADERS/rose_edg_required_macros_and_functions.h)

#endmacro generateBackendCxxCompilerSpecificHeaders
