################################################################
########## DEPRECATED, use FindQImageBlitz instead #############
################################################################
# - Try to find blitz lib
# Once done this will define
#
#  BLITZ_FOUND - system has blitz lib
#  BLITZ_INCLUDES - the blitz include directory
#  BLITZ_LIBRARIES - The libraries needed to use blitz

# Copyright (c) 2006, Montel Laurent, <montel@kde.org>
# Copyright (c) 2007, Allen Winter, <winter@kde.org>
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

include(FindLibraryWithDebug)

if (BLITZ_INCLUDES AND BLITZ_LIBRARIES)
  set(Blitz_FIND_QUIETLY TRUE)
endif (BLITZ_INCLUDES AND BLITZ_LIBRARIES)

if (NOT WIN32)
    # use pkg-config to get the directories and then use these values
    # in the FIND_PATH() and FIND_LIBRARY() calls
    find_package(PkgConfig)
    pkg_check_modules(PC_BLITZ qimageblitz)
endif (NOT WIN32)

find_path(BLITZ_INCLUDES
  NAMES
  qimageblitz.h
  PATH_SUFFIXES qimageblitz
  PATHS
  $ENV{QIMAGEBLITZDIR}/include
  ${PC_BLITZ_INCLUDEDIR}
  ${PC_BLITZ_INCLUDE_DIRS}
  ${KDE4_INCLUDE_DIR}
  ${INCLUDE_INSTALL_DIR}
)

find_library_with_debug(BLITZ_LIBRARIES
  WIN32_DEBUG_POSTFIX d
  qimageblitz
  PATHS
  $ENV{QIMAGEBLITZDIR}/lib
  ${PC_BLITZ_LIBRARY_DIRS}
  ${PC_BLITZ_LIBDIR}
  ${KDE4_LIB_DIR}
  ${LIB_INSTALL_DIR}
)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Blitz DEFAULT_MSG 
                                  BLITZ_INCLUDES BLITZ_LIBRARIES)
mark_as_advanced(BLITZ_INCLUDES BLITZ_LIBRARIES)
