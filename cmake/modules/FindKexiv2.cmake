# - Try to find the KExiv2 library
# Once done this will define
#
#  KEXIV2_FOUND - system has libkexiv2
#  KEXIV2_INCLUDE_DIR - the libkexiv2 include directory
#  KEXIV2_LIBRARIES - Link these to use libkexiv2
#  KEXIV2_DEFINITIONS - Compiler switches required for using libkexiv2
#

# Copyright (c) 2008, Gilles Caulier, <caulier.gilles@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if (KEXIV2_INCLUDE_DIR AND KEXIV2_LIBRARIES)

  message(STATUS "Found Kexiv2 library in cache: ${KEXIV2_LIBRARIES}")

  # in cache already
  SET(KEXIV2_FOUND TRUE)

else (KEXIV2_INCLUDE_DIR AND KEXIV2_LIBRARIES)

  message(STATUS "Check Kexiv2 library in local sub-folder...")

  # Check if library is not in local sub-folder

  FIND_FILE(KEXIV2_LOCAL_FOUND libkexiv2/version.h.cmake ${CMAKE_SOURCE_DIR}/libkexiv2 ${CMAKE_SOURCE_DIR}/libs/libkexiv2 NO_DEFAULT_PATH)

  if (KEXIV2_LOCAL_FOUND)

    FIND_FILE(KEXIV2_LOCAL_FOUND_IN_LIBS libkexiv2/version.h.cmake ${CMAKE_SOURCE_DIR}/libs/libkexiv2 NO_DEFAULT_PATH)
    if (KEXIV2_LOCAL_FOUND_IN_LIBS)
      set(KEXIV2_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libs/libkexiv2)
    else (KEXIV2_LOCAL_FOUND_IN_LIBS)
      set(KEXIV2_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libkexiv2)
    endif (KEXIV2_LOCAL_FOUND_IN_LIBS)
    set(KEXIV2_DEFINITIONS "-I${KEXIV2_INCLUDE_DIR}")
    set(KEXIV2_LIBRARIES kexiv2)
    message(STATUS "Found Kexiv2 library in local sub-folder: ${KEXIV2_INCLUDE_DIR}")
    set(KEXIV2_FOUND TRUE)
    MARK_AS_ADVANCED(KEXIV2_INCLUDE_DIR KEXIV2_LIBRARIES)

  else(KEXIV2_LOCAL_FOUND)
    if(NOT WIN32) 
      message(STATUS "Check Kexiv2 library using pkg-config...")

      # use pkg-config to get the directories and then use these values
      # in the FIND_PATH() and FIND_LIBRARY() calls
      INCLUDE(UsePkgConfig)
    
      PKGCONFIG(libkexiv2 _KEXIV2IncDir _KEXIV2LinkDir _KEXIV2LinkFlags _KEXIV2Cflags)
    
      if(_KEXIV2LinkFlags)
        # query pkg-config asking for a libkexiv2 >= 0.2.0
        EXEC_PROGRAM(${PKGCONFIG_EXECUTABLE} ARGS --atleast-version=0.2.0 libkexiv2 RETURN_VALUE _return_VALUE OUTPUT_VARIABLE _pkgconfigDevNull )
        if(_return_VALUE STREQUAL "0")
            message(STATUS "Found libkexiv2 release >= 0.2.0")
            set(KEXIV2_VERSION_GOOD_FOUND TRUE)
        else(_return_VALUE STREQUAL "0")
            message(STATUS "Found libkexiv2 release < 0.2.0, too old")
            set(KEXIV2_VERSION_GOOD_FOUND FALSE)
            set(KEXIV2_FOUND FALSE)
        endif(_return_VALUE STREQUAL "0")
      else(_KEXIV2LinkFlags)
        set(KEXIV2_VERSION_GOOD_FOUND FALSE)
        set(KEXIV2_FOUND FALSE)
      endif(_KEXIV2LinkFlags)
    else(NOT WIN32)
      set(KEXIV2_VERSION_GOOD_FOUND TRUE)
    endif(NOT WIN32)

    if(KEXIV2_VERSION_GOOD_FOUND)
        set(KEXIV2_DEFINITIONS "${_KEXIV2Cflags}")
    
        FIND_PATH(KEXIV2_INCLUDE_DIR libkexiv2/version.h
        ${_KEXIV2IncDir}
        )
    
        FIND_LIBRARY(KEXIV2_LIBRARIES NAMES kexiv2
        PATHS
        ${_KEXIV2LinkDir}
        )
    
        if (KEXIV2_INCLUDE_DIR AND KEXIV2_LIBRARIES)
            set(KEXIV2_FOUND TRUE)
        endif (KEXIV2_INCLUDE_DIR AND KEXIV2_LIBRARIES)
      endif(KEXIV2_VERSION_GOOD_FOUND) 
      if (KEXIV2_FOUND)
          if (NOT Kexiv2_FIND_QUIETLY)
              message(STATUS "Found libkexiv2: ${KEXIV2_LIBRARIES}")
          endif (NOT Kexiv2_FIND_QUIETLY)
      else (KEXIV2_FOUND)
          if (Kexiv2_FIND_REQUIRED)
              if (NOT KEXIV2_INCLUDE_DIR)
                  message(FATAL_ERROR "Could NOT find libkexiv2 header files")
              endif (NOT KEXIV2_INCLUDE_DIR)
              if (NOT KEXIV2_LIBRARIES)
                  message(FATAL_ERROR "Could NOT find libkexiv2 library")
              endif (NOT KEXIV2_LIBRARIES)
          endif (Kexiv2_FIND_REQUIRED)
      endif (KEXIV2_FOUND)
    
    MARK_AS_ADVANCED(KEXIV2_INCLUDE_DIR KEXIV2_LIBRARIES)

  endif(KEXIV2_LOCAL_FOUND)
  
endif (KEXIV2_INCLUDE_DIR AND KEXIV2_LIBRARIES)
