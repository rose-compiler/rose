if(enable-fortran)
  set(enable-ofp-version "0.8.6" CACHE STRING "version number for OFP")

  ########### install files ###############
  set(jar_file
    ${CMAKE_CURRENT_SOURCE_DIR}/OpenFortranParser-${enable-ofp-version}.jar)

  if(NOT EXISTS ${jar_file})
    message(FATAL_ERROR
      " ${jar_file} does not exist.
      Please specify a different value for enable-ofp-version."
    )
  endif()

install(
  FILES OpenFortranParser-20190822.jar
        ${jar_file}
  DESTINATION ${ROSE_LIB_DIR_NAME})

  # this file also needs to be in the binary directory for
  # related tests to pass
  configure_file(
    ${jar_file}
    ${CMAKE_CURRENT_BINARY_DIR}
    COPYONLY)

  set(files_to_extract
    "fortran/ofp/parser/c/FortranParserAction.h"
    "fortran/ofp/parser/c/jni/c_actions_jni.c"
    "fortran/ofp/parser/c/token.h"
    "fortran/ofp/parser/c/token.c"
    "fortran/ofp/parser/c/ActionEnums.h"
    "fortran/ofp/parser/c/jni/jni_token_funcs.h"
    "fortran/ofp/parser/c/jni/jni_token_funcs.c"
  )

  if (NOT Java_JAR_EXECUTABLE)
    message(FATAL_ERROR "Need jar to extract source files")
  endif()

  if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/ActionEnums.h)
    foreach(file_to_extract ${files_to_extract})
      execute_process(
	COMMAND ${Java_JAR_EXECUTABLE} xf ${jar_file} ${file_to_extract}
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      )
      get_filename_component(filename
	${CMAKE_CURRENT_BINARY_DIR}/${file_to_extract} NAME)
      file(RENAME
	${CMAKE_CURRENT_BINARY_DIR}/${file_to_extract}
	${CMAKE_CURRENT_BINARY_DIR}/${filename})
    endforeach()
    file(REMOVE_RECURSE ${CMAKE_CURRENT_BINARY_DIR}/fortran)
  endif()
endif()
