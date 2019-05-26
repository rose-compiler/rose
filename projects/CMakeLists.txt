if(enable-projects-directory)
  include_directories(${ROSE_INCLUDES})

  ###################################################################################################################
  # Some projects are disabled by commenting them out even when projects in general are enabled. This is done when a
  # project is not portable, does not pass testing, or is no longer maintained.  If you are the project owner, please
  # test your project and re-enable it.
  ###################################################################################################################

  add_subdirectory(BinaryAnalysisTools)
  add_subdirectory(BinaryCloneDetection)
  add_subdirectory(CloneDetection)
  add_subdirectory(extractMPISkeleton)
  add_subdirectory(Fortran_to_C)
  add_subdirectory(ManyCoreRuntime)
  add_subdirectory(MatrixTestingProject)
  add_subdirectory(MPI_Tools)
  add_subdirectory(pragmaParsing)
  add_subdirectory(UpcTranslation)
endif()
