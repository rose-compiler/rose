if(enable-java)
  remove_definitions(-DROSE_DLL_EXPORTS)

  include_directories(
    ${CMAKE_SOURCE_DIR}/src/frontend/ECJ_ROSE_Connection
    ${CMAKE_BINARY_DIR}/src/3rdPartyLibraries/java-parser
    ${JNI_INCLUDE_DIRS})


  ########### next target ###############

  set(JavaTraversal_LIB_SRCS
    JavaParserActionROSE.C JavaTraversal.C token.C jni_token.C
    JavaSourceCodePosition.C jni_JavaSourceCodePosition.C VisitorContext.C
    jni_utils.C java_support.C)

  add_library(JavaTraversal SHARED ${JavaTraversal_LIB_SRCS})
  target_link_libraries(JavaTraversal ROSE_DLL)

  set_target_properties(JavaTraversal PROPERTIES VERSION 1.0.0 SOVERSION 1)
  add_dependencies(JavaTraversal
    rosetta_generated extract_rose_required_ecj_source_files)
  install(TARGETS JavaTraversal DESTINATION ${ROSE_LIB_DIR_NAME})


  ########### next target ###############

  set(libroseJava_SOURCES openJavaParser_main.C ecj.C jserver.C)

  add_library(roseJava OBJECT ${libroseJava_SOURCES})
  add_dependencies(roseJava
    rosetta_generated extract_rose_required_ecj_source_files)
  set_target_properties(roseJava PROPERTIES COMPILE_DEFINITIONS
    ROSE_DLL_EXPORTS=1)


  ########### install files ###############

  install(FILES  ecj.h JavaSourceCodePosition.h java_support.h
		 jni_JavaSourceCodePosition.h jni_token.h jni_utils.h
		 VisitorContext.h
	  DESTINATION ${INCLUDE_INSTALL_DIR})
endif()
