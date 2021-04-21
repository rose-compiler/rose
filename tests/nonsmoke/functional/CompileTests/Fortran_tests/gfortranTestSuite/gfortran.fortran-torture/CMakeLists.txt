set(F90_TESTCODES
  actual.f90 allocate.f90 bergervoet2.f90 complex_1.f90 contained_1.f90
  contained_2.f90 contained_3.f90 contained_4.f90 contained_5.f90 convert.f90
  defined_type_2.f90 dummyfn.f90 empty.f90 empty_interface_1.f90 emptyif-1.f90
  emptyif.f90 fnresvar.f90 gen_interf.f90 implicit_2.f90 inline_1.f90
  inquiry_1.f90 io_end.f90 mloc.f90 module_common.f90 module_expr.f90
  module_proc.f90 module_result.f90 name_clash.f90 named_args.f90
  named_args_2.f90 nested.f90 parameter_1.f90 parameter_3.f90 pr26806.f90
  pr32417.f90 pr41654.f90 pr45634.f90 shape_reshape.f90 strparm_1.f90
  transfer-1.f90 write.f90 20080806-1.f90 ambig.f90 defined_type_1.f90
  defined_type_3.f90 implicit.f90 implicit_1.f90 logical-1.f90 pr30147.f90
  pr36078.f90 arrayio.f90 pr33276.f90 pr40413.f90 pr42781.f90 pr45598.f90
  vrp_1.f90)

set(F77_FIXED_FORMAT_TESTCODES pr32583.f noncontinuation_1.f)

set(ROSE_FLAGS -rose:verbose 0 -rose:skip_syntax_check
               -rose:skipfinalCompileStep -rose:detect_dangling_pointers 1)

foreach(file_to_test ${F90_TESTCODES})
  add_test(
    NAME GFORTRANSUITE_${file_to_test}
    COMMAND testTranslator ${ROSE_FLAGS} -rose:f90 -c
            ${CMAKE_CURRENT_SOURCE_DIR}/${file_to_test})
  set_tests_properties(GFORTRANSUITE_${file_to_test} PROPERTIES LABELS GFORTRANSUITE)
endforeach()

foreach(file_to_test ${F77_FIXED_FORMAT_TESTCODES})
  add_test(
    NAME GFORTRANSUITE_${file_to_test}
    COMMAND testTranslator ${ROSE_FLAGS} -rose:f77 -c
            ${CMAKE_CURRENT_SOURCE_DIR}/${file_to_test})
  set_tests_properties(GFORTRANSUITE_${file_to_test} PROPERTIES LABELS GFORTRANSUITE)
endforeach()
