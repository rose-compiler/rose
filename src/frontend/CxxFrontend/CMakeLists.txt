# There are three ways to get the EDG library:
#   If C/C++ analysis is not configured in ROSE, we can use a mostly empty dummy library.
#   If we have EDG source code ("EDG" subdirectory) we can compile it.
#   Otherwise we can download a precompiled version from the ROSE team.

#if ROSE_BUILD_CXX_LANGUAGE_SUPPORT
if(ENABLE-C)
  if(ROSE_USE_CLANG_FRONTEND)
    message(STATUS "Selecting Clang for C/C++ frontend")
    # Autoconf reference: libroseCxxlibraries = frontend/CxxFrontend/Clang/libroseClangFrontend.la
    add_subdirectory(Clang)
  else()
    message(STATUS "Selecting EDG for C/C++ frontend")
    # Only add EDG subdirectory if we have EDG source code
    # When using precompiled EDG, the library is linked directly without a CMake subdirectory
    if(have_EDG_source AND EDG_COMPILE)
      add_subdirectory(EDG)
    endif()
  endif()
endif() # ENABLE-C
