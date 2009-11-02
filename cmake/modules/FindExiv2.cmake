# - Try to find the Exiv2 library
#
#  EXIV2_MIN_VERSION - You can set this variable to the minimum version you need 
#                      before doing FIND_PACKAGE(Exiv2). The default is 0.12.
# 
# Once done this will define
#
#  EXIV2_FOUND - system has libexiv2
#  EXIV2_INCLUDE_DIR - the libexiv2 include directory
#  EXIV2_LIBRARIES - Link these to use libexiv2
#  EXIV2_DEFINITIONS - Compiler switches required for using libexiv2
#

# Copyright (c) 2008, Gilles Caulier, <caulier.gilles@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


if (EXIV2_INCLUDE_DIR AND EXIV2_LIBRARIES)

  # in cache already
  set(EXIV2_FOUND TRUE)

else (EXIV2_INCLUDE_DIR AND EXIV2_LIBRARIES)
  if (NOT WIN32)
  # use pkg-config to get the directories and then use these values
  # in the FIND_PATH() and FIND_LIBRARY() calls
  include(UsePkgConfig)

  if(NOT EXIV2_MIN_VERSION)
    set(EXIV2_MIN_VERSION "0.12")
  endif(NOT EXIV2_MIN_VERSION)
  
  pkgconfig(exiv2 _EXIV2IncDir _EXIV2LinkDir _EXIV2LinkFlags _EXIV2Cflags)

  if(_EXIV2LinkFlags)
    # query pkg-config asking for a Exiv2 >= 0.12
    exec_program(${PKGCONFIG_EXECUTABLE} ARGS --atleast-version=${EXIV2_MIN_VERSION} exiv2 RETURN_VALUE _return_VALUE OUTPUT_VARIABLE _pkgconfigDevNull )
    if(_return_VALUE STREQUAL "0")
      message(STATUS "Found Exiv2 release >= ${EXIV2_MIN_VERSION}")
      set(EXIV2_VERSION_GOOD_FOUND TRUE)
    else(_return_VALUE STREQUAL "0")
      message(STATUS "Found Exiv2 release < ${EXIV2_MIN_VERSION}")
    endif(_return_VALUE STREQUAL "0")
  else(_EXIV2LinkFlags)
      set(EXIV2_FOUND FALSE)
      set(EXIV2_VERSION_GOOD_FOUND FALSE)
      message(STATUS "Cannot find Exiv2 library!")
  endif(_EXIV2LinkFlags)
  
  else(NOT WIN32)
     #Better check
     set(EXIV2_VERSION_GOOD_FOUND TRUE)
  endif (NOT WIN32)

  if(EXIV2_VERSION_GOOD_FOUND)
     set(EXIV2_DEFINITIONS ${_EXIV2Cflags})
 
     find_path(EXIV2_INCLUDE_DIR exiv2/exif.hpp
       ${_EXIV2IncDir}
     )
  
     find_library(EXIV2_LIBRARIES NAMES exiv2 libexiv2
       PATHS
       ${_EXIV2LinkDir}
     )
  
     if (EXIV2_INCLUDE_DIR AND EXIV2_LIBRARIES)
        set(EXIV2_FOUND TRUE)
        # TODO version check is missing
     endif (EXIV2_INCLUDE_DIR AND EXIV2_LIBRARIES)
   endif(EXIV2_VERSION_GOOD_FOUND)
   if (EXIV2_FOUND)
      if (NOT Exiv2_FIND_QUIETLY)
       message(STATUS "Found Exiv2: ${EXIV2_LIBRARIES}")
      endif (NOT Exiv2_FIND_QUIETLY)
   else (EXIV2_FOUND)
     if (Exiv2_FIND_REQUIRED)
       if (NOT EXIV2_INCLUDE_DIR)
         message(FATAL_ERROR "Could NOT find Exiv2 header files")
       endif (NOT EXIV2_INCLUDE_DIR)
       if (NOT EXIV2_LIBRARIES)
           message(FATAL_ERROR "Could NOT find Exiv2 library")
       endif (NOT EXIV2_LIBRARIES)
     endif (Exiv2_FIND_REQUIRED)
   endif (EXIV2_FOUND)

  mark_as_advanced(EXIV2_INCLUDE_DIR EXIV2_LIBRARIES)
  
endif (EXIV2_INCLUDE_DIR AND EXIV2_LIBRARIES)

