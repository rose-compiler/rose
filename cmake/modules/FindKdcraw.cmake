# - Try to find the Kdcraw library
# Once done this will define
#
#  KDCRAW_FOUND - system has libkdcraw
#  KDCRAW_INCLUDE_DIR - the libkdcraw include directory
#  KDCRAW_LIBRARIES - Link these to use libkdcraw
#  KDCRAW_DEFINITIONS - Compiler switches required for using libkdcraw
#

# Copyright (c) 2008, Gilles Caulier, <caulier.gilles@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


if (KDCRAW_INCLUDE_DIR AND KDCRAW_LIBRARIES)

  message(STATUS "Found Kdcraw library in cache: ${KDCRAW_LIBRARIES}")

  # in cache already
  SET(KDCRAW_FOUND TRUE)

else (KDCRAW_INCLUDE_DIR AND KDCRAW_LIBRARIES)

  message(STATUS "Check Kdcraw library in local sub-folder...")

  # Check if library is not in local sub-folder
 
  FIND_FILE(KDCRAW_LOCAL_FOUND libkdcraw/version.h ${CMAKE_BINARY_DIR}/libkdcraw ${CMAKE_BINARY_DIR}/libs/libkdcraw NO_DEFAULT_PATH)

  if (KDCRAW_LOCAL_FOUND)

    # Was it found in libkdcraw/ or in libs/libkdcraw?
    FIND_FILE(KDCRAW_LOCAL_FOUND_IN_LIBS libkdcraw/version.h ${CMAKE_BINARY_DIR}/libs/libkdcraw NO_DEFAULT_PATH)
    if (KDCRAW_LOCAL_FOUND_IN_LIBS)
      set(KDCRAW_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libs/libkdcraw)
    else (KDCRAW_LOCAL_FOUND_IN_LIBS)
      set(KDCRAW_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libkdcraw)
    endif (KDCRAW_LOCAL_FOUND_IN_LIBS)
    set(KDCRAW_DEFINITIONS "-I${KDCRAW_INCLUDE_DIR}")
    set(KDCRAW_LIBRARIES kdcraw)
    message(STATUS "Found Kdcraw library in local sub-folder: ${KDCRAW_INCLUDE_DIR}")
    set(KDCRAW_FOUND TRUE)
    MARK_AS_ADVANCED(KDCRAW_INCLUDE_DIR KDCRAW_LIBRARIES)

  else(KDCRAW_LOCAL_FOUND)

    message(STATUS "Check Kdcraw library using pkg-config...")
    if(NOT WIN32)
      # use pkg-config to get the directories and then use these values
      # in the FIND_PATH() and FIND_LIBRARY() calls
      INCLUDE(UsePkgConfig)
    
      PKGCONFIG(libkdcraw _KDCRAWIncDir _KDCRAWLinkDir _KDCRAWLinkFlags _KDCRAWCflags)
    
      if(_KDCRAWLinkFlags)
        # query pkg-config asking for a libkdcraw >= 0.2.0
        EXEC_PROGRAM(${PKGCONFIG_EXECUTABLE} ARGS --atleast-version=0.2.0 libkdcraw RETURN_VALUE _return_VALUE OUTPUT_VARIABLE _pkgconfigDevNull )
        if(_return_VALUE STREQUAL "0")
            message(STATUS "Found libkdcraw release >= 0.2.0")
            set(KDCRAW_VERSION_GOOD_FOUND TRUE)
        else(_return_VALUE STREQUAL "0")
            message(STATUS "Found libkdcaw release < 0.2.0, too old")
            set(KDCRAW_VERSION_GOOD_FOUND FALSE)
            set(KDCRAW_FOUND FALSE)
        endif(_return_VALUE STREQUAL "0")
      else(_KDCRAWLinkFlags)
        set(KDCRAW_VERSION_GOOD_FOUND FALSE)
        set(KDCRAW_FOUND FALSE)
      endif(_KDCRAWLinkFlags)
    ELSE(NOT WIN32)
      set(KDCRAW_VERSION_GOOD_FOUND TRUE)
    ENDif(NOT WIN32)

    if(KDCRAW_VERSION_GOOD_FOUND)
        set(KDCRAW_DEFINITIONS "${_KDCRAWCflags}")
    
        FIND_PATH(KDCRAW_INCLUDE_DIR libkdcraw/version.h
        ${_KDCRAWIncDir}
        )
    
        FIND_LIBRARY(KDCRAW_LIBRARIES NAMES kdcraw
        PATHS
        ${_KDCRAWLinkDir}
        )
    
        if (KDCRAW_INCLUDE_DIR AND KDCRAW_LIBRARIES)
            set(KDCRAW_FOUND TRUE)
        endif (KDCRAW_INCLUDE_DIR AND KDCRAW_LIBRARIES)
     endif(KDCRAW_VERSION_GOOD_FOUND) 
     if (KDCRAW_FOUND)
         if (NOT Kdcraw_FIND_QUIETLY)
             message(STATUS "Found libkdcraw: ${KDCRAW_LIBRARIES}")
         endif (NOT Kdcraw_FIND_QUIETLY)
     else (KDCRAW_FOUND)
         if (Kdcraw_FIND_REQUIRED)
             if (NOT KDCRAW_INCLUDE_DIR)
                 message(FATAL_ERROR "Could NOT find libkdcraw header files")
             endif (NOT KDCRAW_INCLUDE_DIR)
             if (NOT KDCRAW_LIBRARIES)
                 message(FATAL_ERROR "Could NOT find libkdcraw library")
             endif (NOT KDCRAW_LIBRARIES)
         endif (Kdcraw_FIND_REQUIRED)
     endif (KDCRAW_FOUND)
    
    MARK_AS_ADVANCED(KDCRAW_INCLUDE_DIR KDCRAW_LIBRARIES)

  endif(KDCRAW_LOCAL_FOUND)
  
endif (KDCRAW_INCLUDE_DIR AND KDCRAW_LIBRARIES)
