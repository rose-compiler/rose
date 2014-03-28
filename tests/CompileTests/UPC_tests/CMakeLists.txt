set(files_to_test
  threads_in_expression.upc barrier.upc consistency.upc hello.upc
  fence.upc forall_1.upc forall_affinity.upc forall_continue.upc
  lock.upc macro_names.upc max_block_size.upc mythread.upc notify_wait.upc
  restrict.upc shared.upc shared_2.upc sharedArrayInit.upc sharedInit.upc
  strict.upc threads.upc threads_2.upc typedef.upc
  upc_all_alloc_ex.upc upc_all_broadcast_ex.upc
  upc_all_lock_alloc_ex.upc upc_all_permute_ex.upc upc_all_prefix_reduceL_ex.upc
  upc_all_reduceL_ex.upc upc_all_scatter_ex.upc upc_all_exchange_ex_dynamic.upc
  upc_all_gather_all_ex_dynamic.upc upc_all_gather_ex_dynamic.upc
  upc_all_scatter_ex_dynamic.upc bupc_test.upc istream.upc test2010_01.upc
  test2010_02.upc test2011_01.upc test2011_02.upc test2011_03.upc
  test2011_04.upc test2011_05.upc test2011_06.upc test2011_07.upc
  test2011_08.upc test2011_09.upc test2011_10.upc test2011_11.upc
  test2011_14.upc static_upc_all_exchange_ex.upc
  static_upc_all_gather_all_ex.upc static_upc_all_gather_ex.upc
  static_test2011_12.upc static_test2011_13.upc)

# Liao (6/11/2008) --edg:upc and --edg:restrict are not required since ROSE can add them internally based on the file suffix (.upc) , no backend compilation since GCC does not recognize UPC .
if(enable-new-edg-interface)
  set(baseline_flags -rose:verbose 0 --edg:no_warnings --edg:c99
                     -rose:exit_after_parser -rose:skipfinalCompileStep
                     -rose:upc_threads 1)
else()
  set(baseline_flags -rose:verbose 0 --edg:no_warnings
                     -rose:skipfinalCompileStep -rose:upc_threads 1)
endif()


set(ROSE_FLAGS -rose:UPC ${baseline_flags})

foreach(file_to_test ${files_to_test})
  compile_test(${file_to_test} TRUE)
endforeach()

# These tests are currently failing for me with the following output:
# Command-line error: invalid option: --upc++
# The tests also seem to be commented out of the Makefile, so I am
# commenting them out here as well.

#set(cxx_files_to_test
#  test2004_22.upc test2011_01.upc test2011_03.upc test2011_04.upc
#  test2011_05.upc test2011_06.upc test2011_07.upc)
#
#set(ROSE_FLAGS -rose:UPCxx ${baseline_flags})
#
#foreach(file_to_test ${cxx_files_to_test})
#  get_filename_component(test_name ${file_to_test} NAME_WE)
#  add_test(
#    NAME ${test_name}cxx
#    COMMAND ${translator}
#      ${ROSE_FLAGS} -c ${CMAKE_CURRENT_SOURCE_DIR}/${file_to_test})
#endforeach()
