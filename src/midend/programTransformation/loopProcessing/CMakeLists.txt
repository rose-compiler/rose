add_subdirectory(depInfo)
add_subdirectory(depGraph)
add_subdirectory(computation)
add_subdirectory(outsideInterface)
add_subdirectory(slicing)
add_subdirectory(driver)
add_subdirectory(prepostTransformation)

if (NOT enable-internalFrontendDevelopment)
  add_library( midend_loopproc OBJECT
    prepostTransformation/PrivatizeScalar.C
    prepostTransformation/BreakupStmt.C
    prepostTransformation/LoopUnroll.C
    computation/LoopTreeTransform.C
    computation/LoopTree.C
    computation/LoopTreeDummyNode.C
    computation/LoopTreeShadow.C
    computation/DepCompTransform.C
    computation/LoopTreeDepComp.C
    computation/LoopTreeLocality.C
    slicing/CompSliceDepGraph.C
    slicing/DynamicCompSlice.C
    slicing/CompSliceLocality.C
    slicing/CompSlice.C
    slicing/DependenceHoisting.C
    depGraph/TransDepGraph.C
    depGraph/DepGraphBuild.C
    depGraph/LoopAnalysis.C
    outsideInterface/ArrayRewrite.C
    outsideInterface/ArrayAnnot.C
    outsideInterface/ArrayInterface.C
    driver/CopyArrayAnal.C
    driver/LoopTransformInterface.C
    driver/FusionAnal.C
    driver/NormalizeCPP.C
    driver/InterchangeAnal.C
    driver/LoopTransformOptions.C
    driver/TransformComputation.C
    driver/BlockingAnal.C
    driver/TypedFusionImpl.C
    driver/ParallelizeLoop.C
    depInfo/StmtDepAnal.C
    depInfo/DepInfo.C
    depInfo/DepRel.C
    depInfo/DepInfoSet.C
    depInfo/DomainInfo.C
    depInfo/DepEDD.C
    depInfo/ReuseAnalysis.C
    depInfo/DepInfoAnal.C)
  add_dependencies(midend_loopproc rosetta_generated)
endif()

