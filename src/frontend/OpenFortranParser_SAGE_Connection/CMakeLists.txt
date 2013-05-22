
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
install(TARGETS fortran_ofp_parser_c_jni_FortranParserActionJNI ${INSTALL_TARGETS_DEFAULT_ARGS})

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
    COMMAND ${CMAKE_COMMAND} -E rename lex.yy.c preproc-fortran-fixed.cc
  )

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
        ofp.h FortranModuleInfo.h FortranParserState.h
        ${fortran_parser_dir}/ActionEnums.h
        ${fortran_parser_dir}/FortranParserAction.h
        ${fortran_parser_dir}/token.h
        ${fortran_parser_dir}/jni_token_funcs.h
  DESTINATION ${INCLUDE_INSTALL_DIR})



#original Makefile.am contents follow:

#include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
#
#INCLUDES = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/OpenFortranParser_SAGE_Connection
#
#EXTRA_DIST = preproc-fortran.ll preproc-fortran-fixed.ll
#DISTCLEANFILES = preproc-fortran.cc preproc-fortran-fixed.cc
#
## DQ (3/14/2009): We have to clear the LDFLAGS variable...  This is not so great.
## we now result the LD_LIBRARY_PATH to be set explicitly.
## LDFLAGS =
#
#if ROSE_USE_OPEN_FORTRAN_PARSER
## This shared library is built in the Open Fortran Parser using the command:
##      gcc -shared -I/usr/apps/java/jdk1.5.0_11/include -I/usr/apps/java/jdk1.5.0_11/include/linux 
##                  -I../  -fPIC ../token.c jni_token_funcs.c c_actions_jni.c ../FortranParserActionPrint.c 
##                  -o ../../../../build/libparser_java_FortranParserActionJNI.so
## To build a similar library using libtool we indicate that we want an installed 
## library (which will then be made into a dynamic shared library).
## lib_LTLIBRARIES = libparser_java_FortranParserActionJNI.la
## lib_LTLIBRARIES = libfortran_ofp_parser_java_FortranParserActionJNI.la
#lib_LTLIBRARIES = libfortran_ofp_parser_c_jni_FortranParserActionJNI.la
#else
#endif
#
## DQ (10/21/2007): build (JJW: either empty or non-empty) lib file so that ROSE/src/Makefile.am need not be conditional on Fortran.
#noinst_LTLIBRARIES = libroseFortran.la
#
#if ROSE_USE_OPEN_FORTRAN_PARSER
#libroseFortran_la_SOURCES      = fortran_support.C openFortranParser_main.C jserver.C ofp.C FortranModuleInfo.C FortranParserState.C preproc-fortran.ll preproc-fortran-fixed.ll
#else
## Compile using a function that represents the API of the Open Fortran Parser
#libroseFortran_la_SOURCES      = dummy_openFortranParser_main.C
#endif
#
#if ROSE_USE_OPEN_FORTRAN_PARSER
## This file (these c_action() function implementations) need to be in a dynamic 
## library so that they can be called by the Java based Open Fortran Parser.
## libparser_java_FortranParserActionJNI_la_SOURCES      = FortranParserActionROSE.C token.c jni_token_funcs.c c_actions_jni.c
## libparser_java_FortranParserActionJNI_la_DEPENDENCIES =
## libfortran_ofp_parser_java_FortranParserActionJNI_la_SOURCES      = FortranParserActionROSE.C token.c jni_token_funcs.c c_actions_jni.c
## libfortran_ofp_parser_java_FortranParserActionJNI_la_DEPENDENCIES =
#libfortran_ofp_parser_c_jni_FortranParserActionJNI_la_SOURCES      = FortranParserActionROSE.C token.c jni_token_funcs.c c_actions_jni.c
#
## To support the Open Fortran Parser we need to build a shared library with a specific name and which will have
## all of the c_action() function implementations.  The name to match is: XXX libparser_java_FortranParserActionJNI.so
## As of Feb 18 2008: the name is changed to libfortran_ofp_parser_c_jni_FortranParserActionJNI.so
#
#include_HEADERS = ActionEnums.h  FortranParserAction.h  token.h rose_fortran_token_maps.h  rose_token_defs.h fortran_support.h jni_token_funcs.h jserver.h ofp.h FortranModuleInfo.h FortranParserState.h
#
## openFortranParserMain: $(top_builddir)/rose_paths.h
#
## DQ (9/24/2007): After updating the Open Fortran Parser, there are a number of files that should be copied over to ROSE.
## Note that only the following are used in ROSE:
##    1) token.c and token.h (definition of OFP's tokens)
##    2) FortranParserAction.h  (header file of C_action functions)
##    3) ActionEnums.h (enum values used in OFP and queried in ROSE)
## All other files copied to ROSE are used as reference within the development.
## OPEN_FORTRAN_PARSER_PATH=/home/dquinlan/ROSE/OpenFortranParser/fortran-parser
#update:
##	cp $(OPEN_FORTRAN_PARSER_PATH)/src/parser/c/token.* $(srcdir)
##	cp $(OPEN_FORTRAN_PARSER_PATH)/src/parser/c/jni/jni_token_funcs.* $(srcdir)
##	cp $(OPEN_FORTRAN_PARSER_PATH)/src/parser/c/jni/c_actions_jni.c $(srcdir)
##	cp $(OPEN_FORTRAN_PARSER_PATH)/src/parser/c/FortranParserAction.h $(srcdir)
##	cp $(OPEN_FORTRAN_PARSER_PATH)/src/parser/c/ActionEnums.h $(srcdir)
##	cp $(OPEN_FORTRAN_PARSER_PATH)/src/parser/c/FortranParserActionNull.c $(srcdir)
#	cp $(OPEN_FORTRAN_PARSER_PATH)/src/fortran/ofp/parser/c/token.* $(srcdir)
#	cp $(OPEN_FORTRAN_PARSER_PATH)/src/fortran/ofp/parser/c/FortranParserAction.h $(srcdir)
#	cp $(OPEN_FORTRAN_PARSER_PATH)/src/fortran/ofp/parser/c/ActionEnums.h $(srcdir)
#	cp $(OPEN_FORTRAN_PARSER_PATH)/src/fortran/ofp/parser/c/FortranParserActionNull.c $(srcdir)
#	cp $(OPEN_FORTRAN_PARSER_PATH)/src/fortran/ofp/parser/c/FortranParserActionPrint.c $(srcdir)
#	cp $(OPEN_FORTRAN_PARSER_PATH)/src/fortran/ofp/parser/c/jni/jni_token_funcs.* $(srcdir)
#	cp $(OPEN_FORTRAN_PARSER_PATH)/src/fortran/ofp/parser/c/jni/c_actions_jni.c $(srcdir)
#	cp $(OPEN_FORTRAN_PARSER_PATH)/src/fortran/ofp/parser/java/IFortranParserAction.java $(srcdir)
#
#endif
#
## EXTRA_DIST = FortranParserActionROSE.C token.c jni_token_funcs.c c_actions_jni.c 
#
