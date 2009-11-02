# - Try to find Flex
# Once done this will define
#
#  FLEX_FOUND - system has Flex
#  FLEX_EXECUTABLE - path of the flex executable
#  FLEX_VERSION - the version string, like "2.5.31"


# Copyright (c) 2008, Jakob Petsovits, <jpetso@gmx.at>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

FIND_PROGRAM(FLEX_EXECUTABLE NAMES flex)

INCLUDE(MacroEnsureVersion)

IF(FLEX_EXECUTABLE)
    SET(FLEX_FOUND TRUE)

    EXECUTE_PROCESS(COMMAND ${FLEX_EXECUTABLE} --version
        OUTPUT_VARIABLE _FLEX_VERSION
    )
    string (REGEX MATCH "[0-9]+\\.[0-9]+\\.[0-9]+" FLEX_VERSION "${_FLEX_VERSION}")
ENDIF(FLEX_EXECUTABLE)

FIND_PATH(FLEX_INCLUDE_DIR FlexLexer.h )
FIND_LIBRARY(FLEX_LIBRARY fl)

IF(FLEX_FOUND)
  IF(NOT Flex_FIND_QUIETLY)
    MESSAGE(STATUS "Found Flex: ${FLEX_EXECUTABLE} ${FLEX_VERSION}")
  ENDIF(NOT Flex_FIND_QUIETLY)
ELSE(FLEX_FOUND)
  IF(Flex_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find Flex")
  ENDIF(Flex_FIND_REQUIRED)
ENDIF(FLEX_FOUND)

MARK_AS_ADVANCED(FLEX_INCLUDE_DIR FLEX_LIBRARY FLEX_EXECUTABLE)
