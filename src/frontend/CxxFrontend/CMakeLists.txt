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
    # have_EDG_source AND !ROSE_USE_CLANG_FRONTEND)
  else()   
    message(STATUS "Selecting EDG for C/C++ frontend")
    # 10/14/24 Removed have_EDG_source for this condition to be true 
    add_subdirectory(EDG)
  endif() 
endif() # ENABLE-C
