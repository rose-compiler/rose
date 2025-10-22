if(NOT ENABLE_INTERNALFRONTENDDEVELOPMENT)
  # Self-contained qrose build - only builds if Qt4 is available
  # Suppress policy warning from legacy FindQt4.cmake module
  if(POLICY CMP0153)
    cmake_policy(SET CMP0153 OLD)
  endif()

  find_package(Qt4 4.4 QUIET COMPONENTS QtCore QtGui Qt3Support)
  if(NOT QT4_FOUND)
    message(STATUS "Qt4 not found or version < 4.4, skipping qrose")
    return()
  endif()

  message(STATUS "Building qrose library (Qt ${QT_VERSION_MAJOR}.${QT_VERSION_MINOR})")
  include(${QT_USE_FILE})

  add_definitions(-DQT3_SUPPORT)
  include_directories(${QT_QT3SUPPORT_INCLUDE_DIR})

  add_subdirectory(Framework)
  add_subdirectory(Widgets)
  add_subdirectory(Components)
  add_subdirectory(QRoseLib)
endif()
