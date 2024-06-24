# May want to eventually add:
# if(ENABLE-BINARY-ANALYSIS)
# if(ENABLE-READWRITE-SET

# Codethorn has never been tested on Windows 
# Codethorn/src/ has some .h and .C files that require ifdef macros
# if we want it to work on Windows in the future

if(NOT WIN32 AND ENABLE-C STREQUAL "ON") 
  add_subdirectory(CodeThorn) 
endif() 

add_subdirectory(BinaryAnalysis)
add_subdirectory(ReadWriteSetGeneration)
