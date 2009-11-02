# - Try to find konqueror library
# Once done this will define
#
#  LIBKONQ_FOUND - system has libkonq library
#  LIBKONQ_INCLUDE_DIR - the LIBKONQ include directory
#  LIBKONQ_LIBRARY - the libkonq library
#
#  Original file: FindMarbleWidget.cmake (found in digikam-0.10.0-beta2)
#  copyright 2008 by Patrick Spendrin <ps_ml@gmx.de>
#  use this file as you like
#
#  Modifications to find libkonq by Joachim Eibl 2008

if(LIBKONQ_INCLUDE_DIR AND LIBKONQ_LIBRARY)

  # Already in cache
  set(LIBKONQ_FOUND TRUE)

else(LIBKONQ_INCLUDE_DIR AND LIBKONQ_LIBRARY)
  find_path(LIBKONQ_INCLUDE_DIR konq_popupmenuplugin.h )

  find_library(LIBKONQ_LIBRARY konq)

  if(LIBKONQ_INCLUDE_DIR AND LIBKONQ_LIBRARY)
    set(LIBKONQ_FOUND TRUE)
  endif(LIBKONQ_INCLUDE_DIR AND LIBKONQ_LIBRARY)

  if(LIBKONQ_FOUND)
    if (NOT LIBKONQ_FIND_QUIETLY)
      message(STATUS "Found libkonq: ${LIBKONQ_LIBRARY}")
    endif (NOT LIBKONQ_FIND_QUIETLY)
  else(LIBKONQ_FOUND)
    if(LIBKONQ_FIND_REQUIRED)
      message(FATAL_ERROR "Could NOT find KDE4 libkonq library")
    endif(LIBKONQ_FIND_REQUIRED)
  endif(LIBKONQ_FOUND)

  mark_as_advanced(LIBKONQ_INCLUDE_DIR LIBKONQ_LIBRARY)
endif(LIBKONQ_INCLUDE_DIR AND LIBKONQ_LIBRARY)
