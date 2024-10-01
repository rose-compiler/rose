find_package(LLVM REQUIRED CONFIG) # Use CONFIG mode to find the modern CMake LLVM package
find_package(Clang REQUIRED CONFIG) # Similar for Clang

message(STATUS "Building libroseClangFrontend") 
message(STATUS "Boost_INCLUDE_DIR ${Boost_INCLUDE_DIR}") 
message(STATUS "Building with Clang frontend")
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "LLVM -I ${LLVM_INCLUDE_DIRS}") # ${CLANG_INCLUDE_DIRS} is the same path as ${LLVM_INCLUDE_DIRS

# TODO use target_include_directories()
include_directories(.  ${LLVM_INCLUDE_DIRS} ${ROSE_INCLUDES}) 
add_definitions(${LLVM_DEFINITIONS})

################################################################################
#
# This section of code is from an intern's attempt in the past
#
################################################################################
# Use LLVM and Clang's CMake targets for linking  
#llvm_map_components_to_libnames(llvm_libs support core irreader mcjit ipo bitwriter linker asmparser instrumentation option frontendopenmp)
 
# Prepare the libraries, preferring to use imported targets for LLVM/Clang if possible
  set(llvm_libs
    ${llvm_libs} # LLVM components mapped to libraries
    ${CLANG_LIBS} # Clang libraries (ensure these are properly found by CMake)
)

  # Extra libraries for the Clang frontend tools
  set(clang_lib
    ${llvm_libs} #roseClangFrontend
    clangFrontend clangStaticAnalyzerFrontend clangStaticAnalyzerCheckers
    clangStaticAnalyzerCore clangIndex clangFrontend clangCodeGen
    clangARCMigrate clangRewrite clangSerialization clangDriver
    clangParse clangSema clangAnalysis clangAST clangLex clangBasic
)
################################################################################
#
# End of Section 
#
################################################################################

# Source files of roseClangFrontend 
set(roseClangFrontend_SOURCE_FILES clang-frontend.cpp 
                                   clang-frontend-decl.cpp 
                                   clang-frontend-stmt.cpp
                                   clang-frontend-type.cpp
                                   clang-to-dot.cpp 
                                   clang-to-dot-decl.cpp 
                                   clang-to-dot-stmt.cpp 
                                   clang-to-dot-type.cpp  
                                   clang-to-rose-support.cpp) 



# Flags to use when compiling  
add_compile_options(
    $<$<BOOL:${ENABLE_ALL_WARNINGS}>:-Wall>
    $<$<NOT:$<BOOL:${ENABLE_ALL_WARNINGS}>>:-Wno-misleading-indentation>
    -fno-rtti # fixes compilation error around 25% 
) 

# Create the target (May want STATIC) 
add_library(roseClangFrontend OBJECT ${roseClangFrontend_SOURCE_FILES}) 

#target_include_directories(roseClangFrontend PUBLIC ${CMAKE_BINARY_DIR}/src/frontend/SageIII) 
add_dependencies(roseClangFrontend rosetta_generated)

target_link_libraries(roseClangFrontend ${llvm_libs})

# Install rules   
install(TARGETS roseClangFrontend
        LIBRARY DESTINATION lib)
