################################################################################
### The stringify.h and stringify.C files have prototypes and definitions for
### functions that convert enum constants to strings. The add_custom_target is
### necessary because the add_custom_command only applies to the current
### directory (this CMakeList.txt file), but we use these two files when
### building librose in another directory.

find_package(Perl)
if(NOT PERL_FOUND)
  message(FATAL_ERROR "Could not find perl.  Perl is needed to generate stringify.C")
endif()

add_custom_command(
     OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/stringify.C
             ${CMAKE_CURRENT_BINARY_DIR}/stringify.h
     COMMAND ${PERL_EXECUTABLE} ${CMAKE_SOURCE_DIR}/scripts/stringify.pl --header --generic --output=${CMAKE_CURRENT_BINARY_DIR}/stringify.C ${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src/frontend/SageIII
     #FIXME: This should also depend on all the files that contain enums, but I'm not sure how to do that. [RPM 2010-10-15]
     DEPENDS ${CMAKE_SOURCE_DIR}/scripts/stringify.pl
     )
add_custom_target(
     generate_stringify
     DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/stringify.C
             ${CMAKE_CURRENT_BINARY_DIR}/stringify.h
     )
add_dependencies(generate_stringify rosetta_generated)

# Tell cmake that these files are generated (see src/util/CMakeLists.txt).
SET_SOURCE_FILES_PROPERTIES(
     ${CMAKE_CURRENT_BINARY_DIR}/stringify.C
     ${CMAKE_CURRENT_BINARY_DIR}/stringify.h
     PROPERTIES GENERATED 1)


########### build library ###############

add_library(roseSupport OBJECT
  transformationSupport.C optionDeclaration.C
  sourceLocationInheritedAttribute.C rangemap.C roseTranslators.C
  utility_functions.C memoryUsage.C threadSupport.C IncludeDirective.C
  SqlDatabase.C Diagnostics.C
  ${CMAKE_CURRENT_BINARY_DIR}/stringify.C)
add_dependencies(roseSupport rosetta_generated generate_stringify)

########### install files ###############

install(FILES  transformationSupport.h
               optionDeclaration.h
               sourceLocationInheritedAttribute.h
               rangemap.h
               roseTranslators.h
               utility_functions.h
               threadSupport.h
               IncludeDirective.h
               callbacks.h
               Diagnostics.h
               SqlDatabase.h
               utility_functionsImpl.C
               ${CMAKE_CURRENT_BINARY_DIR}/stringify.h
	DESTINATION ${INCLUDE_INSTALL_DIR}
  )
