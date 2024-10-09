execute_process(COMMAND sh -c "dirname -z `which gprbuild`" OUTPUT_VARIABLE GPR_SRC)
# Make sure GPR_SRC is not an empty string 
if(NOT GPR_SRC)
  message(FATAL_ERROR "gprbuild is required for Ada support but was not found\n") 
else() 
  message("GPR path: ${GPR_SRC}")
endif()

# Define the output file from the gprbuild command
set(ASIS_ADAPTER_LIB "${CMAKE_CURRENT_BINARY_DIR}/lib/libasis_adapter.so")

# Use add_custom_command to produce the library
add_custom_command(
  OUTPUT ${ASIS_ADAPTER_LIB}
  COMMAND sh -c "gprls -v"
  COMMAND sh -c "gprbuild -p -aP ${GPR_SRC}/../share/gpr -P${CMAKE_CURRENT_SOURCE_DIR}/asis_adapter.gpr -v --relocate-build-tree"
  COMMENT "building asis_adapter by gprbuild"
  VERBATIM
)

# Create a custom target that depends on the library being built
add_custom_target(asis_adapter ALL DEPENDS ${ASIS_ADAPTER_LIB})

# Add rule for libasis_adapter.so when the user calls "make install" 
install(FILES ${ASIS_ADAPTER_LIB} DESTINATION lib)

