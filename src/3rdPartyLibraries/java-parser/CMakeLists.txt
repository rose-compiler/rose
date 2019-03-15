if(enable-java)
  # This file also needs to be in the binary dir for related
  # tests to pass
  configure_file(
    ecj-3.8.2.jar
    ${CMAKE_CURRENT_BINARY_DIR}
    COPYONLY)

  # path separator varies by platform
  set(sep ":")
  if(WIN32)
    set(sep ";")
  endif()

  add_custom_command(
    OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/JavaParser.class
    COMMAND ${Java_JAVAC_EXECUTABLE} -cp
	    "${CMAKE_CURRENT_SOURCE_DIR}/ecj-3.8.2.jar${sep}${CMAKE_CURRENT_SOURCE_DIR}${sep}."
	    -d ${CMAKE_CURRENT_BINARY_DIR}
	    ${CMAKE_CURRENT_SOURCE_DIR}/JavaParser.java
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/JavaParser.java
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

  add_custom_command(
    OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/JavaParser.h
    COMMAND ${Java_JAVAH_EXECUTABLE} -jni
	    -classpath
	    "${CMAKE_CURRENT_SOURCE_DIR}/ecj-3.8.2.jar${sep}."
	    -d ${CMAKE_CURRENT_BINARY_DIR} JavaParser
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/JavaParser.class
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

  add_custom_command(
    OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/JavaTraversal.class
    COMMAND ${Java_JAVAC_EXECUTABLE} -cp
		  "${CMAKE_CURRENT_SOURCE_DIR}/ecj-3.8.2.jar${sep}${CMAKE_CURRENT_SOURCE_DIR}${sep}."
	    ${CMAKE_CURRENT_SOURCE_DIR}/JavaTraversal.java
	    -d ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/JavaTraversal.java
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

  add_custom_command(
    OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/JavaTraversal.h
    COMMAND ${Java_JAVAH_EXECUTABLE} -jni
	    -classpath
	    "${CMAKE_CURRENT_SOURCE_DIR}/ecj-3.8.2.jar${sep}."
	    -d ${CMAKE_CURRENT_BINARY_DIR} JavaTraversal
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/JavaTraversal.class
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

  # This is needed when distcheck compiles the code, I don't know why.
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/JavaParserSupport.class
	  COMMAND ${Java_JAVAC_EXECUTABLE} -cp
	    "${CMAKE_CURRENT_SOURCE_DIR}/ecj-3.8.2.jar${sep}${CMAKE_CURRENT_SOURCE_DIR}${sep}."
	    -d ${CMAKE_CURRENT_BINARY_DIR}
	    ${CMAKE_CURRENT_SOURCE_DIR}/JavaParserSupport.java
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/JavaParserSupport.java
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

  # I assume this is needed the same as for the JavaParserSupport.class rule
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ecjASTVisitor.class
	  COMMAND ${Java_JAVAC_EXECUTABLE} -cp
	    "${CMAKE_CURRENT_SOURCE_DIR}/ecj-3.8.2.jar${sep}${CMAKE_CURRENT_SOURCE_DIR}${sep}."
	    -d ${CMAKE_CURRENT_BINARY_DIR}
	    ${CMAKE_CURRENT_SOURCE_DIR}/ExtendedASTVisitor.java
	    ${CMAKE_CURRENT_SOURCE_DIR}/ecjASTVisitor.java
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ecjASTVisitor.java
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

  # DQ (4/16/2011): Added support for Java specific tokens (to support source position information in ROSE).
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/JavaToken.class
	  COMMAND ${Java_JAVAC_EXECUTABLE} -cp
	    "${CMAKE_CURRENT_SOURCE_DIR}/ecj-3.8.2.jar${sep}${CMAKE_CURRENT_SOURCE_DIR}${sep}."
	    -d ${CMAKE_CURRENT_BINARY_DIR}
	    ${CMAKE_CURRENT_SOURCE_DIR}/JavaToken.java
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/JavaToken.java
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

  # DQ (4/16/2011): Added support for Java specific tokens (to support source position information in ROSE).
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/JavaSourcePositionInformation.class
	  COMMAND ${Java_JAVAC_EXECUTABLE} -cp
	    "${CMAKE_CURRENT_SOURCE_DIR}/ecj-3.8.2.jar${sep}${CMAKE_CURRENT_SOURCE_DIR}${sep}."
	    -d ${CMAKE_CURRENT_BINARY_DIR}
	    ${CMAKE_CURRENT_SOURCE_DIR}/JavaSourcePositionInformation.java
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/JavaSourcePositionInformation.java
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

  # charles4: (2/23/2012):
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/JavaSourcePositionInformationFactory.class
	  COMMAND ${Java_JAVAC_EXECUTABLE} -cp
	    "${CMAKE_CURRENT_SOURCE_DIR}/ecj-3.8.2.jar${sep}${CMAKE_CURRENT_SOURCE_DIR}${sep}."
	    -d ${CMAKE_CURRENT_BINARY_DIR}
	    ${CMAKE_CURRENT_SOURCE_DIR}/JavaSourcePositionInformationFactory.java
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/JavaSourcePositionInformationFactory.java
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

  add_custom_target(extract_rose_required_ecj_source_files
    DEPENDS
      ${CMAKE_CURRENT_BINARY_DIR}/JavaParserSupport.class
      ${CMAKE_CURRENT_BINARY_DIR}/ecjASTVisitor.class
      ${CMAKE_CURRENT_BINARY_DIR}/JavaToken.class
      ${CMAKE_CURRENT_BINARY_DIR}/JavaSourcePositionInformation.class
      ${CMAKE_CURRENT_BINARY_DIR}/JavaSourcePositionInformationFactory.class
      ${CMAKE_CURRENT_BINARY_DIR}/JavaParser.class
      ${CMAKE_CURRENT_BINARY_DIR}/JavaParser.h
      ${CMAKE_CURRENT_BINARY_DIR}/JavaTraversal.class
      ${CMAKE_CURRENT_BINARY_DIR}/JavaTraversal.h)

  ########### install files ###############

  install(
    FILES
      ecj-3.8.2.jar
      ${CMAKE_CURRENT_BINARY_DIR}/ecjASTVisitor.class
      ${CMAKE_CURRENT_BINARY_DIR}/ExtendedASTVisitor.class
      ${CMAKE_CURRENT_BINARY_DIR}/JavaParser.class
      ${CMAKE_CURRENT_BINARY_DIR}/JavaParserSupport.class
      ${CMAKE_CURRENT_BINARY_DIR}/JavaParserSupport$LocalOrAnonymousType.class
      ${CMAKE_CURRENT_BINARY_DIR}/JavaSourcePositionInformation.class
      ${CMAKE_CURRENT_BINARY_DIR}/JavaSourcePositionInformationFactory.class
      ${CMAKE_CURRENT_BINARY_DIR}/JavaToken.class
      ${CMAKE_CURRENT_BINARY_DIR}/JavaTraversal.class
    DESTINATION lib)
endif()
