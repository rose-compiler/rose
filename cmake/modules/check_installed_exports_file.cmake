# INSTALL_DIR - set it to the install destination
# INSTALL_PREFIX - set it to CMAKE_INSTALL_PREFIX
# CURRENT_BINARY_DIR - set it to CMAKE_CURRENT_BINARY_DIR
# FILENAME  - the filename of the exports file

# get the absolute install directory, consider absolute and relative paths and also DESTDIR
if(IS_ABSOLUTE "${INSTALL_DIR}")
   set(installDir "$ENV{DESTDIR}${INSTALL_DIR}")
else(IS_ABSOLUTE "${INSTALL_DIR}")
   set(installDir "$ENV{DESTDIR}${INSTALL_PREFIX}/${INSTALL_DIR}")
endif(IS_ABSOLUTE "${INSTALL_DIR}")

set(installedExportsFile "${installDir}/${FILENAME}")


# if the file already exists at the install location, and if we can
# find the exports file in the build dir, read both, and if their contents differ,
# remove all configuration-specific exports files from the install dir, since
# they may create conflicts if the new targets have been added/targets have been 
# removed/ targets have been renamed/ the namespace for the exported targets has changed
if(EXISTS "${installedExportsFile}")
   if (${INSTALL_DIR} MATCHES "^(/)(.+)$")
      set(binaryDirExportFileDir "_${CMAKE_MATCH_2}")
      set(binaryDirExportsFile "${CURRENT_BINARY_DIR}/CMakeFiles/Export/${binaryDirExportFileDir}/${FILENAME}")

      if(EXISTS "${binaryDirExportsFile}") 
         file(READ "${installedExportsFile}" installedExportsFileContents)
         file(READ "${binaryDirExportsFile}" binaryDirExportsFileContents)

         if(NOT "${installedExportsFileContents}" STREQUAL "${binaryDirExportsFileContents}")

            if("${FILENAME}" MATCHES "^(.+)(\\.cmake)$")
               message(STATUS "Installed and new ${FILENAME} differ, removing installed ${CMAKE_MATCH_1}-*.cmake files")
               file(GLOB files "${installDir}/${CMAKE_MATCH_1}-*.cmake")
               file(REMOVE ${files})
            endif("${FILENAME}" MATCHES "^(.+)(\\.cmake)$")
#        else(NOT "${installedExportsFileContents}" STREQUAL "${binaryDirExportsFileContents}")
#           message(STATUS "FILES are the same")
         endif(NOT "${installedExportsFileContents}" STREQUAL "${binaryDirExportsFileContents}")

      endif(EXISTS "${binaryDirExportsFile}") 
   endif (${INSTALL_DIR} MATCHES "^(/)(.+)$")

endif(EXISTS "${installedExportsFile}")
