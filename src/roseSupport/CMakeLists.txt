########### build library ###############
add_definitions(-DROSE_DLL_EXPORTS)
add_library(roseSupport OBJECT
  transformationSupport.C optionDeclaration.C Combinatorics.C
  sourceLocationInheritedAttribute.C rangemap.C roseTranslators.C
  utility_functions.C memoryUsage.C IncludeDirective.C
  stringify.C)
add_dependencies(roseSupport rosetta_generated)

# Add include directories for optional dependencies used in utility_functions.C
# Note: roseSupport is an OBJECT library, so it doesn't automatically inherit
# transitive dependencies from ROSE_DLL. We use rose_object_library_dependencies
# for common dependencies, and add specific ones here.
target_link_libraries(roseSupport PRIVATE rose_object_library_dependencies)

# Additional dependencies specific to utility_functions.C
if(DWARF_FOUND)
  target_include_directories(roseSupport PRIVATE ${DWARF_INCLUDE_DIRS})
endif()
if(PQXX_FOUND)
  target_include_directories(roseSupport PRIVATE ${PQXX_INCLUDE_DIR})
endif()
if(SQLITE3_FOUND)
  target_include_directories(roseSupport PRIVATE ${SQLITE3_INCLUDE_DIR})
endif()

########### install files ###############

install(FILES  transformationSupport.h
               optionDeclaration.h
               sourceLocationInheritedAttribute.h
               rangemap.h
               roseTranslators.h
               utility_functions.h
               IncludeDirective.h
               callbacks.h
               utility_functionsImpl.C
               stringify.h
	       Combinatorics.h
	DESTINATION ${INCLUDE_INSTALL_DIR}
  )
