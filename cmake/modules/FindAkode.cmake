# - Try to find the aKode library
# Once done this will define
#
#  AKODE_FOUND - system has the aKode library
#  AKODE_INCLUDE_DIR - the aKode include directory
#  AKODE_LIBRARIES - The libraries needed to use aKode

# Copyright (c) 2006, Pino Toscano, <toscano.pino@tiscali.it>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


IF (AKODE_LIBRARIES AND AKODE_INCLUDE_DIR)

  # in cache already
  SET(AKODE_FOUND TRUE)

ELSE (AKODE_LIBRARIES AND AKODE_INCLUDE_DIR)

  FIND_PROGRAM(AKODECONFIG_EXECUTABLE NAMES akode-config) 

  # if akode-config has been found
  IF (AKODECONFIG_EXECUTABLE)

    EXEC_PROGRAM(${AKODECONFIG_EXECUTABLE} ARGS --libs RETURN_VALUE _return_VALUE OUTPUT_VARIABLE AKODE_LIBRARIES)

    EXEC_PROGRAM(${AKODECONFIG_EXECUTABLE} ARGS --cflags RETURN_VALUE _return_VALUE OUTPUT_VARIABLE AKODE_INCLUDE_DIR)

    IF (AKODE_LIBRARIES AND AKODE_INCLUDE_DIR)
      SET(AKODE_FOUND TRUE)
      STRING(REGEX REPLACE "-I(.+)" "\\1" AKODE_INCLUDE_DIR "${AKODE_INCLUDE_DIR}")
      message(STATUS "Found aKode: ${AKODE_LIBRARIES}")
    ENDIF (AKODE_LIBRARIES AND AKODE_INCLUDE_DIR)

    # ensure that they are cached
    set(AKODE_INCLUDE_DIR ${AKODE_INCLUDE_DIR})
    set(AKODE_LIBRARIES ${AKODE_LIBRARIES})

  ENDIF (AKODECONFIG_EXECUTABLE)
  IF(AKODE_FOUND)
    IF(NOT Akode_FIND_QUIETLY)
      MESSAGE(STATUS "Akode found: ${AKODE_LIBRARIES}")
    ENDIF(NOT Akode_FIND_QUIETLY)
  ELSE(AKODE_FOUND)
  IF(Akode_FIND_REQUIRED)
     MESSAGE(FATAL_ERROR "Could not find Akode")
   ENDIF(Akode_FIND_REQUIRED)
  ENDIF(AKODE_FOUND)

ENDIF (AKODE_LIBRARIES AND AKODE_INCLUDE_DIR)

