add_library(util_graphs OBJECT
  DGBaseGraphImpl.C
  SCCAnalysis.C
  TransAnalysis.C
  TypedFusion.C
)

########### install files ###############
install(FILES
  BaseGraphCreate.h DGBaseGraphImpl.h GraphAccess.h GraphDotOutput.h
  GraphGroup.h GraphIO.h GraphScope.h GraphUpdate.h GraphUtils.h
  MultiGraphCreate.h MultiGraphObserve.h SCCAnalysis.h TransAnalysis.h
  TypedFusion.h VirtualGraphCreate.h
  DESTINATION ${INCLUDE_INSTALL_DIR})
