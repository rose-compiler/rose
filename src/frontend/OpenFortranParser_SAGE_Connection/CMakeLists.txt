if(enable-fortran)
  include_directories(${CMAKE_SOURCE_DIR}/src/frontend/OpenFortranParser_SAGE_Connection ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


  ########### next target ###############

  set(fortran_parser_dir
    ${CMAKE_BINARY_DIR}/src/3rdPartyLibraries/fortran-parser)
  include_directories(${fortran_parser_dir} ${JNI_INCLUDE_DIRS})

  set(fortran_ofp_parser_c_jni_FortranParserActionJNI_LIB_SRCS
    FortranParserActionROSE.C
    ${fortran_parser_dir}/token.c
    ${fortran_parser_dir}/jni_token_funcs.c
    ${fortran_parser_dir}/c_actions_jni.c)

  add_library(fortran_ofp_parser_c_jni_FortranParserActionJNI SHARED
    ${fortran_ofp_parser_c_jni_FortranParserActionJNI_LIB_SRCS})
  target_link_libraries(fortran_ofp_parser_c_jni_FortranParserActionJNI
    ${KDE4_KDECORE_LIBS})
  add_dependencies(fortran_ofp_parser_c_jni_FortranParserActionJNI
    rosetta_generated)

  set_target_properties(fortran_ofp_parser_c_jni_FortranParserActionJNI PROPERTIES VERSION 1.0.0 SOVERSION 1)
  install(TARGETS fortran_ofp_parser_c_jni_FortranParserActionJNI DESTINATION ${ROSE_LIB_DIR_NAME})

  if(enable-fortran)
    # Using FLEX_TARGET() would be cleaner, but the autoconf buildsystem
    # requires outfile="lex.yy.c" in the .ll files.
    #
    # FLEX_TARGET(Preproc preproc-fortran.ll
    #   ${CMAKE_CURRENT_BINARY_DIR}/preproc-fortran.cc)
    # FLEX_TARGET(PreprocFixed preproc-fortran-fixed.ll
    #   ${CMAKE_CURRENT_BINARY_DIR}/preproc-fortran-fixed.cc)
    add_custom_command(
      OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/preproc-fortran.cc
	     ${CMAKE_CURRENT_BINARY_DIR}/preproc-fortran-fixed.cc
      COMMAND ${FLEX_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/preproc-fortran.ll
      COMMAND ${CMAKE_COMMAND} -E rename lex.yy.c preproc-fortran.cc
      COMMAND ${FLEX_EXECUTABLE}
	${CMAKE_CURRENT_SOURCE_DIR}/preproc-fortran-fixed.ll
      COMMAND ${CMAKE_COMMAND} -E rename lex.yy.c preproc-fortran-fixed.cc)

  set(libroseFortran_SOURCES
    fortran_support.C fortran_error_handler.C openFortranParser_main.C
    jserver.C ofp.C FortranModuleInfo.C FortranParserState.C
    preproc-fortran.cc preproc-fortran-fixed.cc)
  else()
    # Compile using a function that represents the API of the Open Fortran Parser
    set(libroseFortran_SOURCES dummy_openFortranParser_main.C)
  endif()

  add_library(roseFortran ${libroseFortran_SOURCES})
  target_link_libraries(roseFortran ${JAVA_JVM_LIBRARY})
  add_dependencies(roseFortran rosetta_generated)
  ########### next target ###############


  ########### install files ###############

  install(
    FILES rose_fortran_token_maps.h  rose_token_defs.h fortran_support.h jserver.h
      ofp.h FortranModuleInfo.h FortranParserState.h fortran_error_handler.h
      ${fortran_parser_dir}/ActionEnums.h
      ${fortran_parser_dir}/FortranParserAction.h
      ${fortran_parser_dir}/token.h
      ${fortran_parser_dir}/jni_token_funcs.h
    DESTINATION ${INCLUDE_INSTALL_DIR})
endif()
