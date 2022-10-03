if(enable-projects-directory)
  include_directories(${ROSE_INCLUDES})

  ###################################################################################################################
  # Some projects are disabled by commenting them out even when projects in general are enabled. This is done when a
  # project is not portable, does not pass testing, or is no longer maintained.  If you are the project owner, please
  # test your project and re-enable it.
  ###################################################################################################################

  add_subdirectory(CloneDetection)
  add_subdirectory(ManyCoreRuntime)
  add_subdirectory(MPI_Tools)
endif()
