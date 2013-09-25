
add_custom_command(
  OUTPUT MPICallListHelper.h
  COMMAND ${PERL_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/mpiMapGenerator
)

add_executable(SPINSlice SPINSlice.C MPICallListHelper.h)
target_link_libraries(SPINSlice ROSE_DLL ${Boost_LIBRARIES})

