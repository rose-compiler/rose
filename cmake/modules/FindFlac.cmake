# - Try to find Flac, the Free Lossless Audio Codec
# Once done this will define
#
#  FLAC_FOUND - system has Flac
#  FLAC_INCLUDE_DIR - the Flac include directory
#  FLAC_LIBRARIES - Link these to use Flac
#  FLAC_OGGFLAC_LIBRARIES - Link these to use OggFlac
#
# No version checking is done - use FLAC_API_VERSION_CURRENT to
# conditionally compile version-dependent code

# Copyright (c) 2006, Laurent Montel, <montel@kde.org>
# Copyright (c) 2006, Alexander Neundorf, <neundorf@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if(FLAC_INCLUDE_DIR AND FLAC_LIBRARIES)
    # Already in cache, be silent
    set(Flac_FIND_QUIETLY TRUE)	
endif(FLAC_INCLUDE_DIR AND FLAC_LIBRARIES)

FIND_PATH(FLAC_INCLUDE_DIR FLAC/metadata.h)

FIND_LIBRARY(FLAC_LIBRARIES NAMES FLAC )

FIND_LIBRARY(FLAC_OGG_LIBRARY NAMES OggFLAC)



IF(FLAC_INCLUDE_DIR AND FLAC_LIBRARIES)
   SET(FLAC_FOUND TRUE)
   IF (FLAC_OGG_LIBRARY)
      SET(FLAC_OGGFLAC_LIBRARIES ${FLAC_OGG_LIBRARY} ${FLAC_LIBRARIES})
   ENDIF (FLAC_OGG_LIBRARY)
ELSE(FLAC_INCLUDE_DIR AND FLAC_LIBRARIES)
   SET(FLAC_FOUND FALSE)
ENDIF(FLAC_INCLUDE_DIR AND FLAC_LIBRARIES)

IF(FLAC_FOUND)
   IF(NOT Flac_FIND_QUIETLY)
      MESSAGE(STATUS "Found Flac: ${FLAC_LIBRARIES}")
   ENDIF(NOT Flac_FIND_QUIETLY)
ELSE(FLAC_FOUND)
   IF(Flac_FIND_REQUIRED)
      MESSAGE(FATAL_ERROR "Could not find Flac")
   ENDIF(Flac_FIND_REQUIRED)
   IF(NOT Flac_FIND_QUIETLY)
      MESSAGE(STATUS "Could not find Flac")
   ENDIF(NOT Flac_FIND_QUIETLY)
ENDIF(FLAC_FOUND)

# show the FLAC_INCLUDE_DIR and FLAC_LIBRARIES variables only in the advanced view
MARK_AS_ADVANCED(FLAC_INCLUDE_DIR FLAC_LIBRARIES FLAC_OGG_LIBRARY)

