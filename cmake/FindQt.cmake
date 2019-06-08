# Qt detection for ROSE.
#
#  INPUTS:
#    QT_ROOT       -- QT installation directory or "no" or empty
#                       * If "no" then do not use QT and do not search for it.
#                       * If empty use QT if found, no error if not found
#                       * Else require QT to exist at specified location and use it
#
#  OUTPUTS:
#    QT_FOOUND     -- Boolean: whether the QT library was found.

macro(find_qt)
  if("${QT_ROOT}" STREQUAL "no")
    # Do not use QT, and therefore do not even search for it. Make sure all outputs are cleared to avoid problems with
    # users maybe setting them.
    set(QT_FOUND FALSE)

  elseif("${QT_ROOT}" STREQUAL "")
    message(WARNING "Qt searching is not supported yet; assuming -DQT_ROOT=no")
    set(QT_ROOT no)
    set(QT_FOUND FALSE)

  else()
    message(FATAL_ERROR "Qt explicit root is not supported yet; use -DQT_ROOT=no")
  endif()
  
  # Summarize
  if(VERBOSE)
    message(STATUS "QT_ROOT       = '${QT_ROOT}'")
    message(STATUS "QT_FOUND      = '${QT_FOUND}'")
  endif()

  # ROSE variables
endmacro()
