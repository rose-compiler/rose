set(mstl_SRC DOTGraphInterface.C)
if (NOT enable-internalFrontendDevelopment)
  list(APPEND mstl_SRC PDFGeneration.C)
endif()
add_library(mstl OBJECT ${mstl_SRC})
add_dependencies(mstl rosetta_generated)


install(
  FILES
    DOTGeneration.h DOTGenerationImpl.h DOTGraphInterface.h
    DOTRepresentation.h DOTRepresentationImpl.h DOTSubgraphRepresentation.h
    DOTSubgraphRepresentationImpl.h PDFGeneration.h Translator.h TreeTraversal.h
  DESTINATION ${INCLUDE_INSTALL_DIR})
