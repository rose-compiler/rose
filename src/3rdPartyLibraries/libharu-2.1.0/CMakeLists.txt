if(NOT enable-internalFrontendDevelopment)
  find_package(PNG)
  if (NOT PNG_FOUND)
    add_definitions(-DHPDF_NOPNGLIB)
  endif()

  add_subdirectory(src)
  add_subdirectory(include)
endif()
