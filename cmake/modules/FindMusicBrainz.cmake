# Module to find the musicbrainz library
#
# It defines
#  MUSICBRAINZ_INCLUDE_DIR - the include dir 
#  MUSICBRAINZ_LIBRARIES - the required libraries
#  MUSICBRAINZ_FOUND - true if both of the above have been found

# Copyright (c) 2006,2007 Laurent Montel, <montel@kde.org>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if(MUSICBRAINZ_INCLUDE_DIR AND MUSICBRAINZ_LIBRARIES)
   set(MUSICBRAINZ_FIND_QUIETLY TRUE)
endif(MUSICBRAINZ_INCLUDE_DIR AND MUSICBRAINZ_LIBRARIES)

FIND_PATH(MUSICBRAINZ_INCLUDE_DIR musicbrainz/musicbrainz.h)

FIND_LIBRARY( MUSICBRAINZ_LIBRARIES NAMES musicbrainz)

# We need version 2, version 3 is source-incompatible
FIND_PATH(MUSICBRAINZ3_INCLUDE_DIR musicbrainz3/musicbrainz.h)
if(MUSICBRAINZ3_INCLUDE_DIR AND NOT MUSICBRAINZ_INCLUDE_DIR)
   message(STATUS "\nIncompatible version 3.x of the musicbrainz includes detected. Version 2.x is needed.\n")
endif(MUSICBRAINZ3_INCLUDE_DIR AND NOT MUSICBRAINZ_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args( MusicBrainz DEFAULT_MSG
                                   MUSICBRAINZ_INCLUDE_DIR MUSICBRAINZ_LIBRARIES)

MARK_AS_ADVANCED(MUSICBRAINZ_INCLUDE_DIR MUSICBRAINZ_LIBRARIES)

