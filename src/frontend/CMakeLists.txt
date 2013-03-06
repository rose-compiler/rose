
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(SageIII)
add_subdirectory(BinaryDisassembly)
add_subdirectory(BinaryLoader)
add_subdirectory(BinaryFormats)
add_subdirectory(Disassemblers)

if(enable-c)
  add_subdirectory(CxxFrontend)
endif()
if(enable-fortran)
  add_subdirectory(OpenFortranParser_SAGE_Connection)
endif()
if(enable-java)
  add_subdirectory(ECJ_ROSE_Connection)
endif()

#TP COMMENTED OUT FOR NOW. ADD LATER
#if (NOT WIN32)
if(enable-php)
  add_subdirectory(PHPFrontend)
endif()

if(enable-python)
  add_subdirectory(PythonFrontend)
endif()
add_subdirectory(PHPFrontend)
#endif (NOT WIN32)


########### install files ###############

