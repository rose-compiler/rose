add_subdirectory(abstractHandle)
add_subdirectory(abstractLayer)
add_subdirectory(abstractMemoryObject)
add_subdirectory(astDiagnostics)
add_subdirectory(astProcessing)
add_subdirectory(astMatching)
add_subdirectory(astQuery)
add_subdirectory(astRewriteMechanism)
add_subdirectory(astUtil)
add_subdirectory(binaryAnalyses)
add_subdirectory(programTransformation)
if(NOT enable-internalFrontendDevelopment)
  add_subdirectory(programAnalysis)
endif()


add_library(midend OBJECT
  midend_roseh_pch.cpp
  abstractMemoryObject/memory_object.cpp
  abstractMemoryObject/memory_object_impl.cpp
  astQuery/nodeQueryInheritedAttribute.C
  astQuery/nameQuery.C
  astQuery/astQueryInheritedAttribute.C
  astQuery/numberQueryInheritedAttribute.C
  astQuery/booleanQuery.C
  astQuery/booleanQueryInheritedAttribute.C
  astQuery/numberQuery.C
  astQuery/astQuery.C
  astQuery/nameQueryInheritedAttribute.C
  astQuery/nodeQuery.C
  astSnippet/Snippet.C)

add_dependencies(midend rosetta_generated)
