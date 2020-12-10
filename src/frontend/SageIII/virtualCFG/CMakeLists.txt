set(virtualCFG_SRC memberFunctions.C)

if(NOT enable-internalFrontendDevelopment)
  list(APPEND virtualCFG_SRC
    virtualCFG.C cfgToDot.C memberFunctions.C staticCFG.C customFilteredCFG.C
    interproceduralCFG.C virtualBinCFG.C)
endif()

add_library(virtualCFG OBJECT ${virtualCFG_SRC})
add_dependencies(virtualCFG rosetta_generated)


########### install files ###############
install(
  FILES virtualCFG.h virtualBinCFG.h staticCFG.h cfgToDot.h filteredCFG.h
        filteredCFGImpl.h customFilteredCFG.h interproceduralCFG.h
  DESTINATION ${INCLUDE_INSTALL_DIR})
