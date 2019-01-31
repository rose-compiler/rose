add_custom_command(
  OUTPUT license.h
  COMMAND ${CMAKE_SOURCE_DIR}/scripts/to-c-string.pl <${CMAKE_SOURCE_DIR}/LicenseInformation/ROSE_BSD_License.txt >license.h
  DEPENDS ${CMAKE_SOURCE_DIR}/LicenseInformation/ROSE_BSD_License.txt
  COMMENT "Generating license C string...")

########### build library ###############
add_definitions(-DROSE_DLL_EXPORTS)
add_library(roseSupport OBJECT
  transformationSupport.C optionDeclaration.C Combinatorics.C
  sourceLocationInheritedAttribute.C rangemap.C roseTranslators.C
  utility_functions.C memoryUsage.C IncludeDirective.C
  SqlDatabase.C Diagnostics.C stringify.C initialize.C CommandLine.C)
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
	       CommandLine.h
	       Combinatorics.h
	DESTINATION ${INCLUDE_INSTALL_DIR}
  )
