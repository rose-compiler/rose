
include_directories(
  ${CMAKE_SOURCE_DIR}/src/frontend/ECJ_ROSE_Connection
  ${CMAKE_BINARY_DIR}/src/3rdPartyLibraries/java-parser
  ${QT_INCLUDES} ${JNI_INCLUDE_DIRS})


########### next target ###############

set(JavaTraversal_LIB_SRCS
  JavaParserActionROSE.C JavaTraversal.C token.C jni_token.C
  JavaSourceCodePosition.C jni_JavaSourceCodePosition.C VisitorContext.C
  jni_utils.C java_support.C)

add_library(JavaTraversal SHARED ${JavaTraversal_LIB_SRCS})

set_target_properties(JavaTraversal PROPERTIES VERSION 1.0.0 SOVERSION 1)
add_dependencies(JavaTraversal
  rosetta_generated extract_rose_required_ecj_source_files)
install(TARGETS JavaTraversal ${INSTALL_TARGETS_DEFAULT_ARGS})


########### next target ###############

set(libroseJava_SOURCES openJavaParser_main.C ecj.C)

# Handle case of Java language support specified and
# Fortran language support NOT specified.
if(NOT enable-fortran)
  list(APPEND libroseJava_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../OpenFortranParser_SAGE_Connection/jserver.C)
endif()

add_library(roseJava ${libroseJava_SOURCES})
target_link_libraries(roseJava JavaTraversal)


########### install files ###############

#install(FILES  DESTINATION ${INCLUDE_INSTALL_DIR})

