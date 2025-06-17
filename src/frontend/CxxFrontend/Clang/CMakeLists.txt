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

# Extra libraries for the Clang frontend tools
set(CLANG_LIBRARIES
  clangFrontend clangStaticAnalyzerFrontend clangStaticAnalyzerCheckers
  clangStaticAnalyzerCore clangIndex clangFrontend clangCodeGen
  clangARCMigrate clangRewrite clangSerialization clangDriver
  clangParse clangSema clangAnalysis clangAST clangLex clangBasic)

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
) 

# Create the target (May want STATIC) 
add_library(roseClangFrontend OBJECT ${roseClangFrontend_SOURCE_FILES}) 

#target_include_directories(roseClangFrontend PUBLIC ${CMAKE_BINARY_DIR}/src/frontend/SageIII) 
add_dependencies(roseClangFrontend rosetta_generated)

target_link_libraries(roseClangFrontend ${CLANG_LIBRARIES})

install(FILES clang-builtin-c.h clang-builtin-cpp.hpp clang-builtin-cuda.hpp clang-builtin-opencl.h DESTINATION ${INCLUDE_INSTALL_DIR}/../clang/)

# Install rules   
install(TARGETS roseClangFrontend
        LIBRARY DESTINATION lib)
