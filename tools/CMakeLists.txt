# TO DO: add 'make tools' target 

# Codethorn has never been tested on Windows 
# Codethorn/src/ has some .h and .C files that require ifdef macros
# if we want it to work on Windows in the future
# RWSetGeneration has linking errors on Windows 

if(NOT WIN32 AND ENABLE-C STREQUAL "ON") 
  if(NOT ENABLE-ADA) 
    add_subdirectory(CodeThorn) 
    add_subdirectory(ReadWriteSetGeneration)
  endif() 
endif()
add_subdirectory(BinaryAnalysis)
