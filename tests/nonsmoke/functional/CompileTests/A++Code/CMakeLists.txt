if(with-ROSE_LONG_MAKE_CHECK_RULE)
  set(files_to_test
    test_bool.C
    data_hash.C
    abstract_op.C
    error_checking.C
    index_operator_notemplates.C
    lazy_statement.C
    new_operators_notemplates.C
    descriptor_cache.C
    lazy_aggregate.C
    lazy_task.C
    static_initialization.C
    comm_man.C
    descriptor_notemplates.C
    lazy_expression.C
    lazy_taskset.C
    index.C
    lazy_operand.C
    lazyarray.C
    where.C
    partitioning.C
    domain.C
    array_notemplates.C
  )
else()
  set(files_to_test
    test_bool.C
    data_hash.C
  )
endif()

foreach(file_to_test ${files_to_test})
  compile_test(${file_to_test} AXXTEST)
endforeach()
