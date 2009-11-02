# Once done this will define
#
#  KONTO_FOUND - system has the Nepomuk-KDE backbone lib Konto
#  KONTO_INCLUDES - the libKonto include directory
#  KONTO_LIBRARIES - Link these to use libKonto
#

# Copyright (c) 2008, Sebastian Trueg, <sebastian@trueg.de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


if(KONTO_INCLUDES AND KONTO_LIBRARIES)
  # Already in cache, be silent
  set(Konto_FIND_QUIETLY TRUE)
endif(KONTO_INCLUDES AND KONTO_LIBRARIES


FIND_PATH(KONTO_INCLUDES 
  NAMES
  konto/class.h
  PATHS
  ${KDE4_INCLUDE_DIR}
  ${INCLUDE_INSTALL_DIR}
)

FIND_LIBRARY(KONTO_LIBRARIES 
  NAMES 
  konto
  PATHS
  ${KDE4_LIB_DIR}
  ${LIB_INSTALL_DIR}
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Kondo DEFAULT_MSG KONTO_INCLUDES KONTO_LIBRARIES)

