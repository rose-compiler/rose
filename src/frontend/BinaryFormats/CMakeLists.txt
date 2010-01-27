
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(roseBinaryFormats_fileList_part0
   ### Utilities ###
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/DataConversion.C
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/ExecGeneric.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/ExtentMap.C
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/Hexdump.C
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/Interpretation.C
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/MemoryMap.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/Rva.C 

   ### Generic Base Classes ###
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/GenericDynamicLinking.C
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/GenericFile.C
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/GenericFormat.C
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/GenericHeader.C
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/GenericSection.C
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/GenericString.C

   ### Executable Linking Format (ELF) ###
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/ElfDynamicLinking.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/ElfErrorFrame.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/ElfFileHeader.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/ElfNote.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/ElfRelocation.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/ElfSection.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/ElfSectionTable.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/ElfSegmentTable.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/ElfStringTable.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/ElfSymbolTable.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/ElfSymbolVersion.C 

   ### Microsoft Windows PE Format */
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/PeExport.C
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/PeFileHeader.C
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/PeImport.C
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/PeSection.C
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/PeSectionTable.C
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/PeStringTable.C
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/PeSymbolTable.C

   ### Simulated Loader Memory Mapping ###
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/Loader.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/LoaderELF.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/LoaderELFObj.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/LoaderPE.C 

   ### Other Formats (partial implementations and not so well tested) ###
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/ExecDOS.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/ExecNE.C 
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryFormats/ExecLE.C 
   )

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

add_library( roseBinaryFormats STATIC ${roseBinaryFormats_fileList_part0} )
#add_library( roseBinaryFormats1 STATIC ${roseBinaryFormats_fileList_part1} )
#add_library( roseBinaryFormats2 STATIC ${roseBinaryFormats_fileList_part2} )

add_dependencies( roseBinaryFormats ROSETTA )
#add_dependencies( roseBinaryFormats1 ROSETTA )
#add_dependencies( roseBinaryFormats2 ROSETTA )

########### install files ###############

install(FILES  DataConversion.h IntelPinSupport.h Loader.h LoaderELF.h LoaderELFObj.h LoaderPE.h MemoryMap.h DESTINATION ${INCLUDE_INSTALL_DIR})
