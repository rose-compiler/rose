include_directories(${ROSE_INCLUDES})

add_subdirectory(arrayOptimization)
add_subdirectory(autoParallelization)
add_subdirectory(BabelPreprocessor)
add_subdirectory(BinFuncDetect)
add_subdirectory(bugSeeding)
add_subdirectory(C_to_Promela)
#add_subdirectory(CertSecureCodeProject)
add_subdirectory(CloneDetection)
add_subdirectory(DataFaultTolerance)
add_subdirectory(DocumentationGenerator)
add_subdirectory(extractMPISkeleton)
add_subdirectory(graphColoring)
add_subdirectory(highLevelGrammars)
add_subdirectory(mint)
add_subdirectory(MPI_Tools)
add_subdirectory(OpenMP_Analysis)
add_subdirectory(PowerAwareCompiler)
add_subdirectory(pragmaParsing)
add_subdirectory(RTC)
add_subdirectory(taintcheck)
add_subdirectory(UpcTranslation)


if(OPENSSL_FOUND)
  add_subdirectory(BinaryCloneDetection)
endif()

if(OPENGL_FOUND)
  add_subdirectory(binaryVisualization)
endif()

if(enable-compass2)
  add_subdirectory(compass2)
endif()

if(PYTHONINTERP_FOUND)
  add_subdirectory(DatalogAnalysis)
endif()

if(GCJ)
  #add_subdirectory(FiniteStateModelChecker)
endif()

if (enable-fortran)
  add_subdirectory(Fortran_to_C)
endif()

# These targets do not compile using ROSE
if(NOT ROSE_USING_ROSE_ANALYSIS)
  if(HAVE_REALPATH)
    add_subdirectory(HeaderFilesInclusion)
  endif()
endif()

if(SQLITE3_FOUND)
  add_subdirectory(MacroRewrapper)
endif()

# highresTimer.C needs to be rewritten so that it is cross-platform
if(NOT WIN32)
  add_subdirectory(ManyCoreRuntime)
endif()

if(with-yices)
  add_subdirectory(OpenMP_Checker)
endif()

if(with-ppl)
  add_subdirectory(PolyhedralModel)
endif()

if(with-maple)
  add_subdirectory(programModeling)
endif()

if(with-llvm)
  add_subdirectory(roseToLLVM)
endif()


if (with-roseQt)
  include_directories(
    ${QT_QT3SUPPORT_INCLUDE_DIR} ${ROSE_BINARY_DIR}/src/roseSupport
  )
  add_definitions(-DQT3_SUPPORT)

  add_subdirectory(AstEquivalence)
  add_subdirectory(QtDesignerPlugins)
  add_subdirectory(RoseQt)
endif()


# target was commented out of Makefile
#add_subdirectory(checkPointExample)

#add_subdirectory(javaport)
#add_subdirectory(minitermite)

