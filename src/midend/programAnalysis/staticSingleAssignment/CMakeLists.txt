
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_library(SSA OBJECT
  staticSingleAssignmentCalculation.C staticSingleAssignmentQueries.C
  uniqueNameTraversal.C defsAndUsesTraversal.C reachingDef.C
  staticSingleAssignmentInterprocedural.C)
add_dependencies(SSA rosetta_generated)
