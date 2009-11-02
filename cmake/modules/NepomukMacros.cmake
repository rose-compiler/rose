# This file contains the following macros:
#
# NEPOMUK_GENERATE_FROM_ONTOLOGY
# Parameters:
#   ontofile     - Path to the NRL ontology defining the resources to be generated.
#   targetdir    - Folder to which the generated sources should be written.
#   out_headers  - Variable which will be filled with the names of all generated headers.
#   out_sources  - Variable which will be filled with the names of all generated sources.
#   out_includes - Variable which will be filled with complete include statements of all 
#                  generated resource classes.
#
# In addition to the parameters an arbitrary number of template filenames can be set as arguments
#
# In case of success NEPOMUK_RESOURCES_GENERATED is true, otherwise false

# (C) 2007 Sebastian Trueg <trueg@kde.org>


macro(NEPOMUK_GENERATE_FROM_ONTOLOGY ontofile targetdir out_headers out_sources out_includes)

  # init
  set(NEPOMUK_RESOURCES_GENERATED false)

  find_program(RCGEN nepomuk-rcgen PATHS ${KDE4_BIN_INSTALL_DIR} ${BIN_INSTALL_DIR} NO_DEFAULT_PATH)

  if(NOT RCGEN)

    message(STATUS "Failed to find the Nepomuk source generator" )

  else(NOT RCGEN)

    FILE(TO_NATIVE_PATH ${RCGEN} RCGEN)

    execute_process(
      COMMAND ${RCGEN} --listheaders --prefix ${targetdir}/ --ontologies ${ontofile}
      OUTPUT_VARIABLE ${out_headers}
      RESULT_VARIABLE rcgen_result
      )

    # If the first call succeeds it is very very likely that the rest will, too
    if(${rcgen_result} EQUAL 0)

      execute_process(
        COMMAND ${RCGEN} --listsources --prefix ${targetdir}/ --ontologies ${ontofile}
        OUTPUT_VARIABLE ${out_sources}
        )
      
      execute_process(
        COMMAND ${RCGEN} --listincludes --ontologies ${ontofile}
        OUTPUT_VARIABLE ${out_includes}
        )

      execute_process(
        COMMAND ${RCGEN} --writeall --templates ${ARGN} --target ${targetdir}/ --ontologies ${ontofile}
        )

      set(NEPOMUK_RESOURCES_GENERATED true)

    else(${rcgen_result} EQUAL 0)

      message(STATUS "Failed to generate Nepomuk resource classes.")

    endif(${rcgen_result} EQUAL 0)

  endif(NOT RCGEN)
  
endmacro(NEPOMUK_GENERATE_FROM_ONTOLOGY)
