
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############


# DQ (12/3/2009): All these files compile, but on Windows the ${roseBinaryFormats_SRC_part2}
# list of files cause the library to fail to link.  We can look into this more later.
# Each file has been confirmed to indivually cause the library to fail to build.
set(roseBinaryFormats_SRC ${roseBinaryFormats_fileList_part0} )
# ${roseBinaryFormats_fileList_part2} )

# Since it is a linking issue specific to Windows, we currently exclude 
# the problem files from the generation of the library.
# add_library( roseBinaryFormats STATIC ${roseBinaryFormats_SRC} )
#if (WIN32)
#   add_library( roseBinaryFormats STATIC ${roseBinaryFormats_fileList_part1} )
#else (WIN32)
#   add_library( roseBinaryFormats STATIC ${roseBinaryFormats_SRC} )
#endif (WIN32)

# Alternative code to demonstrate and test failing library build.
# add_library( roseBinaryFormats STATIC ${roseBinaryFormats_SRC_part1}
#
   ### Simulated Loader Memory Mapping ###
# These files each compile, but cause the creation of the library on Windows to fail.
# Confirmed Failing File  ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/Loader.C 
# Confirmed Failing File  ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/LoaderELF.C 
# Confirmed Failing File  ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/LoaderELFObj.C 
# Confirmed Failing File  ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/LoaderPE.C 

   ### Other Formats (partial implementations and not so well tested) ###
# Confirmed Failing File  ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/ExecDOS.C 
# Confirmed Failing File  ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/ExecNE.C 
# Confirmed Failing File  ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/ExecLE.C 
# )

#add_library( roseBinaryFormats STATIC ${roseBinaryFormats_fileList_part0} )

#add_dependencies( roseBinaryFormats ROSETTA )

########### install files ###############

install(FILES  DataConversion.h IntelPinSupport.h MemoryMap.h
        DESTINATION ${INCLUDE_INSTALL_DIR})
