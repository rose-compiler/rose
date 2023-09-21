execute_process(COMMAND sh -c "dirname -z `which gprbuild`" OUTPUT_VARIABLE GPR_SRC)
message("GPR pah: ${GPR_SRC}")
add_custom_target(asis_adapter ALL
                  COMMAND sh -c "gprls -v"
                  COMMAND sh -c "gprbuild -p -aP ${GPR_SRC}/../share/gpr -P${CMAKE_CURRENT_SOURCE_DIR}/asis_adapter.gpr -v --relocate-build-tree"
                  COMMENT "building asis_adapter by gprbuild")
