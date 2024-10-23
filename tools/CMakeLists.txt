#
#  Codethorn has never been tested on Windows, RWSetGeneration has linking errors on Windows 
#  Codethorn/src/ has some .h and .C files that require ifdef macros If we want it to work on Windows in the future
#

if(NOT WIN32 AND ENABLE-C STREQUAL "ON") 
  if(NOT ENABLE-ADA) 
    # The "tools" that "make tools" makes, under these conditions #
    message(STATUS "Running make tools will compile the tools supported by this configuration") 
    message(STATUS "The tools supported by this configuration are Codethorn and RWSET-ANALYSIS") 

    # CODETHORN 
    add_subdirectory(CodeThorn)                          
    add_dependencies(tools codethorn ltlthorn codethorn1)

    # RWSET-ANALYSIS 
    add_subdirectory(ReadWriteSetGeneration)
    add_dependencies(tools LocalRWSetGenerator Name2Node TestabilityGrader)  

    # Additional Tools supported by this configuration or these conditions go here #


    ################################################################################
  endif() 
endif()

# Always add BinaryAnalysis subdirectory 
add_subdirectory(BinaryAnalysis)
# When Enabled at config time, build the Binary analysis tools 
if(ENABLE-BINARY-ANALYSIS)
  # Lots of executables and libs here, there may be a better way
  add_dependencies(tools bat bat-ana bat-cc bat-cfg bat-cg bat-check-execution bat-container bat-dataflow bat-debug-semantics bat-delta-bijection bat-dis bat-dwarf-lines bat-flir-ascribe bat-flir-insert bat-flir-lib bat-insnfreq bat-insnfreq-cmp bat-entropy bat-lsb bat-lsd bat-lsf bat-lsv bat-mem bat-native-trace bat-pointers bat-prop bat-rwsets bat-scan-magic bat-similar-functions bat-simplify bat-stack-deltas bat-to-c bat-trace bat-var bat-xml-to-json) 
endif() 
