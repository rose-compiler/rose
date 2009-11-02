# Once done this will define
#
# Nepomuk requires Soprano, so this module checks for Soprano too.
#
#  NEPOMUK_FOUND - system has Nepomuk
#  NEPOMUK_INCLUDE_DIR - the Nepomuk include directory
#  NEPOMUK_LIBRARIES - Link these to use Nepomuk
#  NEPOMUK_DEFINITIONS - Compiler switches required for using Nepomuk
#


# Copyright (c) 2008, Sebastian Trueg, <sebastian@trueg.de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


if (NOT DEFINED Soprano_FOUND)
  macro_optional_find_package(Soprano)
  macro_log_feature(Soprano_FOUND "Soprano" "Semantic Desktop Storing" "" FALSE "" "Soprano is needed for Nepomuk")
endif (NOT DEFINED Soprano_FOUND)

if (Soprano_FOUND)

  set (NEPOMUK_FIND_REQUIRED ${Nepomuk_FIND_REQUIRED})
  if (NEPOMUK_INCLUDE_DIR AND NEPOMUK_LIBRARIES)

    # Already in cache, be silent
    set(NEPOMUK_FIND_QUIETLY TRUE)

  else (NEPOMUK_INCLUDE_DIR AND NEPOMUK_LIBRARIES)
    find_path(NEPOMUK_INCLUDE_DIR
      NAMES
      nepomuk/global.h
      PATHS
      ${KDE4_INCLUDE_DIR}
      ${INCLUDE_INSTALL_DIR}
      )

    find_library(NEPOMUK_LIBRARIES
      NAMES
      nepomuk
      PATHS
      ${KDE4_LIB_DIR}
      ${LIB_INSTALL_DIR}
      )

    mark_as_advanced(NEPOMUK_INCLUDE_DIR NEPOMUK_LIBRARIES)

  endif (NEPOMUK_INCLUDE_DIR AND NEPOMUK_LIBRARIES)

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(NEPOMUK DEFAULT_MSG 
                                    NEPOMUK_LIBRARIES NEPOMUK_INCLUDE_DIR)
  #to retain backward compatibility
  set (Nepomuk_FOUND ${NEPOMUK_FOUND})

endif (Soprano_FOUND)
