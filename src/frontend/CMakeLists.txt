
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

add_subdirectory(SageIII)
add_subdirectory(CxxFrontend)
#AS Add openfortran parser later
#add_subdirectory(OpenFortranParser_SAGE_Connection)
add_subdirectory(BinaryDisassembly)
add_subdirectory(BinaryLoader)
add_subdirectory(BinaryFormats)
add_subdirectory(Disassemblers)
#TP COMMENTED OUT FOR NOW. ADD LATER
#if (NOT WIN32)
add_subdirectory(PHPFrontend)
add_subdirectory(PythonFrontend)
#endif (NOT WIN32)

########### install files ###############

