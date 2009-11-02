# - Try to find BlueZ
# Once done this will define
#
#  BLUEZ_FOUND - system has BlueZ
#  BLUEZ_INCLUDE_DIR - the BlueZ include directory
#  BLUEZ_LIBRARIES - Link these to use BlueZ
#  BLUEZ_DEFINITIONS - Compiler switches required for using BlueZ
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#

# Copyright (c) 2008, Daniel Gollub, <dgollub@suse.de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


if ( BLUEZ_INCLUDE_DIR AND BLUEZ_LIBRARIES )
   # in cache already
   SET(BlueZ_FIND_QUIETLY TRUE)
endif ( BLUEZ_INCLUDE_DIR AND BLUEZ_LIBRARIES )

# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls
if( NOT WIN32 )
  find_package(PkgConfig)

  pkg_check_modules(PC_BLUEZ bluez)

  set(BLUEZ_DEFINITIONS ${PC_BLUEZ_CFLAGS_OTHER})
endif( NOT WIN32 )

FIND_PATH(BLUEZ_INCLUDE_DIR NAMES bluetooth/bluetooth.h
  PATHS
  ${PC_BLUEZ_INCLUDEDIR}
  ${PC_BLUEZ_INCLUDE_DIRS}
  /usr/X11/include
)

FIND_LIBRARY(BLUEZ_LIBRARIES NAMES bluetooth
  PATHS
  ${PC_BLUEZ_LIBDIR}
  ${PC_BLUEZ_LIBRARY_DIRS}
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(BlueZ DEFAULT_MSG BLUEZ_INCLUDE_DIR BLUEZ_LIBRARIES )

# show the BLUEZ_INCLUDE_DIR and BLUEZ_LIBRARIES variables only in the advanced view
MARK_AS_ADVANCED(BLUEZ_INCLUDE_DIR BLUEZ_LIBRARIES )

