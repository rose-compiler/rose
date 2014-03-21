install(FILES
    analysis/analysis.h analysis/analysisCommon.h analysis/dataflow.h
    analysis/partitionedAnalysis.h AnalysisDebuggingUtils.h
    arrIndexLabeler/arrIndexLabeler.h cfgUtils/baseCFGIterator.h
    cfgUtils/CallGraphTraverse.h cfgUtils/CFGRewrite.h
    cfgUtils/cfgUtils.h cfgUtils/DataflowCFG.h
    cfgUtils/VirtualCFGIterator.h genericDataflowCommon.h genUID.h
    lattice/affineInequality.h lattice/ConstrGraph.h lattice/lattice.h
    lattice/latticeFull.h rwAccessLabeler/rwAccessLabeler.h
    simpleAnalyses/ConstrGraphAnalysis.h simpleAnalyses/divAnalysis.h
    simpleAnalyses/dominatorAnalysis.h simpleAnalyses/liveDeadVarAnalysis.h
    simpleAnalyses/nodeConstAnalysis.h simpleAnalyses/placeUIDs.h
    simpleAnalyses/printAnalysisStates.h simpleAnalyses/saveDotAnalysis.h
    simpleAnalyses/sequenceStructAnalysis.h simpleAnalyses/sgnAnalysis.h
    simpleAnalyses/taintAnalysis.h simpleAnalyses/VariableStateTransfer.h
    state/functionState.h state/LogicalCond.h state/nodeState.h
    variables/variables.h variables/varSets.h
  DESTINATION ${INCLUDE_INSTALL_DIR})
