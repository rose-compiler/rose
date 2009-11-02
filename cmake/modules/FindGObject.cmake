# - Try to find GObject
# Once done this will define
#
#  GOBJECT_FOUND - system has GObject
#  GOBJECT_INCLUDE_DIR - the GObject include directory
#  GOBJECT_LIBRARIES - the libraries needed to use GObject
#  GOBJECT_DEFINITIONS - Compiler switches required for using GObject

# Copyright (c) 2006, Tim Beaulen <tbscope@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


IF (GOBJECT_INCLUDE_DIR AND GOBJECT_LIBRARIES)
   # in cache already
   SET(GObject_FIND_QUIETLY TRUE)
ELSE (GOBJECT_INCLUDE_DIR AND GOBJECT_LIBRARIES)
   SET(GObject_FIND_QUIETLY FALSE)
ENDIF (GOBJECT_INCLUDE_DIR AND GOBJECT_LIBRARIES)

IF (NOT WIN32)
   # use pkg-config to get the directories and then use these values
   # in the FIND_PATH() and FIND_LIBRARY() calls
   FIND_PACKAGE(PkgConfig)
   PKG_CHECK_MODULES(PC_GOBJECT gobject-2.0) 
   #MESSAGE(STATUS "DEBUG: GObject include directory = ${GOBJECT_INCLUDE_DIRS}")
   #MESSAGE(STATUS "DEBUG: GObject link directory = ${GOBJECT_LIBRARY_DIRS}")
   #MESSAGE(STATUS "DEBUG: GObject CFlags = ${GOBJECT_CFLAGS}")
   SET(GOBJECT_DEFINITIONS ${PC_GOBJECT_CFLAGS_OTHER})
ENDIF (NOT WIN32)

FIND_PATH(GOBJECT_INCLUDE_DIR gobject.h
   PATHS
   ${PC_GOBJECT_INCLUDEDIR}
   ${PC_GOBJECT_INCLUDE_DIRS}
   PATH_SUFFIXES glib-2.0/gobject/
   )

FIND_LIBRARY(_GObjectLibs NAMES gobject-2.0
   PATHS
   ${PC_GOBJECT_LIBDIR}
   ${PC_GOBJECT_LIBRARY_DIRS}
   )
FIND_LIBRARY(_GModuleLibs NAMES gmodule-2.0
   PATHS
   ${PC_GOBJECT_LIBDIR}
   ${PC_GOBJECT_LIBRARY_DIRS}
   )
FIND_LIBRARY(_GThreadLibs NAMES gthread-2.0
   PATHS
   ${PC_GOBJECT_LIBDIR}
   ${PC_GOBJECT_LIBRARY_DIRS}
   )
FIND_LIBRARY(_GLibs NAMES glib-2.0
   PATHS
   ${PC_GOBJECT_LIBDIR}
   ${PC_GOBJECT_LIBRARY_DIRS}
   )

SET( GOBJECT_LIBRARIES ${_GObjectLibs} ${_GModuleLibs} ${_GThreadLibs} ${_GLibs} )

IF (GOBJECT_INCLUDE_DIR AND GOBJECT_LIBRARIES)
   SET(GOBJECT_FOUND TRUE)
ELSE (GOBJECT_INCLUDE_DIR AND GOBJECT_LIBRARIES)
   SET(GOBJECT_FOUND FALSE)
ENDIF (GOBJECT_INCLUDE_DIR AND GOBJECT_LIBRARIES)

IF (GOBJECT_FOUND)
   IF (NOT GObject_FIND_QUIETLY)
      MESSAGE(STATUS "Found GObject libraries: ${GOBJECT_LIBRARIES}")
      MESSAGE(STATUS "Found GObject includes : ${GOBJECT_INCLUDE_DIR}")
   ENDIF (NOT GObject_FIND_QUIETLY)
ELSE (GOBJECT_FOUND)
    IF (GObject_FIND_REQUIRED)
      MESSAGE(STATUS "Could NOT find GObject")
    ENDIF(GObject_FIND_REQUIRED)
ENDIF (GOBJECT_FOUND)

MARK_AS_ADVANCED(GOBJECT_INCLUDE_DIR _GObjectLibs _GModuleLibs _GThreadLibs _GLibs)
