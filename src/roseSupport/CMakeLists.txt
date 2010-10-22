
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

### The stringify.h and stringify.C files have prototypes and definitions for functions that convert enum constants
### to strings. The add_custom_target is necessary because the add_custom_command only applies to the current directory
### (this CMakeList.txt file), but we use these two files when building librose in a higher directory.
add_custom_command(
     OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/stringify.C ${CMAKE_CURRENT_BINARY_DIR}/stringify.h
     COMMAND perl ${CMAKE_SOURCE_DIR}/scripts/stringify.pl --header --generic --output=${CMAKE_CURRENT_BINARY_DIR}/stringify.C ${CMAKE_SOURCE_DIR}/src ${CMAKE_BINARY_DIR}/src/frontend/SageIII
     #FIXME: This should also depend on all the files that contain enums, but I'm not sure how to do that. [RPM 2010-10-15]
     DEPENDS ${CMAKE_SOURCE_DIR}/scripts/stringify.pl
     )
add_custom_target(
     generate_stringify
     DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/stringify.C ${CMAKE_CURRENT_BINARY_DIR}/stringify.h
     )



########### next target ###############


#add_library( rose_support ${rose_support_src}  )
#add_dependencies( rose_support ROSETTA  )
########### install files ###############

install(FILES  transformationSupport.h     optionDeclaration.h     sourceLocationInheritedAttribute.h     roseTranslators.h     utility_functions.h DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
## To keep compile commands tidy, please ONLY put required include flags here.
#INCLUDES = $(ROSE_INCLUDES)
#
### Don't use the repository in ../src
#CXX_TEMPLATE_REPOSITORY_PATH = .
#
#libroseSupportSources = \
#     transformationSupport.C \
#     optionDeclaration.C \
#     sourceLocationInheritedAttribute.C \
#     roseTranslators.C \
#     utility_functions.C \
#     memoryUsage.C
#
## DQ (10/11/2007): This used to be part of the template instationation mechanism, but it was 
## based on nm and was not robust.  Instead we instantiate all templates and figure out which 
## are truely required.
## templateSupport.C
#
## lib_LIBRARIES = libroseSupport.a
## libroseSupport_a_SOURCES  = $(libroseSupportSources)
#
#noinst_LTLIBRARIES = libroseSupport.la
#
#libroseSupport_la_SOURCES = $(libroseSupportSources)
#
#include_HEADERS = \
#   transformationSupport.h \
#   optionDeclaration.h \
#   sourceLocationInheritedAttribute.h \
#   roseTranslators.h \
#   utility_functions.h
#
## DQ (10/11/2007): This used to be part of the template instationation mechanism, but it was 
## based on nm and was not robust.  Instead we instantiate all templates and figure out which 
## are truely required.
## templateSupport.h
#
#clean-local:
#	rm -rf Templates.DB ii_files ti_files
#
#distclean-local:
#	rm -rf Templates.DB
#
#
#
#
#
#
#
#
