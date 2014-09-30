set(virtualCFG_SRC memberFunctions.C)

if(NOT enable-internalFrontendDevelopment)
  list(APPEND virtualCFG_SRC
    virtualCFG.C cfgToDot.C memberFunctions.C staticCFG.C customFilteredCFG.C
    interproceduralCFG.C)
endif()

if(enable-binary-analysis)
  list(APPEND virtualCFG_SRC virtualBinCFG.C)
endif()

add_library(virtualCFG OBJECT ${virtualCFG_SRC})


########### install files ###############
install(
  FILES virtualCFG.h virtualBinCFG.h staticCFG.h cfgToDot.h filteredCFG.h
        filteredCFGImpl.h customFilteredCFG.h interproceduralCFG.h
  DESTINATION ${INCLUDE_INSTALL_DIR})
