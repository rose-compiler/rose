add_subdirectory(CallGraphAnalysis)
add_subdirectory(OAWrap)

# DQ (11/25/2009) Comment out under Windows.
if(WIN32)
  message("Commented out use of OpenAnalysis in Windows.")
else()
  add_subdirectory(CFG)
endif()

add_subdirectory(bitvectorDataflow)
add_subdirectory(VirtualFunctionAnalysis)
add_subdirectory(valuePropagation)
add_subdirectory(defUseAnalysis)
add_subdirectory(dominanceAnalysis)
add_subdirectory(staticInterproceduralSlicing)
add_subdirectory(staticSingleAssignment)
add_subdirectory(pointerAnal)
add_subdirectory(annotationLanguageParser)
add_subdirectory(distributedMemoryAnalysis)
add_subdirectory(variableRenaming)
add_subdirectory(genericDataflow)
add_subdirectory(ssaUnfilteredCfg)
add_subdirectory(EditDistance)
add_subdirectory(ReadWriteSetAnalysis)

# A separate, Windows-specific set of source files is needed to pass Windows CI/CD 

# Define source files for midend_pa (list1)
set(LIST1_FILES
  defUseAnalysis/DefUseAnalysisAbstract.cpp
  defUseAnalysis/DefUseAnalysis.cpp
  defUseAnalysis/GlobalVarAnalysis.cpp
  defUseAnalysis/LivenessAnalysis.cpp
  defUseAnalysis/dfaToDot.cpp
  defUseAnalysis/DefUseAnalysis_perFunction.cpp
  valuePropagation/ValueAnnot.C
  valuePropagation/ArrayAnnot.C
  valuePropagation/ValuePropagate.C
  variableRenaming/VariableRenaming.C
  OAWrap/OAWrap.C
  OAWrap/SAGE2OA.C
  CallGraphAnalysis/CallGraphDB.C
  CallGraphAnalysis/CallGraph.C
  CallGraphAnalysis/ClassHierarchyGraph.C
  staticInterproceduralSlicing/MergedDependenceGraph.C
  staticInterproceduralSlicing/ControlFlowGraph.C
  staticInterproceduralSlicing/FunctionDependenceGraph.C
  staticInterproceduralSlicing/ControlDependenceGraph.C
  staticInterproceduralSlicing/DependenceGraph.C
  staticInterproceduralSlicing/EDefUse.C
  staticInterproceduralSlicing/DataDependenceGraph.C
  staticInterproceduralSlicing/SlicingInfo.C
  staticInterproceduralSlicing/CreateSlice.C
  staticInterproceduralSlicing/CreateSliceSet.C
  staticInterproceduralSlicing/SystemDependenceGraph.C
  staticInterproceduralSlicing/DefUseExtension.C
  CFG/CFG_ROSE.C
  pointerAnal/PtrAnalCFG.C
  pointerAnal/PtrAnal.C
  bitvectorDataflow/DataFlowAnalysis.C
  bitvectorDataflow/ReachingDefinition.C
  bitvectorDataflow/DefUseChain.C
  VirtualFunctionAnalysis/VirtualFunctionAnalysis.C
  VirtualFunctionAnalysis/PtrAliasAnalysis.C
  VirtualFunctionAnalysis/IntraProcAliasAnalysis.C
  VirtualFunctionAnalysis/IntraProcDataFlowAnalysis.C
  VirtualFunctionAnalysis/InterProcDataFlowAnalysis.C
  staticSingleAssignment/staticSingleAssignmentCalculation.C
  staticSingleAssignment/staticSingleAssignmentQueries.C
  staticSingleAssignment/uniqueNameTraversal.C
  staticSingleAssignment/defsAndUsesTraversal.C
  staticSingleAssignment/reachingDef.C
  staticSingleAssignment/staticSingleAssignmentInterprocedural.C
  EditDistance/EditDistance.C
  EditDistance/TreeEditDistance.C
  ReadWriteSetAnalysis/LocalRWSetGenerator.C
  ReadWriteSetAnalysis/Name2Node.C
  ReadWriteSetAnalysis/ReadWriteSetRecords.C
  ReadWriteSetAnalysis/SageUtils.C
  ReadWriteSetAnalysis/TestabilityGrader.C
  ReadWriteSetAnalysis/VxUtilFuncs.C
)

# Define source files for midend_pa (list2)
set(LIST2_FILES
  defUseAnalysis/DefUseAnalysisAbstract.cpp
  defUseAnalysis/DefUseAnalysis.cpp
  defUseAnalysis/GlobalVarAnalysis.cpp
  defUseAnalysis/LivenessAnalysis.cpp
  defUseAnalysis/dfaToDot.cpp
  defUseAnalysis/DefUseAnalysis_perFunction.cpp
  valuePropagation/ValueAnnot.C
  valuePropagation/ArrayAnnot.C
  valuePropagation/ValuePropagate.C
  variableRenaming/VariableRenaming.C
  OAWrap/OAWrap.C
  OAWrap/SAGE2OA.C
  genericDataflow/analysis/analysis.C
  genericDataflow/analysis/analysisCommon.C
  genericDataflow/analysis/dataflow.C
  genericDataflow/analysis/partitionedAnalysis.C
  genericDataflow/AnalysisDebuggingUtils.C
  genericDataflow/arrIndexLabeler/arrIndexLabeler.C
  genericDataflow/cfgUtils/CallGraphTraverse.C
  genericDataflow/cfgUtils/cfgUtils.C
  genericDataflow/cfgUtils/CFGRewrite.C
  genericDataflow/cfgUtils/DataflowCFG.C
  genericDataflow/cfgUtils/VirtualCFGIterator.C
  genericDataflow/genUID.C
  genericDataflow/lattice/affineInequality.C
  genericDataflow/lattice/ConstrGraph.C
  genericDataflow/lattice/lattice.C
  genericDataflow/rwAccessLabeler/rwAccessLabeler.C
  genericDataflow/simpleAnalyses/ConstrGraphAnalysis.C
  genericDataflow/simpleAnalyses/divAnalysis.C
  genericDataflow/simpleAnalyses/dominatorAnalysis.C
  genericDataflow/simpleAnalyses/liveDeadVarAnalysis.C
  genericDataflow/simpleAnalyses/nodeConstAnalysis.C
  genericDataflow/simpleAnalyses/placeUIDs.C
  genericDataflow/simpleAnalyses/printAnalysisStates.C
  genericDataflow/simpleAnalyses/saveDotAnalysis.C
  genericDataflow/simpleAnalyses/sequenceStructAnalysis.C
  genericDataflow/simpleAnalyses/sgnAnalysis.C
  genericDataflow/simpleAnalyses/taintAnalysis.C
  genericDataflow/state/functionState.C
  genericDataflow/state/nodeState.C
  genericDataflow/variables/variables.C
  genericDataflow/variables/varSets.C
  ssaUnfilteredCfg/ssaUnfilteredCfgCalculation.C
  ssaUnfilteredCfg/ssaUnfilteredCfgQueries.C
  ssaUnfilteredCfg/defsAndUsesUnfilteredCfg.C
  ssaUnfilteredCfg/reachingDefUnfilteredCfg.C
  CallGraphAnalysis/CallGraphDB.C
  CallGraphAnalysis/CallGraph.C
  CallGraphAnalysis/ClassHierarchyGraph.C
  CallGraphAnalysis/newCallGraph.C
  staticInterproceduralSlicing/MergedDependenceGraph.C
  staticInterproceduralSlicing/ControlFlowGraph.C
  staticInterproceduralSlicing/FunctionDependenceGraph.C
  staticInterproceduralSlicing/ControlDependenceGraph.C
  staticInterproceduralSlicing/DependenceGraph.C
  staticInterproceduralSlicing/EDefUse.C
  staticInterproceduralSlicing/DataDependenceGraph.C
  staticInterproceduralSlicing/SlicingInfo.C
  staticInterproceduralSlicing/CreateSlice.C
  staticInterproceduralSlicing/CreateSliceSet.C
  staticInterproceduralSlicing/SystemDependenceGraph.C
  staticInterproceduralSlicing/DefUseExtension.C
  CFG/CFG_ROSE.C
  pointerAnal/PtrAnalCFG.C
  pointerAnal/PtrAnal.C
  dataflowAnalysis/ReachingDefinitionFacade.C
  bitvectorDataflow/DataFlowAnalysis.C
  bitvectorDataflow/ReachingDefinition.C
  bitvectorDataflow/DefUseChain.C
  VirtualFunctionAnalysis/VirtualFunctionAnalysis.C
  VirtualFunctionAnalysis/PtrAliasAnalysis.C
  VirtualFunctionAnalysis/IntraProcAliasAnalysis.C
  VirtualFunctionAnalysis/IntraProcDataFlowAnalysis.C
  VirtualFunctionAnalysis/InterProcDataFlowAnalysis.C
  staticSingleAssignment/staticSingleAssignmentCalculation.C
  staticSingleAssignment/staticSingleAssignmentQueries.C
  staticSingleAssignment/uniqueNameTraversal.C
  staticSingleAssignment/defsAndUsesTraversal.C
  staticSingleAssignment/reachingDef.C
  staticSingleAssignment/staticSingleAssignmentInterprocedural.C
  EditDistance/EditDistance.C
  EditDistance/TreeEditDistance.C
  ReadWriteSetAnalysis/LocalRWSetGenerator.C
  ReadWriteSetAnalysis/Name2Node.C
  ReadWriteSetAnalysis/ReadWriteSetRecords.C
  ReadWriteSetAnalysis/SageUtils.C
  ReadWriteSetAnalysis/TestabilityGrader.C
  ReadWriteSetAnalysis/VxUtilFuncs.C
)

if(NOT enable-internalFrontendDevelopment)
  # If Windows, use list1 as source files for midend_pa, otherwise, use list2
  if(WIN32)
    add_library(midend_pa OBJECT ${LIST1_FILES})
  else()
    add_library(midend_pa OBJECT ${LIST2_FILES})
  endif()
  add_dependencies(midend_pa rosetta_generated)
endif()

if(WIN32)
  message(STATUS "Detected Windows OS; using no extra include dirs.")
else()
  # On non-Windows: use the directories you need
  message(STATUS "Non-Windows OS detected; setting midend_pa_inc_dirs.")
  set(midend_pa_inc_dirs
      . 
      genericDataflow/
      genericDataflow/analysis
      genericDataflow/arrIndexLabeler
      genericDataflow/cfgUtils
      genericDataflow/lattice
      genericDataflow/simpleAnalyses
      genericDataflow/rwAccessLabeler
      genericDataflow/state
      genericDataflow/variables
      genericDataflow/../CFG
      ssaUnfilteredCfg/
  )
  target_include_directories(midend_pa PRIVATE ${midend_pa_inc_dirs})
endif()
