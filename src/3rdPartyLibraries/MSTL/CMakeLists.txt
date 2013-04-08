set(mstl_SRCS DOTGraphInterface.C)

if (NOT enable-internalFrontendDevelopment)
  list(APPEND mstl_SRCS PDFGeneration.C)
endif()

add_library(mstl OBJECT ${mstl_SRCS}) 
add_dependencies(mstl rosetta_generated)
