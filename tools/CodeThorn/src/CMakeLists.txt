#
# Codethorn has never been tested on Windows, RWSetGeneration has linking errors
# on Windows Codethorn/src/ has some .h and .C files that require ifdef macros
# If we want it to work on Windows in the future
#
option(ENABLE-CODETHORN "Enable CodeThorn program analysis and verification" TRUE)
if(NOT WIN32 AND ENABLE-CODETHORN AND NOT ENABLE-CLANG-FRONTEND AND ENABLE-C STREQUAL "ON")
  if(NOT ENABLE_OPENMP)
    # Codethorn has openmp directives
    find_package(OpenMP QUIET)
    if(NOT OpenMP_FOUND)
      message(WARNING "Codethorn Requires OpenMP which was not found.")
    endif()
  endif()
  # all targets include these directories
  include_directories(. ${ROSE_INCLUDES} ${CMAKE_PREFIX_INSTALL}/include/rose
    ${CMAKE_BINARY_DIR})

  # all targets apply these flags TODO: Make this target-only flags
  add_compile_options(-fopenmp $<$<BOOL:${ENABLE_ALL_WARNINGS}>:-Wall>
    $<$<NOT:$<BOOL:${ENABLE_ALL_WARNINGS}>>:-Wno-misleading-indentation> -O3)

  # ##########################################################################
  # LIBRARY: CODETHORN (CORE)
  # ##########################################################################
  include(LibCodeThornSources.cmake) # source/header variables defined here

  # Create the target codethorn (libcodethorn.so) 
  add_library(codethorn SHARED ${CODETHORN_SOURCE_FILES})

  # Link libcodethorn.so to dependencies
  target_link_libraries(codethorn ROSE_DLL)

  # Install libcodethorn.so into lib
  install(TARGETS codethorn LIBRARY DESTINATION lib)

  # Header files for libcodethorn.so to be installed
  list(APPEND CODETHORN_ALL_HEADER_FILES ${CODETHORN_SOLO_HEADER_FILES}
    ${CODETHORN_HEADER_FILES})

  # Imstalls FILES in DESTINATION when user runs make install
  install(FILES ${CODETHORN_ALL_HEADER_FILES}
    DESTINATION include/rose/codethorn)

  # ##########################################################################
  # LIBRARY: LTLTHORN/ (for linear temporal logic verification)
  # ##########################################################################
  include(LibLTLThornSources.cmake) # source/header variables defined here

  # Create the target ltlthorn (libltlthorn.so)
  add_library(ltlthorn SHARED ${LTLTHORN_SOURCE_FILES})

  # Link libltlthorn.so to dependencies
  target_link_libraries(ltlthorn ROSE_DLL)

  # Install libltlthorn.so, don't install headers for this target
  install(TARGETS ltlthorn LIBRARY DESTINATION lib)

  # ##########################################################################
  # TOOL: CODETHORN
  # ##########################################################################
  add_executable(codethorn1 CodeThornCommandLineOptions.C codethorn.C)
  target_link_libraries(
    codethorn1 PRIVATE codethorn ltlthorn ROSE_DLL
    $<$<BOOL:${ENABLE-C}>:OpenMP::OpenMP_C>)

  install(TARGETS codethorn1 RUNTIME DESTINATION bin)

  # ##########################################################################
  # TOOL: LTLTHORN
  # ##########################################################################
  # Now add these targets to the tools build target.
  add_dependencies(tools codethorn ltlthorn codethorn1)
endif()
