add_subdirectory(SageIII)
add_subdirectory(BinaryFormats)
add_subdirectory(Disassemblers)
add_subdirectory(CxxFrontend)
add_subdirectory(OpenFortranParser_SAGE_Connection)
add_subdirectory(ECJ_ROSE_Connection)
add_subdirectory(PHPFrontend)
add_subdirectory(PythonFrontend)
add_subdirectory(Experimental_Ada_ROSE_Connection)

if(ENABLE-JOVIAL)
  # TSG 9/10/24: The CMakeLists.txt in this dir is wip  
  add_subdirectory(Experimental_Jovial_ROSE_Connection)
endif()
