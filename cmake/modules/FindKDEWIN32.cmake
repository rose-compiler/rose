# - Try to find the KDEWIN32 library
# 
# Once done this will define
#
#  KDEWIN32_FOUND - system has KDEWIN32
#  KDEWIN32_INCLUDES - the KDEWIN32 include directories
#  KDEWIN32_LIBRARIES - The libraries needed to use KDEWIN32

# Copyright (c) 2006, Alexander Neundorf, <neundorf@kde.org>
# Copyright (c) 2007-2008, Ralf Habacker, <ralf.habacker@freenet.de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


if (WIN32)
  if (NOT KDEWIN32_DIR)

    find_path(KDEWIN32_INCLUDE_DIR winposix_export.h
      ${CMAKE_INCLUDE_PATH}
      ${CMAKE_INSTALL_PREFIX}/include
    )
 
    # search for kdewin32 in the default install directory for applications (default of (n)make install)
    FILE(TO_CMAKE_PATH "${CMAKE_LIBRARY_PATH}" _cmakeLibraryPathCmakeStyle)

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set (LIBRARY_NAME kdewin32d)
    else (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set (LIBRARY_NAME kdewin32)
    endif (CMAKE_BUILD_TYPE STREQUAL "Debug")

    find_library(KDEWIN32_LIBRARY
      NAMES ${LIBRARY_NAME}
      PATHS 
        ${_cmakeLibraryPathCmakeStyle}
        ${CMAKE_INSTALL_PREFIX}/lib
      NO_SYSTEM_ENVIRONMENT_PATH
    )

    if (KDEWIN32_LIBRARY AND KDEWIN32_INCLUDE_DIR)
      set(KDEWIN32_FOUND TRUE)
      # add needed system libs
      set(KDEWIN32_LIBRARIES ${KDEWIN32_LIBRARY} user32 shell32 ws2_32 netapi32 userenv)
  
      if (MINGW)
        #mingw compiler
        set(KDEWIN32_INCLUDES ${KDEWIN32_INCLUDE_DIR} ${KDEWIN32_INCLUDE_DIR}/mingw)
      else (MINGW)
        # msvc compiler
        # add the MS SDK include directory if available
        file(TO_CMAKE_PATH "$ENV{MSSDK}" MSSDK_DIR)
        set(KDEWIN32_INCLUDES ${KDEWIN32_INCLUDE_DIR} ${KDEWIN32_INCLUDE_DIR}/msvc ${MSSDK_DIR})
      endif (MINGW)
  
    endif (KDEWIN32_LIBRARY AND KDEWIN32_INCLUDE_DIR)
    # required for configure
    set(CMAKE_REQUIRED_INCLUDES ${CMAKE_REQUIRED_INCLUDES} ${KDEWIN32_INCLUDES})
    set(CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} ${KDEWIN32_LIBRARIES})      

  endif (NOT KDEWIN32_DIR)

  if (KDEWIN32_FOUND)
    if (NOT KDEWIN32_FIND_QUIETLY)
      message(STATUS "Found kdewin32 library: ${KDEWIN32_LIBRARY}")
    endif (NOT KDEWIN32_FIND_QUIETLY)

  else (KDEWIN32_FOUND)
    if (KDEWIN32_FIND_REQUIRED)
      message(FATAL_ERROR "Could NOT find KDEWIN32 library\nPlease install it first")
    endif (KDEWIN32_FIND_REQUIRED)
  endif (KDEWIN32_FOUND)
endif (WIN32)
