# These files install in the same location as the Autoconf system
# i.e., /<rose-install-dir>/include
# rose-install-dir == ${CMAKE_INSTALL_PREFIX}
set(ROSE_CONFIG_HEADER_FILES rose-g++-headerfilefixup.h
                             rose_specific_ammintrin.h
                             rose_specific_cdefs.h
                             rose_specific_emmintrin.h
                             rose_specific_nmmintrin.h
                             rose_specific_pmmintrin.h
                             rose_specific_smmintrin.h
                             rose_specific_tmmintrin.h
                             rose_specific_xmmintrin.h
                             rose_stdarg.h
                             rose_varargs.h
)

# Check if the compiler is GNU and the version is later than 4.6
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "4.6")
    list(APPEND ROSE_CONFIG_HEADER_FILES rose_specific_avxintrin.h) 
    if(NOT DEFINED USING_GCC_LATER_THAN_4_6) 
      set(USING_GCC_LATER_THAN_4_6 1) 
    endif()
  endif()
endif()

install(FILES ${ROSE_CONFIG_HEADER_FILES} 
        DESTINATION ${INCLUDE_INSTALL_DIR_FOR_INTRINSICS})
