set(astProcessing_SRC
  AstPDFGeneration.C
  AstNodeVisitMapping.C
  AstTextAttributesHandling.C
  AstDOTGeneration.C
  AstProcessing.C
  AstSimpleProcessing.C
  AstNodePtrs.C
  AstSuccessorsSelectors.C
  AstAttributeMechanism.C
  AstReverseSimpleProcessing.C
  AstClearVisitFlags.C
  AstTraversal.C
  AstCombinedSimpleProcessing.C)

if(NOT WIN32)
  list(APPEND astProcessing_SRC
    AstSharedMemoryParallelSimpleProcessing.C
    plugin.C
    AstRestructure.C)
endif()

add_library(astProcessing OBJECT ${astProcessing_SRC})
add_dependencies(astProcessing rosetta_generated)


########### install files ###############

set(files_to_install
  AstPDFGeneration.h AstNodeVisitMapping.h AstAttributeMechanism.h
  AstTextAttributesHandling.h AstDOTGeneration.h AstProcessing.h
  AstSimpleProcessing.h AstTraverseToRoot.h AstNodePtrs.h
  AstSuccessorsSelectors.h AstReverseProcessing.h
  AstReverseSimpleProcessing.h AstRestructure.h AstClearVisitFlags.h
  AstTraversal.h AstCombinedProcessing.h AstCombinedProcessingImpl.h
  AstCombinedSimpleProcessing.h StackFrameVector.h AstDOTGenerationImpl.C
  graphProcessing.h graphProcessingSgIncGraph.h graphTemplate.h
  AstSharedMemoryParallelProcessing.h AstSharedMemoryParallelProcessingImpl.h
  AstSharedMemoryParallelSimpleProcessing.h
  SgGraphTemplate.h)

if(NOT WIN32)
  #tps commented out AstSharedMemoryParallelProcessing.h for Windows
  list(APPEND files_to_install AstSharedMemoryParallelProcessing.h plugin.h)
endif()

install(FILES ${files_to_install} DESTINATION ${INCLUDE_INSTALL_DIR})
