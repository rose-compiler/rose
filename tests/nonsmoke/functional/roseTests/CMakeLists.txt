# DQ (4/17/2010): These directories are now organized based on what sets of
# languages are enabled in ROSE (including binary analysis).

if(enable-c)
  add_subdirectory (astMergeTests)
  add_subdirectory (astPerformanceTests)
  add_subdirectory (astProcessingTests)
  add_subdirectory (astQueryTests)
  add_subdirectory (astRewriteTests)
  add_subdirectory (astSymbolTableTests)
  add_subdirectory (astTokenStreamTests)
  if (NOT CYGWIN)
    add_subdirectory (programTransformationTests)
  endif()
  #add_subdirectory (graph_tests)
  #add_subdirectory (mergeTraversal_tests)
  #add_subdirectory (astLValueTests)
  #add_subdirectory (abstractMemoryObjectTests)

  if (NOT with-insure)
    #add_subdirectory (roseHPCToolkitTests)
  endif ()

  # TOO (2/16/2011): Errors with Thrifty (GCC 3.4.4) backend compiler. We can
  # gradually enable these tests at a later stage if necessary.
  set(c_compiler_version
  "${BACKEND_C_COMPILER_MAJOR_VERSION_NUMBER}.${BACKEND_C_COMPILER_MINOR_VERSION_NUMBER}.${BACKEND_C_COMPILER_PATCH_LEVEL_NUMBER}")

  if ( (NOT CMAKE_COMPILER_S_GNUCC) OR (NOT ${c_compiler_version} STREQUAL
  "3.4.4") )

    # TOO (2/17/11): Errors with Tensilica's Xtensa compilers.
    if (NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Xtensa")
      #add_subdirectory (astFileIOTests)

      if (NOT with-insure)
        #add_subdirectory (astInterfaceTests)
        #add_subdirectory (astInliningTests)
        #add_subdirectory (ompLoweringTests)
        #add_subdirectory (programAnalysisTests)
        #add_subdirectory (astOutliningTests)
        #add_subdirectory (loopProcessingTests)
        #add_subdirectory (fileLocation_tests)
      endif() #insure
    endif() #xtensa
  endif() # gcc 3.4.4
endif() # C/C++

if (enable-php)
  #add_subdirectorY(PHPTests)
endif()

if (NOT with-insure)
  # DQ (4/23/2011): At least one of the files (process_cmdline.o) in this
  # directory does not seem to compile with Insure++ (it hangs for Hudson, but
  # I can't reproduce the problem locally).
  # This was not a problem the second time that Insure++ compiled ROSE.
  #add_subdirectory (utilTests)

  # Binary tests are a current problem for Insure++.
  if (enable-binary-analysis)
    #add_subdirectory(binaryTests)
  endif()
endif()
