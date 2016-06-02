########### build library ###############
add_definitions(-DROSE_DLL_EXPORTS)
add_library(roseSupport OBJECT
  transformationSupport.C optionDeclaration.C
  sourceLocationInheritedAttribute.C rangemap.C roseTranslators.C
  utility_functions.C memoryUsage.C IncludeDirective.C
  SqlDatabase.C Diagnostics.C stringify.C initialize.C)
add_dependencies(roseSupport rosetta_generated)

########### install files ###############

install(FILES  transformationSupport.h
               optionDeclaration.h
               sourceLocationInheritedAttribute.h
               rangemap.h
               roseTranslators.h
               utility_functions.h
               IncludeDirective.h
               callbacks.h
               Diagnostics.h
               SqlDatabase.h
               utility_functionsImpl.C
               stringify.h
	       initialize.h
	DESTINATION ${INCLUDE_INSTALL_DIR}
  )
