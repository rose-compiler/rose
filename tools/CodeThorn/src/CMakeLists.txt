################################################################################
# LIBRARY: CODETHORN (CORE) 
################################################################################
# Codethorn library header files without corresponding source files
set(CODETHORN_SOLO_HEADER_FILES
    AnalysisParameters.h 
    CollectionOperators.h 
    CtxAnalysis.h 
    CtxAttribute.h 
    CtxLattice.h 
    CtxPropertyStateFactory.h 
    CtxTransfer.h 
    DataDependenceVisualizer.h 
    EqualityMaintainer.h 
    Evaluator.h 
    ExecutionTrace.h 
    HSetMaintainer.h 
    ReadWriteData.h 
    WorkListSeq.h
)

# Codethorn library header files (each with a corresponding source file)
set(CODETHORN_HEADER_FILES
    AbstractValue.h 
    addressTakenAnalysis.h 
    AliasAnalysis.h 
    AstUtility.h 
    AnalysisAstAnnotator.h 
    AnalysisReporting.h 
    CTAnalysis.h 
    ArrayElementAccessData.h 
    AstNodeInfo.h 
    BoolLattice.h 
    CallString.h 
    CFAnalysis.h 
    ClassHierarchyAnalysis.h 
    ClassHierarchyWriter.h 
    CodeThornLib.h 
    CodeThornPasses.h 
    CommandLineOptions.h 
    ContNodeAttribute.h 
    CounterexampleGenerator.h 
    CPAstAttribute.h 
    CppLabeler.h 
    CppStdUtilities.h 
    CTIOLabeler.h 
    CtxCallStrings.h 
    CtxPrioritySolver.h 
    CtxReachabilityLattice.h 
    CtxSolver0.h 
    CtxUnfoldedAnalysis.h 
    DataDependenceVisualizer.h 
    defUseQuery.h 
    DFAbstractSolver.h 
    DFAnalysisBaseWithoutData.h 
    DFAnalysisBase.h 
    CSDFAnalysisBase.h 
    CSDFAnalysisBaseWithoutData.h 
    ConstantConditionAnalysis.h 
    DFTransferFunctions.h 
    DotGraphCfgFrontend.h 
    EState.h 
    EStateTransferFunctions.h 
    EStateWorkList.h 
    EStatePriorityWorkList.h 
    FIConstAnalysis.h 
    Flow.h 
    FunctionCallMapping.h 
    FunctionCallMapping2.h 
    FunctionCallTarget.h 
    FunctionId.h 
    FunctionIdMapping.h 
    GeneralWorkList.h 
    GeneralWorkList.C 
    GeneralPriorityWorkList.h 
    GeneralPriorityWorkList.C 
    InputOutput.h 
    InternalChecks.h 
    IOAnalyzer.h 
    IOSequenceGenerator.h 
    IOSolver.h 
    LanguageRestrictorCollection.h 
    LanguageRestrictor.h 
    Lattice.h 
    LoopInfo.h 
    Miscellaneous2.h 
    Miscellaneous.h 
    Normalization.h 
    NormalizationCxx.h 
    NormalizationInliner.h 
    NormalizationOp.h 
    NumberIntervalLattice.h 
    Options.h 
    CodeThornOptions.h 
    ParProOptions.h 
    LTLOptions.h 
    LTLRersMapping.h 
    DFSolver1.h 
    DFSolver2.h 
    ObjectLayoutAnalysis.h 
    MemoryViolationAnalysis.h 
    PointerAnalysisInterface.h 
    PragmaHandler.h 
    PredefinedSemanticFunctions.h 
    ProgramAbstractionLayer.h 
    ProgramInfo.h 
    ProgramLocationsReport.h 
    ProgramLocationsAnalysis.h 
    PropertyState.h 
    PropertyStateFactory.h 
    PropertyValueTable.h 
    PState.h 
    ReachabilityAnalysis.h 
    ReadWriteListener.h 
    ReadWriteTraceAnalysis.h 
    RersSpecialization.h 
    RERS_empty_specialization.h 
    RersCounterexample.h 
    RewriteStatistics.h 
    RewriteSystem.h 
    RoseCompatibility.h 
    Solver5.h 
    Solver16.h 
    Solver17.h 
    Solver18.h 
    Solver19.h 
    Solver8.h 
    Solver.h 
    Specialization.h 
    SvcompWitness.h 
    TimeMeasurement.h 
    TimingCollector.h 
    TopologicalSort.h 
    TransitionGraph.h 
    TransitionGraphReducer.h 
    TypeSizeMapping.h 
    VariableIdUtils.h 
    VariableIdMappingExtended.h 
    VariableValueMonitor.h 
    Visualizer.h
)

# Codethorn library source files (each with a corresponding header file)
set(CODETHORN_SOURCE_FILES
    AbstractValue.C 
    addressTakenAnalysis.C 
    AliasAnalysis.C 
    AstUtility.C 
    AnalysisAstAnnotator.C 
    AnalysisReporting.C 
    CTAnalysis.C 
    ArrayElementAccessData.C 
    AstNodeInfo.C 
    BoolLattice.C 
    CallString.C 
    CFAnalysis.C 
    ClassHierarchyAnalysis.C 
    ClassHierarchyWriter.C 
    CodeThornLib.C 
    CodeThornPasses.C 
    CommandLineOptions.C 
    ContNodeAttribute.C 
    CounterexampleGenerator.C 
    CPAstAttribute.C 
    CppLabeler.C 
    CppStdUtilities.C 
    CTIOLabeler.C 
    CtxCallStrings.C 
    CtxPrioritySolver.C 
    CtxReachabilityLattice.C 
    CtxSolver0.C 
    CtxUnfoldedAnalysis.C 
    DataDependenceVisualizer.C 
    defUseQuery.C 
    DFAnalysisBaseWithoutData.C 
    DFAnalysisBase.C 
    CSDFAnalysisBase.C 
    ConstantConditionAnalysis.C 
    DFTransferFunctions.C 
    DotGraphCfgFrontend.C 
    EState.C 
    EStateTransferFunctions.C 
    EStateWorkList.C 
    EStatePriorityWorkList.C 
    FIConstAnalysis.C 
    Flow.C 
    FunctionCallMapping.C 
    FunctionCallMapping2.C 
    FunctionCallTarget.C 
    FunctionId.C 
    FunctionIdMapping.C 
    InputOutput.C 
    InternalChecks.C 
    IOAnalyzer.C 
    IOSequenceGenerator.C 
    IOSolver.C 
    LanguageRestrictorCollection.C 
    LanguageRestrictor.C 
    Lattice.C 
    LoopInfo.C 
    CodeThornOptions.C 
    LTLOptions.C 
    ParProOptions.C 
    LTLRersMapping.C 
    Miscellaneous2.C 
    Miscellaneous.C 
    Normalization.C 
    NormalizationCxx.C 
    NormalizationInliner.C 
    NormalizationOp.C 
    NumberIntervalLattice.C 
    DFSolver1.C 
    DFSolver2.C 
    ObjectLayoutAnalysis.C 
    MemoryViolationAnalysis.C 
    PointerAnalysisInterface.C 
    PragmaHandler.C 
    PredefinedSemanticFunctions.C 
    ProgramAbstractionLayer.C 
    ProgramInfo.C 
    ProgramLocationsReport.C 
    ProgramLocationsAnalysis.C 
    PropertyState.C 
    PropertyStateFactory.C 
    PropertyValueTable.C 
    PState.C 
    ReachabilityAnalysis.C 
    ReadWriteListener.C 
    ReadWriteTraceAnalysis.C 
    RERS_empty_specialization.C 
    RersCounterexample.C 
    RewriteStatistics.C 
    RewriteSystem.C 
    RoseCompatibility.C 
    Solver5.C 
    Solver16.C 
    Solver17.C 
    Solver18.C 
    Solver19.C 
    Solver8.C 
    Solver.C 
    Specialization.C 
    SvcompWitness.C 
    TimeMeasurement.C 
    TimingCollector.C 
    TopologicalSort.C 
    TransitionGraph.C 
    TransitionGraphReducer.C 
    TypeSizeMapping.C 
    VariableIdUtils.C 
    VariableIdMappingExtended.C 
    VariableValueMonitor.C 
    Visualizer.C
)

# Create the target codethorn (libcodethorn.so) "lib" is appended to the target name 
add_library(codethorn SHARED ${CODETHORN_SOURCE_FILES}) 

# Include these directories for the codethorn target (-I) 
target_include_directories(codethorn PUBLIC
                                     .
                                     ${ROSE_INCLUDES} 
                                     ${CMAKE_BINARY_DIR}/include/rose
                                     ${CMAKE_SOURCE_DIR}/src/frontend/SageIII 
                                     ${CMAKE_SOURCE_DIR}/src/midend/programTransformation/singleStatementToBlockNormalization
                                     ${CMAKE_SOURCE_DIR}/src/midend/programTransformation/extractFunctionArgumentsNormalization

)

# Link libcodethorn.so to librose.so 
target_link_libraries(codethorn PUBLIC ROSE_DLL midend_pt)

# Install libcodethorn.so 
install(TARGETS codethorn 
        LIBRARY DESTINATION lib)

# Install libcodethorn.so header files 
list(APPEND CODETHORN_ALL_HEADER_FILES ${CODETHORN_SOLO_HEADER_FILES} ${CODETHORN_HEADER_FILES})
install(FILES ${CODETHORN_ALL_HEADER_FILES} 
        DESTINATION include/rose/codethorn) 

################################################################################
# LIBRARY: LTLTHORN/ (for linear temporal logic verification)
################################################################################
set(LTLTHORN_HEADER_FILES 
    ltlthorn-lib/CounterexampleAnalyzer.h 
    ltlthorn-lib/LtsminConnection.h 
    ltlthorn-lib/ParProAutomataGenerator.h 
    ltlthorn-lib/ParProAutomata.h 
    ltlthorn-lib/ParProAnalyzer.h 
    ltlthorn-lib/ParProEState.h 
    ltlthorn-lib/ParProExplorer.h 
    ltlthorn-lib/ParProLtlMiner.h 
    ltlthorn-lib/ParProSpotState.h 
    ltlthorn-lib/ParProSpotSuccIter.h 
    ltlthorn-lib/ParProSpotTgba.h 
    ltlthorn-lib/ParProTransitionGraph.h 
    ltlthorn-lib/PromelaCodeGenerator.h 
    ltlthorn-lib/Solver10.h 
    ltlthorn-lib/Solver11.h 
    ltlthorn-lib/Solver12.h 
    ltlthorn-lib/SpotConnection.h 
    ltlthorn-lib/SpotMiscellaneous.h 
    ltlthorn-lib/SpotRenameVisitor.h 
    ltlthorn-lib/SpotState.h 
    ltlthorn-lib/SpotSuccIter.h 
    ltlthorn-lib/SpotTgba.h
)

set(LTLTHORN_SOURCE_FILES
    ltlthorn-lib/CounterexampleAnalyzer.C 
    ltlthorn-lib/LtsminConnection.C 
    ltlthorn-lib/ParProAutomataGenerator.C 
    ltlthorn-lib/ParProAutomata.C 
    ltlthorn-lib/ParProAnalyzer.C 
    ltlthorn-lib/ParProEState.C 
    ltlthorn-lib/ParProExplorer.C 
    ltlthorn-lib/ParProLtlMiner.C 
    ltlthorn-lib/ParProSpotState.C 
    ltlthorn-lib/ParProSpotSuccIter.C 
    ltlthorn-lib/ParProSpotTgba.C 
    ltlthorn-lib/ParProTransitionGraph.C 
    ltlthorn-lib/PromelaCodeGenerator.C 
    ltlthorn-lib/Solver10.C 
    ltlthorn-lib/Solver11.C 
    ltlthorn-lib/Solver12.C 
    ltlthorn-lib/SpotConnection.C 
    ltlthorn-lib/SpotMiscellaneous.C 
    ltlthorn-lib/SpotRenameVisitor.C 
    ltlthorn-lib/SpotState.C 
    ltlthorn-lib/SpotSuccIter.C 
    ltlthorn-lib/SpotTgba.C
    LTLThornLib.C
)

# only informative (not installed)
set(LIBLTLTHORN_XHEADERS  
    $(LTLTHORN_HEADER_FILES) 
    LTLThornLib.h
)

# library sources
# TODO: Determine why/if we need this 
# set(LIBLTLTHORN_LA_SOURCES
# $(LTLTHORN_SOURCE_FILES} 
#    LTLThornLib.C
#)

## Create the target ltlthorn (libltlthorn.so) 
add_library(ltlthorn SHARED ${LTLTHORN_SOURCE_FILES}) 

# Include these directories for the codethorn target (-I) 
target_include_directories(ltlthorn PRIVATE
                                     .
                                     ${ROSE_INCLUDES}
)

# Link libltlthorn.so to librose.so 
target_link_libraries(ltlthorn PRIVATE ROSE_DLL)

# Install libltlthorn.so, don't install headers for this target
install(TARGETS ltlthorn 
        LIBRARY DESTINATION lib)

################################################################################
# TOOL: CODETHORN
################################################################################
# may use blt for these things 
# blt_add_executable(NAME codethorn1
#                   SOURCES  CodeThornCommandLineOptions.C codethorn.C)
#                   OUTPUT_DIR ${CMAKE_INSTALL_DIRECTORY}/bin
#                   DEPENDS_ON blt::openmp
#                   )

# For CI/CD pipeline 
if(NOT TARGET OpenMP::OpenMP_CXX) 
  find_package(OpenMP REQUIRED) 
  message(STATUS "Finding OpenMP for Codethorn") 
endif() 

add_executable(codethorn1 CodeThornCommandLineOptions.C codethorn.C)
target_link_libraries(codethorn1 PRIVATE codethorn ltlthorn ROSE_DLL OpenMP::OpenMP_CXX)
install(TARGETS codethorn1 
        RUNTIME DESTINATION bin)

################################################################################
# TOOL: LTLTHORN
################################################################################

