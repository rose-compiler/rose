#
# Find an installation of Soprano
#
# Sets the following variables:
#  Soprano_FOUND            - true is Soprano has been found
#  SOPRANO_INCLUDE_DIR      - The include directory
#  SOPRANO_LIBRARIES        - The Soprano core library to link to (libsoprano)
#  SOPRANO_INDEX_LIBRARIES  - The Soprano index library (libsopranoindex)
#  SOPRANO_CLIENT_LIBRARIES - The Soprano client library (libsopranoclient)
#  SOPRANO_SERVER_LIBRARIES - The Soprano server library (libsopranoserver)
#  SOPRANO_VERSION          - The Soprano version (string value)
#
# SOPRANO_PLUGIN_NQUADPARSER_FOUND      - true if the nquadparser plugin is found
# SOPRANO_PLUGIN_NQUADSERIALIZER_FOUND  - true if the nquadserializer plugin is found
# SOPRANO_PLUGIN_RAPTORPARSER_FOUND     - true if the raptorparser plugin is found
# SOPRANO_PLUGIN_RAPTORSERIALIZER_FOUND - true if the raptorserializer plugin is found
# SOPRANO_PLUGIN_REDLANDBACKEND_FOUND   - true if the redlandbackend plugin is found
# SOPRANO_PLUGIN_SESAME2BACKEND_FOUND   - true if the sesame2backend plugin is found
#
# Options:
#  Set SOPRANO_MIN_VERSION to set the minimum required Soprano version (default: 1.99)
#

# Copyright (c) 2008, Sebastian Trueg, <sebastian@trueg.de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


#if(SOPRANO_INCLUDE_DIR AND SOPRANO_LIBRARIES AND SOPRANO_INDEX_LIBRARIES AND SOPRANO_SERVER_LIBRARIES)

  # read from cache
#  set(Soprano_FOUND TRUE)
#  set(SopranoServer_FOUND TRUE)
#  set(SopranoClient_FOUND TRUE)
#  set(SopranoIndex_FOUND TRUE)

#else(SOPRANO_INCLUDE_DIR AND SOPRANO_LIBRARIES AND SOPRANO_INDEX_LIBRARIES AND SOPRANO_SERVER_LIBRARIES)
  include(FindLibraryWithDebug)

  find_path(SOPRANO_INCLUDE_DIR 
    NAMES
    soprano/soprano.h
    PATHS
    ${KDE4_INCLUDE_DIR}
    ${INCLUDE_INSTALL_DIR}
    )

  find_library_with_debug(SOPRANO_INDEX_LIBRARIES 
    WIN32_DEBUG_POSTFIX d
    NAMES
    sopranoindex
    PATHS
    ${KDE4_LIB_DIR}
    ${LIB_INSTALL_DIR}
    )

  find_library_with_debug(SOPRANO_CLIENT_LIBRARIES 
    WIN32_DEBUG_POSTFIX d
    NAMES
    sopranoclient
    PATHS
    ${KDE4_LIB_DIR}
    ${LIB_INSTALL_DIR}
    )

  find_library_with_debug(SOPRANO_LIBRARIES
    WIN32_DEBUG_POSTFIX d
    NAMES soprano
    PATHS
    ${KDE4_LIB_DIR}
    ${LIB_INSTALL_DIR}
  )

  find_library_with_debug(SOPRANO_SERVER_LIBRARIES 
    WIN32_DEBUG_POSTFIX d
    NAMES
    sopranoserver
    PATHS
    ${KDE4_LIB_DIR}
    ${LIB_INSTALL_DIR}
    )

  # check for all the libs as required to make sure that we do not try to compile with an old version

  if(SOPRANO_INCLUDE_DIR AND SOPRANO_LIBRARIES)
    set(Soprano_FOUND TRUE)
  endif(SOPRANO_INCLUDE_DIR AND SOPRANO_LIBRARIES)

  if(Soprano_FOUND AND SOPRANO_INDEX_LIBRARIES)
    set(SopranoIndex_FOUND TRUE)
  endif(Soprano_FOUND AND SOPRANO_INDEX_LIBRARIES)

  if(Soprano_FOUND AND SOPRANO_CLIENT_LIBRARIES)
    set(SopranoClient_FOUND TRUE)
  endif(Soprano_FOUND AND SOPRANO_CLIENT_LIBRARIES)

  if(Soprano_FOUND AND SOPRANO_SERVER_LIBRARIES)
    set(SopranoServer_FOUND TRUE)
  endif(Soprano_FOUND AND SOPRANO_SERVER_LIBRARIES)
  
  # check Soprano version

  # We set a default for the minimum required version to be backwards compatible
  if(NOT SOPRANO_MIN_VERSION)
    set(SOPRANO_MIN_VERSION "1.99")
  endif(NOT SOPRANO_MIN_VERSION)

  if(Soprano_FOUND)
    file(READ ${SOPRANO_INCLUDE_DIR}/soprano/version.h SOPRANO_VERSION_CONTENT)
    string(REGEX MATCH "SOPRANO_VERSION_STRING \".*\"\n" SOPRANO_VERSION_MATCH ${SOPRANO_VERSION_CONTENT})
    if(SOPRANO_VERSION_MATCH)
      string(REGEX REPLACE "SOPRANO_VERSION_STRING \"(.*)\"\n" "\\1" SOPRANO_VERSION ${SOPRANO_VERSION_MATCH})
      if(SOPRANO_VERSION STRLESS "${SOPRANO_MIN_VERSION}")
        set(Soprano_FOUND FALSE)
        if(Soprano_FIND_REQUIRED)
          message(FATAL_ERROR "Soprano version ${SOPRANO_VERSION} is too old. Please install ${SOPRANO_MIN_VERSION} or newer")
        else(Soprano_FIND_REQUIRED)
          message(STATUS "Soprano version ${SOPRANO_VERSION} is too old. Please install ${SOPRANO_MIN_VERSION} or newer")
        endif(Soprano_FIND_REQUIRED)
      endif(SOPRANO_VERSION STRLESS "${SOPRANO_MIN_VERSION}")
    endif(SOPRANO_VERSION_MATCH)
  endif(Soprano_FOUND)
  
  #look for parser plugins
  if(Soprano_FOUND)
    find_path(SOPRANO_PLUGIN_DIR 
      NAMES
      soprano/plugins
      PATHS
      ${SHARE_INSTALL_PREFIX} /usr/share /usr/local/share
      NO_DEFAULT_PATH
      NO_SYSTEM_ENVIRONMENT_PATH
      )
    set(SOPRANO_PLUGIN_DIR "${SOPRANO_PLUGIN_DIR}/soprano/plugins")

    if(EXISTS ${SOPRANO_PLUGIN_DIR}/nquadparser.desktop)
      set(SOPRANO_PLUGIN_NQUADPARSER_FOUND TRUE)
      set(_plugins "${_plugins} nquadparser")
    endif(EXISTS ${SOPRANO_PLUGIN_DIR}/nquadparser.desktop)

    if(EXISTS ${SOPRANO_PLUGIN_DIR}/nquadserializer.desktop)
      set(SOPRANO_PLUGIN_NQUADSERIALIZER_FOUND TRUE)
      set(_plugins "${_plugins} nquadserializer")
    endif(EXISTS ${SOPRANO_PLUGIN_DIR}/nquadserializer.desktop)

    if(EXISTS ${SOPRANO_PLUGIN_DIR}/raptorparser.desktop)
      set(SOPRANO_PLUGIN_RAPTORPARSER_FOUND TRUE)
      set(_plugins "${_plugins} raptorparser")
    endif(EXISTS ${SOPRANO_PLUGIN_DIR}/raptorparser.desktop)

    if(EXISTS ${SOPRANO_PLUGIN_DIR}/raptorserializer.desktop)
      set(SOPRANO_PLUGIN_RAPTORSERIALIZER_FOUND TRUE)
      set(_plugins "${_plugins} raptorserializer")
    endif(EXISTS ${SOPRANO_PLUGIN_DIR}/raptorserializer.desktop)

    if(EXISTS ${SOPRANO_PLUGIN_DIR}/redlandbackend.desktop)
      set(SOPRANO_PLUGIN_REDLANDBACKEND_FOUND TRUE)
      set(_plugins "${_plugins} redlandbackend")
    endif(EXISTS ${SOPRANO_PLUGIN_DIR}/redlandbackend.desktop)

    if(EXISTS ${SOPRANO_PLUGIN_DIR}/sesame2backend.desktop)
      set(SOPRANO_PLUGIN_SESAME2BACKEND_FOUND TRUE)
      set(_plugins "${_plugins} sesame2backend")
    endif(EXISTS ${SOPRANO_PLUGIN_DIR}/sesame2backend.desktop)

  endif(Soprano_FOUND)

  if(Soprano_FOUND)
    if(NOT Soprano_FIND_QUIETLY)
      message(STATUS "Found Soprano: ${SOPRANO_LIBRARIES}")
      message(STATUS "Found Soprano includes: ${SOPRANO_INCLUDE_DIR}")
      message(STATUS "Found Soprano Index: ${SOPRANO_INDEX_LIBRARIES}")
      message(STATUS "Found Soprano Client: ${SOPRANO_CLIENT_LIBRARIES}")
      message(STATUS "Found Soprano Plugin Dir: ${SOPRANO_PLUGIN_DIR}")
      message(STATUS "Found Soprano Plugins:${_plugins}")
    endif(NOT Soprano_FIND_QUIETLY)
  else(Soprano_FOUND)
    if(Soprano_FIND_REQUIRED)
      if(NOT SOPRANO_INCLUDE_DIR)
	message(FATAL_ERROR "Could not find Soprano includes.")
      endif(NOT SOPRANO_INCLUDE_DIR)
      if(NOT SOPRANO_LIBRARIES)
	message(FATAL_ERROR "Could not find Soprano library.")
      endif(NOT SOPRANO_LIBRARIES)
    else(Soprano_FIND_REQUIRED)
      if(NOT SOPRANO_INCLUDE_DIR)
        message(STATUS "Could not find Soprano includes.")
      endif(NOT SOPRANO_INCLUDE_DIR)
      if(NOT SOPRANO_LIBRARIES)
        message(STATUS "Could not find Soprano library.")
      endif(NOT SOPRANO_LIBRARIES)
    endif(Soprano_FIND_REQUIRED)
  endif(Soprano_FOUND)

mark_as_advanced(SOPRANO_CLIENT_LIBRARIES SOPRANO_INDEX_LIBRARIES SOPRANO_LIBRARIES SOPRANO_SERVER_LIBRARIES SOPRANO_INCLUDE_DIR )

#endif(SOPRANO_INCLUDE_DIR AND SOPRANO_LIBRARIES AND SOPRANO_INDEX_LIBRARIES AND SOPRANO_SERVER_LIBRARIES)
