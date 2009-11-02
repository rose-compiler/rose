# Try to find Gettext functionality
# Once done this will define
#
#  GETTEXT_FOUND - system has Gettext
#  GETTEXT_INCLUDE_DIR - Gettext include directory
#  GETTEXT_LIBRARIES - Libraries needed to use Gettext

# TODO: This will enable translations only if Gettext functionality is
# present in libc. Must have more robust system for release, where Gettext
# functionality can also reside in standalone Gettext library, or the one
# embedded within kdelibs (cf. gettext.m4 from Gettext source).
#
# Copyright (c) 2006, Chusslove Illich, <caslav.ilic@gmx.net>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


if (LIBC_HAS_DGETTEXT OR LIBINTL_HAS_DGETTEXT)

  # in cache already
  set(GETTEXT_FOUND TRUE)

else (LIBC_HAS_DGETTEXT OR LIBINTL_HAS_DGETTEXT)

  include(CheckLibraryExists)
  include(CheckFunctionExists)

  find_path(GETTEXT_INCLUDE_DIR libintl.h)
  if(GETTEXT_INCLUDE_DIR)
     set(HAVE_LIBINTL_H 1)
  else(GETTEXT_INCLUDE_DIR)
     set(HAVE_LIBINTL_H 0)
  endif(GETTEXT_INCLUDE_DIR)

  set(GETTEXT_LIBRARIES)

  if (HAVE_LIBINTL_H)
     check_function_exists(dgettext LIBC_HAS_DGETTEXT)
     if (LIBC_HAS_DGETTEXT)
        set(GETTEXT_SOURCE "built in libc")
        set(GETTEXT_FOUND TRUE)
     else (LIBC_HAS_DGETTEXT)
        find_library(LIBINTL_LIBRARY NAMES intl libintl )

        check_library_exists(${LIBINTL_LIBRARY} "dgettext" "" LIBINTL_HAS_DGETTEXT)
        if (LIBINTL_HAS_DGETTEXT)
           set(GETTEXT_SOURCE "in ${LIBINTL_LIBRARY}")
           set(GETTEXT_LIBRARIES ${LIBINTL_LIBRARY})
           set(GETTEXT_FOUND TRUE)
        endif (LIBINTL_HAS_DGETTEXT)
     endif (LIBC_HAS_DGETTEXT)
  endif (HAVE_LIBINTL_H)
  
  if (GETTEXT_FOUND)
     if (NOT Gettext_FIND_QUIETLY)
        message(STATUS "Found Gettext: ${GETTEXT_SOURCE}")
     endif (NOT Gettext_FIND_QUIETLY)
  else (GETTEXT_FOUND)
     if (Gettext_FIND_REQUIRED)
        message(FATAL_ERROR "Could NOT find Gettext")
     endif (Gettext_FIND_REQUIRED)
  endif (GETTEXT_FOUND)
  
  mark_as_advanced(GETTEXT_INCLUDE_DIR GETTEXT_LIBRARIES)

endif (LIBC_HAS_DGETTEXT OR LIBINTL_HAS_DGETTEXT)


# - Find GNU gettext tools
# This module looks for the GNU gettext tools. This module defines the 
# following values:
#  GETTEXT_MSGMERGE_EXECUTABLE: the full path to the msgmerge tool.
#  GETTEXT_MSGFMT_EXECUTABLE: the full path to the msgfmt tool.
#  GETTEXT_FOUND: True if gettext has been found.
#
# Additionally it provides the following macros:
# GETTEXT_CREATE_TRANSLATIONS ( outputFile [ALL] file1 ... fileN )
#    This will create a target "translations" which will convert the 
#    given input po files into the binary output mo file. If the 
#    ALL option is used, the translations will also be created when
#    building the default target.
# GETTEXT_PROCESS_POT( <potfile> [ALL] [INSTALL_DESTINATION <destdir>] <lang1> <lang2> ... )
#     Process the given pot file to mo files.
#     If INSTALL_DESTINATION is given then automatically install rules will be created,
#     the language subdirectory will be taken into account (by default use share/locale/).
#     If ALL is specified, the pot file is processed when building the all traget.
#     It creates a custom target "potfile".
#
# GETTEXT_PROCESS_PO_FILES(<lang> [ALL] [INSTALL_DESTINATION <dir>] <po1> <po2> ...)
#     Process the given po files to mo files for the given language.
#     If INSTALL_DESTINATION is given then automatically install rules will be created,
#     the language subdirectory will be taken into account (by default use share/locale/).
#     If ALL is specified, the po files are processed when building the all traget.
#     It creates a custom target "pofiles".



FIND_PROGRAM(GETTEXT_MSGMERGE_EXECUTABLE msgmerge)

FIND_PROGRAM(GETTEXT_MSGFMT_EXECUTABLE msgfmt)

MACRO(GETTEXT_CREATE_TRANSLATIONS _potFile _firstPoFile)

   SET(_gmoFiles)
   GET_FILENAME_COMPONENT(_potBasename ${_potFile} NAME_WE)
   GET_FILENAME_COMPONENT(_absPotFile ${_potFile} ABSOLUTE)

   SET(_addToAll)
   IF(${_firstPoFile} STREQUAL "ALL")
      SET(_addToAll "ALL")
      SET(_firstPoFile)
   ENDIF(${_firstPoFile} STREQUAL "ALL")

   FOREACH (_currentPoFile ${ARGN})
      GET_FILENAME_COMPONENT(_absFile ${_currentPoFile} ABSOLUTE)
      GET_FILENAME_COMPONENT(_abs_PATH ${_absFile} PATH)
      GET_FILENAME_COMPONENT(_lang ${_absFile} NAME_WE)
      SET(_gmoFile ${CMAKE_CURRENT_BINARY_DIR}/${_lang}.gmo)

      ADD_CUSTOM_COMMAND( 
         OUTPUT ${_gmoFile} 
         COMMAND ${GETTEXT_MSGMERGE_EXECUTABLE} --quiet --update --backup=none -s ${_absFile} ${_absPotFile}
         COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${_gmoFile} ${_absFile}
         DEPENDS ${_absPotFile} ${_absFile} 
      )

      INSTALL(FILES ${_gmoFile} DESTINATION share/locale/${_lang}/LC_MESSAGES RENAME ${_potBasename}.mo) 
      SET(_gmoFiles ${_gmoFiles} ${_gmoFile})

   ENDFOREACH (_currentPoFile )

   ADD_CUSTOM_TARGET(translations ${_addToAll} DEPENDS ${_gmoFiles})

ENDMACRO(GETTEXT_CREATE_TRANSLATIONS )

# GETTEXT_PROCESS_POT( <potfile> [ALL] [INSTALL_DESTINATION <destdir>] <lang1> <lang2> ... )
MACRO(GETTEXT_PROCESS_POT_FILE _potFile)

   SET(_gmoFiles)
   SET(_args ${ARGN})
   SET(_addToAll)
   SET(_installDest)

   IF(_args)
      LIST(GET _args 0 _tmp)
      IF("${_tmp}" STREQUAL "ALL")
         SET(_addToAll ALL)
         LIST(REMOVE_AT _args 0)
      ENDIF("${_tmp}" STREQUAL "ALL")
   ENDIF(_args)

   IF(_args)
      LIST(GET _args 0 _tmp)
      IF("${_tmp}" STREQUAL "INSTALL_DESTINATION")
         LIST(GET _args 1 _installDest )
         LIST(REMOVE_AT _args 0 1)
      ENDIF("${_tmp}" STREQUAL "INSTALL_DESTINATION")
   ENDIF(_args)

   GET_FILENAME_COMPONENT(_potBasename ${_potFile} NAME_WE)
   GET_FILENAME_COMPONENT(_absPotFile ${_potFile} ABSOLUTE)

#    message(STATUS "1 all ${_addToAll} dest ${_installDest} args: ${_args}")

   FOREACH (_lang ${_args})
      SET(_poFile  "${CMAKE_CURRENT_BINARY_DIR}/${_lang}.po")
      SET(_gmoFile "${CMAKE_CURRENT_BINARY_DIR}/${_lang}.gmo")

      ADD_CUSTOM_COMMAND( 
         OUTPUT "${_poFile}" 
         COMMAND ${GETTEXT_MSGMERGE_EXECUTABLE} --quiet --update --backup=none -s ${_poFile} ${_absPotFile}
         DEPENDS ${_absPotFile}
      )

      ADD_CUSTOM_COMMAND( 
         OUTPUT "${_gmoFile}"
         COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${_gmoFile} ${_poFile}
         DEPENDS ${_absPotFile} ${_poFile} 
      )

      IF(_installDest)
         INSTALL(FILES ${_gmoFile} DESTINATION ${_installDest}/${_lang}/LC_MESSAGES RENAME ${_potBasename}.mo) 
      ENDIF(_installDest)
      LIST(APPEND _gmoFiles ${_gmoFile})

   ENDFOREACH (_lang )

   ADD_CUSTOM_TARGET(potfile ${_addToAll} DEPENDS ${_gmoFiles})

ENDMACRO(GETTEXT_PROCESS_POT_FILE)


# GETTEXT_PROCESS_PO_FILES(<lang> [ALL] [INSTALL_DESTINATION <dir>] <po1> <po2> ...)
MACRO(GETTEXT_PROCESS_PO_FILES _lang)
   SET(_gmoFiles)
   SET(_args ${ARGN})
   SET(_addToAll)
   SET(_installDest)

   LIST(GET _args 0 _tmp)
   IF("${_tmp}" STREQUAL "ALL")
      SET(_addToAll ALL)
      LIST(REMOVE_AT _args 0)
   ENDIF("${_tmp}" STREQUAL "ALL")
   
   LIST(GET _args 0 _tmp)
   IF("${_tmp}" STREQUAL "INSTALL_DESTINATION")
      LIST(GET _args 1 _installDest )
      LIST(REMOVE_AT _args 0 1)
   ENDIF("${_tmp}" STREQUAL "INSTALL_DESTINATION")
   
#    message(STATUS "2 all ${_addToAll} dest ${_installDest} args: ${_args}")
   
   FOREACH(_current_PO_FILE ${_args})
      GET_FILENAME_COMPONENT(_basename ${_current_PO_FILE} NAME_WE)
      SET(_gmoFile ${CMAKE_CURRENT_BINARY_DIR}/${_basename}.gmo)
      add_custom_command(OUTPUT ${_gmoFile}
            COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} -o ${_gmoFile} ${_current_PO_FILE}
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            DEPENDS ${_current_PO_FILE}
         )

      IF(_installDest)
         INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${_basename}.gmo DESTINATION ${_installDest}/${_lang}/LC_MESSAGES/ RENAME ${_basename}.mo)
      ENDIF(_installDest)
      LIST(APPEND _gmoFiles ${_gmoFile})
   ENDFOREACH(_current_PO_FILE)
   ADD_CUSTOM_TARGET(pofiles ${_addToAll} DEPENDS ${_gmoFiles})
ENDMACRO(GETTEXT_PROCESS_PO_FILES)


#IF (GETTEXT_MSGMERGE_EXECUTABLE AND GETTEXT_MSGFMT_EXECUTABLE )
#   SET(GETTEXT_FOUND TRUE)
#ELSE (GETTEXT_MSGMERGE_EXECUTABLE AND GETTEXT_MSGFMT_EXECUTABLE )
#   SET(GETTEXT_FOUND FALSE)
#   IF (GetText_REQUIRED)
#      MESSAGE(FATAL_ERROR "GetText not found")
#   ENDIF (GetText_REQUIRED)
#ENDIF (GETTEXT_MSGMERGE_EXECUTABLE AND GETTEXT_MSGFMT_EXECUTABLE )



