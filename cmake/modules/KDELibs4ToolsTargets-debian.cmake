#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debian".
#----------------------------------------------------------------

# Commands may need to know the format version.
SET(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "KDE4__kconfig_compiler" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__kconfig_compiler APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__kconfig_compiler PROPERTIES
  IMPORTED_LOCATION_DEBIAN "/usr/bin/kconfig_compiler"
  )

# Import target "KDE4__meinproc4" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__meinproc4 APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__meinproc4 PROPERTIES
  IMPORTED_LOCATION_DEBIAN "/usr/bin/meinproc4"
  )

# Import target "KDE4__makekdewidgets" for configuration "Debian"
SET_PROPERTY(TARGET KDE4__makekdewidgets APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBIAN)
SET_TARGET_PROPERTIES(KDE4__makekdewidgets PROPERTIES
  IMPORTED_LOCATION_DEBIAN "/usr/bin/makekdewidgets"
  )

# Commands beyond this point should not need to know the version.
SET(CMAKE_IMPORT_FILE_VERSION)
