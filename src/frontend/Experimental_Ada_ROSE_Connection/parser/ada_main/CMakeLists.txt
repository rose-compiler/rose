execute_process(COMMAND sh -c "dirname -z `which gprbuild`" OUTPUT_VARIABLE GPR_SRC)
# Make sure GPR_SRC is not an empty string 
if(NOT GPR_SRC)
  message(FATAL_ERROR "gprbuild is required for Ada support but was not found\n") 
else() 
  message("GPR path: ${GPR_SRC}")
endif()

# Custom target to run gprbuild manually
add_custom_target(run_parser_adapter ALL
    COMMAND gprbuild
        -p
        -aP ${CMAKE_CURRENT_SOURCE_DIR}/../asis_adapter
        -P${CMAKE_CURRENT_SOURCE_DIR}/ada_main.gpr
        -v
        --relocate-build-tree=.
        run_parser_adapter
    COMMENT "Building ada_main program with gprbuild"
)

#set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "obj")