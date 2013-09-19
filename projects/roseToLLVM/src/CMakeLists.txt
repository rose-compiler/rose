
include_directories(${with-llvm} ${ROSE_SOURCE_DIR}/rosetollvm/include)
add_subdirectory(rosetollvm)

add_executable(rose2llvm translator.cpp)
target_link_libraries(rose2llvm rosetollvm)
