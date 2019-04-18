add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/license.h
  COMMAND perl ${CMAKE_SOURCE_DIR}/scripts/to-c-string.pl <${CMAKE_SOURCE_DIR}/LicenseInformation/ROSE_BSD_License.txt >license.h
  DEPENDS ${CMAKE_SOURCE_DIR}/LicenseInformation/ROSE_BSD_License.txt
  COMMENT "Generating license C string...")

add_custom_target(generate_license_header DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/license.h)

########### build library ###############
add_definitions(-DROSE_DLL_EXPORTS)
add_library(roseSupport OBJECT
  transformationSupport.C optionDeclaration.C Combinatorics.C
  sourceLocationInheritedAttribute.C rangemap.C roseTranslators.C
  utility_functions.C memoryUsage.C IncludeDirective.C
  SqlDatabase.C Diagnostics.C stringify.C initialize.C CommandLine.C)
add_dependencies(roseSupport rosetta_generated generate_license_header)

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
	       RoseException.h
	DESTINATION ${INCLUDE_INSTALL_DIR}
  )
