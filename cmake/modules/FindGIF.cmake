# - Try to find GIF
# Once done this will define
#
#  GIF_FOUND - system has GIF
#  GIF_INCLUDE_DIR - the GIF include directory
#  GIF_LIBRARIES - Libraries needed to use GIF
#  GIF_DEFINITIONS - Compiler switches required for using GIF

# Copyright (c) 2002 Kitware, Inc., Insight Consortium.  All rights reserved.
# See Copyright.txt or http://www.cmake.org/HTML/Copyright.html for details.

IF (GIF_INCLUDE_DIR AND GIF_LIBRARIES)
  # Already in cache, be silent
  SET(GIF_FIND_QUIETLY TRUE)
ENDIF (GIF_INCLUDE_DIR AND GIF_LIBRARIES)

FIND_PATH(GIF_INCLUDE_DIR gif_lib.h)

SET(POTENTIAL_GIF_LIBS gif libgif ungif libungif giflib)
FIND_LIBRARY(GIF_LIBRARIES NAMES ${POTENTIAL_GIF_LIBS})

IF (GIF_INCLUDE_DIR AND GIF_LIBRARIES)
   SET(CMAKE_REQUIRED_INCLUDES "${GIF_INCLUDE_DIR}" "${CMAKE_REQUIRED_INCLUDES}")
   CHECK_STRUCT_MEMBER(GifFileType UserData gif_lib.h GIF_FOUND)
ENDIF (GIF_INCLUDE_DIR AND GIF_LIBRARIES)

IF (GIF_FOUND)
  IF (NOT GIF_FIND_QUIETLY)
    MESSAGE(STATUS "Found GIF: ${GIF_LIBRARIES}")
  ENDIF (NOT GIF_FIND_QUIETLY)
ELSE (GIF_FOUND)
  IF (GIF_FIND_REQUIRED)
    MESSAGE(STATUS "Looked for GIF libraries named ${POTENTIAL_GIF_LIBS}.")
    MESSAGE(STATUS "Found no acceptable GIF library. This is fatal.")
    MESSAGE(FATAL_ERROR "Could NOT find GIF")
  ENDIF (GIF_FIND_REQUIRED)
ENDIF (GIF_FOUND)

MARK_AS_ADVANCED(GIF_INCLUDE_DIR GIF_LIBRARIES)
