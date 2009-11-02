# - Try to find SQLITE3
# Once done this will define
#
#  SQLITE3_FOUND - system has SQLITE3
#  SQLITE3_INCLUDE_DIR - the SQLITE3 include directory
#  SQLITE3_LIBRARIES - Link these to use SQLITE3
#  SQLITE3_DEFINITIONS - Compiler switches required for using SQLITE3
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#


if ( SQLITE3_INCLUDE_DIR AND SQLITE3_LIBRARIES )
   # in cache already
   SET(SQLITE3_FIND_QUIETLY TRUE)
endif ( SQLITE3_INCLUDE_DIR AND SQLITE3_LIBRARIES )

# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls
if( NOT WIN32 )
  INCLUDE(FindPkgConfig)

  pkg_check_modules(SQLITE3 REQUIRED sqlite3 )

endif( NOT WIN32 )

#FIND_PATH(SQLITE3_INCLUDE_DIR NAMES sqlite3.h
#  PATHS
#  ${_SQLITE3IncDir}
#)

#FIND_LIBRARY(SQLITE3_LIBRARIES NAMES sqlite3
#  PATHS
#  ${_SQLITE3LinkDir}
#)

#include(FindPackageHandleStandardArgs)
#FIND_PACKAGE_HANDLE_STANDARD_ARGS(SQLITE3 DEFAULT_MSG SQLITE3_INCLUDE_DIR SQLITE3_LIBRARIES )

# show the SQLITE3_INCLUDE_DIR and SQLITE3_LIBRARIES variables only in the advanced view
MARK_AS_ADVANCED(SQLITE3_INCLUDE_DIR SQLITE3_LIBRARIES )

