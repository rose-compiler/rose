
include_directories(
  ${CMAKE_SOURCE_DIR}/src/frontend/ECJ_ROSE_Connection
  ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}
  ${JNI_INCLUDE_DIRS})


########### next target ###############


set(libJavaTraversal_LIB_SRCS
  openJavaParser_main.C 
  ecj.C)

add_library(libJavaTraversal SHARED ${libJavaTraversal_LIB_SRCS})
target_link_libraries(libJavaTraversal ${KDE4_KDECORE_LIBS})

set_target_properties(libJavaTraversal PROPERTIES VERSION 1.0.0 SOVERSION 1)
add_dependencies(libJavaTraversal rosetta_generated)
install(TARGETS libJavaTraversal ${INSTALL_TARGETS_DEFAULT_ARGS})


########### next target ###############

set(libroseJava_SOURCES openJavaParser_main.C ecj.C)

# Handle case of Java language support specified and
# Fortran language support NOT specified.
if(NOT enable-fortran)
  list(APPEND libroseJava_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../OpenFortranParser_SAGE_Connection/jserver.C)
endif()

add_library(roseJava ${libroseJava_SOURCES})
add_dependencies(roseJava rosetta_generated)


########### install files ###############

#install(FILES  DESTINATION ${INCLUDE_INSTALL_DIR})



