# Once done this will define
#
#  KNEPOMUK_FOUND - system has the Nepomuk-KDE backbone lib KNep
#  KNEPOMUK_INCLUDES - the libKNep include directory
#  KNEPOMUK_LIBRARIES - Link these to use libKNep
#

# Copyright (c) 2008, Sebastian Trueg, <sebastian@trueg.de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


if(KNEPOMUK_INCLUDES AND KNEPOMUK_LIBRARIES)
	set(KNepomuk_FIND_QUIETLY TRUE)
endif(KNEPOMUK_INCLUDES AND KNEPOMUK_LIBRARIES)

FIND_PATH(KNEPOMUK_INCLUDES 
  NAMES
  knepomuk/knepomuk.h
  PATHS
  ${KDE4_INCLUDE_DIR}
  ${INCLUDE_INSTALL_DIR}
)

FIND_LIBRARY(KNEPOMUK_LIBRARIES 
  NAMES 
  knepomuk
  PATHS
  ${KDE4_LIB_DIR}
  ${LIB_INSTALL_DIR}
)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(KNepomuk DEFAULT_MSG KNEPOMUK_INCLUDES KNEPOMUK_LIBRARIES )


MARK_AS_ADVANCED(KNEPOMUK_INCLUDES KNEPOMUK_LIBRARIES)

