add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/licenseString.h
  COMMAND perl ${CMAKE_SOURCE_DIR}/scripts/to-c-string.pl <${CMAKE_SOURCE_DIR}/LicenseInformation/ROSE_BSD_License.txt >licenseString.h
  DEPENDS ${CMAKE_SOURCE_DIR}/LicenseInformation/ROSE_BSD_License.txt
  COMMENT "Generating license C string...")

add_custom_target(generate_license_header DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/licenseString.h)

########### build library ###############
add_definitions(-DROSE_DLL_EXPORTS)
add_library(roseSupport OBJECT
  transformationSupport.C optionDeclaration.C Combinatorics.C
  sourceLocationInheritedAttribute.C rangemap.C roseTranslators.C
  utility_functions.C memoryUsage.C IncludeDirective.C
  stringify.C)
add_dependencies(roseSupport rosetta_generated generate_license_header)
if(READLINE_FOUND)
  target_include_directories(roseSupport PRIVATE ${READLINE_INCLUDE_DIR})
  target_link_libraries(roseSupport ${READLINE_LIBRARY})
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
               Diagnostics.h
               utility_functionsImpl.C
               stringify.h
	       initialize.h
	       CommandLine.h
	       Combinatorics.h
	       RoseException.h
	       SourceLocation.h
	DESTINATION ${INCLUDE_INSTALL_DIR}
  )
