# Liao 11/25/2009
# Converted from generate-backend-compiler-specific-headers.m4
#
# This should be called after roseChooseBackendCompiler.cmake is called

# GENERATE_BACKEND_CXX_COMPILER_SPECIFIC_HEADERS
# ----------------------------------------------------
  execute_process (
    COMMAND chmod u+x ${PROJECT_SOURCE_DIR}/config/create_system_headers
    COMMAND mkdir -p ${ROSE_TOP_BINARY_DIR}/include-staging/${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}_HEADERS
    )

    message ("Starting to run: ${PROJECT_SOURCE_DIR}/config/create_system_headers ${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH} ./include-staging/${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}_HEADERS ${PROJECT_SOURCE_DIR}")
# we split several commands into different execute_process ()
# otherwise cmake may skip executing the last ones!!!    
  execute_process(
#  /home/liao6/daily-test-rose/cmake/rose.cmake.git/config/create_system_headers c++ ./include-staging/c++_HEADERS /home/liao6/daily-test-rose/cmake/rose.cmake.git    
    COMMAND ${PROJECT_SOURCE_DIR}/config/create_system_headers ${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH} ./include-staging/${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}_HEADERS ${PROJECT_SOURCE_DIR}
    WORKING_DIRECTORY ${ROSE_TOP_BINARY_DIR}
    ERROR_VARIABLE errorVar
    )
# the error code is too strict. don't use it for now  
#  if (errorVar)
#    message (FATAL_ERROR "Could not run create_system_headers properly!!")
#  endif (errorVar)  
  set (ROSE_CXX_HEADERS_DIR "${CMAKE_INSTALL_PREFIX}/include/${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}_HEADERS")

  # copy another header , config/create_system_headers will create this for us
#  configure_file(${ROSE_TOP_SRC_DIR}/config/rose_edg_required_macros_and_functions.h.cmake ${ROSE_TOP_BINARY_DIR}/include-staging/${BACKEND_CXX_COMPILER_NAME_WITHOUT_PATH}_HEADERS/rose_edg_required_macros_and_functions.h)

# GENERATE_BACKEND_C_COMPILER_SPECIFIC_HEADERS
# ----------------------------------------------------
  execute_process (
    COMMAND chmod u+x ${PROJECT_SOURCE_DIR}/config/create_system_headers
    COMMAND mkdir -p ${ROSE_TOP_BINARY_DIR}/include-staging/${BACKEND_C_COMPILER_NAME_WITHOUT_PATH}_HEADERS
    )

    message ("Starting to run: ${PROJECT_SOURCE_DIR}/config/create_system_headers ${BACKEND_C_COMPILER_NAME_WITHOUT_PATH} ./include-staging/${BACKEND_C_COMPILER_NAME_WITHOUT_PATH}_HEADERS ${PROJECT_SOURCE_DIR}")
  execute_process(
    COMMAND ${PROJECT_SOURCE_DIR}/config/create_system_headers ${BACKEND_C_COMPILER_NAME_WITHOUT_PATH} ./include-staging/${BACKEND_C_COMPILER_NAME_WITHOUT_PATH}_HEADERS ${PROJECT_SOURCE_DIR}
    WORKING_DIRECTORY ${ROSE_TOP_BINARY_DIR}
    ERROR_VARIABLE errorVar
    )
# the error code is too strict. don't use it for now  
#  if (errorVar)
#    message (FATAL_ERROR "Could not run create_system_headers properly!!")
#  endif (errorVar)  
  set (ROSE_C_HEADERS_DIR "${CMAKE_INSTALL_PREFIX}/include/${BACKEND_C_COMPILER_NAME_WITHOUT_PATH}_HEADERS")


