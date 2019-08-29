if(NOT "${gfortran_version}" VERSION_EQUAL "4.0")
  add_subdirectory(LANL_POP)
  add_subdirectory(gfortranTestSuite)
endif()

set(F90_TESTCODES
  test2011_01.f90 test2011_02.f90 test2011_04.f90 test2011_06.f90
  test2011_07.f90 test2007_01.f90 test2007_02.f90 test2007_03.f90
  test2007_04.f90 test2007_05.f90 test2007_06.f90 test2007_07.f90
  test2007_08.f90 test2007_09.f90 test2007_10.f90 test2007_11.f90
  test2007_12.f90 test2007_13.f90 test2007_14.f90 test2007_15.f90
  test2007_16.f90 test2007_17.f90 test2007_19.f90 test2007_20.f90
  test2007_21.f90 test2007_22.f90 test2007_23.f90 test2007_24.f90
  test2007_25.f90 test2007_26.f90 test2007_27.f90 test2007_28.f90
  test2007_29.f90 test2007_32.f90 test2007_33.f90 test2007_34.f90
  test2007_36.f90 test2007_38.f90 test2007_39.f90 test2007_40.f90
  test2007_48.f90 test2007_49.f90 test2007_50.f90 test2007_51.f90
  test2007_52.f90 test2007_53.f90 test2007_54.f90 test2007_55.f90
  test2007_56.f90 test2007_57.f90 test2007_58.f90 test2007_59.f90
  test2007_60.f90 test2007_61.f90 test2007_62.f90 test2007_63.f90
  test2007_64.f90 test2007_65.f90 test2007_66.f90 test2007_67.f90
  test2007_68.f90 test2007_69.f90 test2007_71.f90 test2007_72.f90
  test2007_73.f90 test2007_74.f90 test2007_75.f90 test2007_76.f90
  test2007_77.f90 test2007_78.f90 test2007_79.f90 test2007_80.f90
  test2007_89.f90 test2007_90.f90 test2007_91.f90 test2007_92.f90
  test2007_93.f90 test2007_94.f90 test2007_95.f90 test2007_96.f90
  test2007_97.f90 test2007_99.f90 test2007_100.f90 test2007_101.f90
  test2007_102.f90 test2007_103.f90 test2007_104.f90 test2007_105.f90
  test2007_106.f90 test2007_107.f90 test2007_109.f90 test2007_110.f90
  test2007_111.f90 test2007_117.f90 test2007_118.f90 test2007_119.f90
  test2007_121.f90 test2007_122.f90 test2007_126.f90 test2007_127.f90
  test2007_131.f90 test2007_135.f90 test2007_136.f90 test2007_137.f90
  test2007_138.f90 test2007_139.f90 test2007_140.f90 test2007_141.f90
  test2007_143.f90 test2007_144.f90 test2007_149.f90 test2007_158.f90
  test2007_159.f90 test2007_160.f90 test2007_161.f90 test2007_165.f90
  test2007_166.f90 test2007_168.f90 test2007_169.f90 test2007_170.f90
  test2007_178.f90 test2007_179.f90 test2007_181.f90 test2007_182.f90
  test2007_190.f90 test2007_219.f90 test2007_236.f90 test2007_238.f90
  test2007_239.f90 test2007_240.f90 test2007_244.f90 test2007_248.f90
  test2007_249.f90 test2007_250.f90 test2007_254.f90 test2007_255.f90
  test2007_257.f90 test2007_262.f90 test2008_30.f90 test2009_1.f90
  test2010_01.f90 test2010_02.f90 test2010_04.f90 test2010_05.f90
  test2010_15.f90 test2010_16.f90 test2010_17.f90 test2010_18.f90
  test2010_20.f90 test2010_21.f90 test2010_22.f90 test2010_23.f90
  test2010_24.f90 test2010_25.f90 test2010_28.f90 test2010_31.f90
  test2010_32.f90 test2010_33.f90 test2010_34.f90 test2010_36.f90
  test2010_37.f90 test2010_38.f90 test2010_40.f90 test2010_41.f90
  test2010_42.f90 test2010_43.f90 test2010_44.f90 test2010_45.f90
  test2010_46.f90 test2010_47.f90 test2010_48.f90 test2010_49.f90
  test2010_50.f90 test2010_51.f90 test2010_59.f90 test2010_60.f90
  test2010_61.f90 test2010_62.f90 test2010_63.f90 test2010_64.f90
  test2010_65.f90 test2010_66.f90 test2010_67.f90 test2010_72.f90
  test2010_73.f90 test2010_78.f90 test2010_79.f90 test2010_82.f90
  test2010_84.f90 test2010_85.f90 test2010_86.f90 test2010_90.f90
  test2010_100.f90 test2010_107.f90 test2010_108.f90 test2010_109.f90
  test2010_111.f90 test2010_112.f90 test2010_113.f90 test2010_114.f90
  test2010_115.f90 test2010_116.f90 test2010_117.f90 test2010_118.f90
  test2010_120.f90 test2010_121.f90 test2010_122.f90 test2010_123.f90
  test2010_124.f90 test2010_125.f90 test2010_127.f90 test2010_128.f90
  test2010_129.f90 test2010_130.f90 test2010_131.f90 test2010_132.f90
  test2010_133.f90 test2010_134.f90 test2010_135.f90 test2010_136.f90
  test2010_137.f90 test2010_138.f90 test2010_139.f90 test2010_140.f90
  test2010_141.f90 test2010_142.f90 test2010_143.f90 test2010_144.f90
  test2010_145.f90 test2010_146.f90 test2010_147.f90 test2010_148.f90
  test2010_149.f90 test2010_150.f90 test2010_151.f90 test2010_152.f90
  test2010_153.f90 test2010_154.f90 test2010_155.f90 test2010_156.f90
  test2010_158.f90 test2010_159.f90 test2010_160.f90 test2010_162.f90
  test2010_163.f90 test2010_165.f90 test2010_166.f90 test2010_167.f90
  test2010_168.f90 test2010_169.f90 test2010_170.f90 test2010_172.f90
  test2010_173.f90 test2010_174.f90 test2010_182.f90 test2010_183.f90
  test2010_184.f90 test2011_03.f90 test2011_05.f90 test2011_08.f90
  test2011_09.f90 test2011_10.f90 test2011_12.f90 test2011_13.f90
  test2011_14.f90 test2011_15.f90 test2011_16.f90 test2011_17.f90
  test2011_18.f90 test2011_19.f90 test2011_20.f90 test2011_21.f90
  test2011_22.f90 test2011_36.f90 test2011_39.f90 test2011_40.f90
  test2011_41.f90 test2011_43.f90 test2011_44.f90 test2011_45.f90
  test2011_46.f90 test2011_50.f90 test2011_51.f90 test2011_52.f90
  test2011_53.f90 test2011_54.f90 test2011_55.f90 test2011_57.f90
  test2011_60.f90 test2011_62.f90 test2011_63.f90 test2011_64.f90
  test2011_68.f90 test2011_69.f90 test2011_71.f90 test2011_72.f90
  test2011_75.f90 test2011_76.f90 test2011_Rice_allocate-derived-type-scope.f90
  test2011_Rice_asterisk-length.f90 test2011_Rice_attrs-dimension.f90
  test2011_Rice_caseSensitive.f90 test2011_Rice_char-array-decl-asterisk.f90
  test2011_Rice_char-kind-decl.f90 test2011_Rice_constructor-initializer.f90
  test2011_Rice_data-implied-do.f90
  test2011_Rice_data-implied-do-object-list.f90
  test2011_Rice_derived-type-2d-component.f90
  test2011_Rice_derived-type-3d-component.f90
  test2011_Rice_derived-type-array.f90 test2011_Rice_elseif.f90
  test2011_Rice_empty-string-constant.f90 test2011_Rice_eq.f90
  test2011_Rice_explicit-shape1.f90 test2011_Rice_explicit-shape2.f90
  test2011_Rice_forward-decl-func-type.f90
  test2011_Rice_forward-ref-derived-type.f90
  test2011_Rice_func-char-len.f90 test2011_Rice_funcdecl-1.f90
  test2011_Rice_function-implicit-result-1.f90
  test2011_Rice_function-implicit-result-2.f90
  test2011_Rice_function-no-result-1.f90 test2011_Rice_function-no-result-2.f90
  test2011_Rice_function-result-1.f90 test2011_Rice_function-result-2.f90
  test2011_Rice_function-where-var-expected.f90 test2011_Rice_ifConstruct.f90
  test2011_Rice_ifStmt.f90 test2011_Rice_interface.f90 test2011_Rice_if-stop.f90
  test2011_Rice_inquire-in-ifthen.f90 test2011_Rice_intent.f90
  test2011_Rice_io-implied-do-with-stride.f90
  test2011_Rice_keyword-arg-extra-paren-2.f90
  test2011_Rice_keyword-arg-extra-paren.f90 test2011_Rice_lost-kindtype-1.f90
  test2011_Rice_lost-kindtype-2.f90 test2011_Rice_multi-char-decl.f90
  test2011_Rice_nullify.f90 test2011_Rice_pause-no-stop-code.f90
  test2011_Rice_pointer.f90 test2011_Rice_pointer-after-dimension.f90
  test2011_Rice_pointer-init.f90
  test2011_Rice_pointer-to-forward-ref-derived-type.f90
  test2011_Rice_public.f90 test2011_Rice_return-with-expr.f90
  test2011_Rice_singleton-array-constructor.f90
  test2011_Rice_stop-no-stop-code.f90 test2011_Rice_unary-plus-minus.f90
  test2011_Rice_use-only-rename.f90 test2011_Rice_use-rename.f90
  test2011_Rice_vector-constructor-1.f90 test2011_Rice_vector-constructor-2.f90
  test2011_Rice_vector-constructor-3.f90 test2011_Rice_vector-constructor-4.f90
  test2011_Rice_write-no-output-item-list.f90
  test2011_Rice_wrong-func-type.f90 test2011_Rice_wrong-func-type-2.f90
  test2011_Rice_wrong-func-type-3.f90
  #EXPECTED FAILURE: test2011_Rice_A-cant-assert-keyword-nonnull.f90
  #EXPECTED FAILURE: test2011_Rice_B-cant-assert-keyword-nonnull.f90
  #EXPECTED FAILURE: test2011_Rice_C-cant-assert-keyword-nonnull.f90
  #EXPECTED TO FAILURE: test2011_Rice_lost-func-return-type.f90
  test2019_derived_type.f90
  test2019_write_io_control_spec.f90
  test2019_recursive_function.f90
  test2019_derived_type_name_conflict.f90
  test2019_rewind.f90
  test2019_renameList.f90
  test2019_designator.f90
  test2019_contiguous_attr.f90
  test2019_doconstruct.f90 
  canonicalExample.f90)

if("${gfortran_version}" VERSION_LESS "4.5")
  # DQ (4/10/2011): If it is not gfortran 4.5, then add this test code (these fail syntax checking with 4.5).
  list(APPEND F90_TESTCODES
    test2010_35.f90 test2011_Rice_funcdecl-4.f90 
    #EXPECTED FAILURE: test2011_37.f90
  )
else()
  # phlin (as 08/22/2012):
  # test case for ROSE-bugs #597
  list(APPEND F90_TESTCODES test2012_SavedCommonBlock.f90)
endif()

if(NOT "${gfortran_version}" VERSION_EQUAL "4.0")
   # These are tests that fail using the gfrotran version 4.0 compiler (but pass with all other versions of gfortran).
   list(APPEND F90_TESTCODES test2007_263.f90)
endif()

# DQ (2/1/2011): These test codes will not pass the initial syntax checking with the GNU gfortran version 4.4.x compiler.
if("${gfortran_version}" VERSION_LESS "4.4")
   list(APPEND F90_TESTCODES test2010_164.f90)
endif()

set(F77_TESTCODES
  test2007_124.f test2007_125.f test2007_128.f test2007_129.f test2007_132.f
  test2007_145.f test2007_148.f test2007_150.f test2007_151.f test2007_153.f
  test2007_154.f test2007_155.f test2007_156.f test2007_157.f test2007_162.f
  test2007_163.f test2007_164.f test2007_175.f test2007_183.f test2007_184.f
  test2007_185.f test2007_186.f test2007_187.f test2007_188.f test2007_189.f
  test2007_191.f test2007_192.f test2007_193.f test2007_194.f test2007_195.f
  test2007_196.f test2007_197.f test2007_198.f test2007_200.f test2007_201.f
  test2007_202.f test2007_204.f test2007_205.f test2007_206.f test2007_207.f
  test2007_208.f test2007_209.f test2007_211.f test2007_212.f test2007_213.f
  test2007_214.f test2007_215.f test2007_216.f test2007_217.f test2007_218.f
  test2007_220.f test2007_221.f test2007_222.f test2007_223.f test2007_224.f
  test2007_226.f test2007_227.f test2007_228.f test2007_229.f test2007_230.f
  test2007_231.f test2007_232.f test2007_234.f test2007_242.f test2007_243.f
  test2007_245.f test2007_246.f test2007_247.f test2007_251.f test2007_253.f
  test2007_256.f test2007_258.f test2007_259.f test2007_260.f test2007_261.f
  test2010_29.f test2010_30.f test2010_68.f test2010_69.f
  test2011_Rice_char-star1-function.f test2011_Rice_char-star2-function.f
  test2011_Rice_computed-goto.f test2011_Rice_continuation-spits-token.f
  test2011_Rice_initial-tab.f test2011_Rice_intrinsic-mixup.f
  test2011_Rice_lost-func-return-type.f test2011_Rice_shared_continue.f
  test2011_Rice_shared_label_stmt.f
  jacobi.f default.f flush.f exmpaleA221f.f exampleA251f.f schedule.f
  testNewOFP.f test2007_86.f test2007_142.f test2007_147.f test2007_199.f
  test2007_203.f test2007_225.f test2007_235.f test2007_241.f test2007_252.f
  test2008_02.f test2008_03.f test2010_52.f test2010_53.f test2010_54.f
  test2010_70.f test2010_105.f test2010_157.f test2011_94.f)

set(F03_TESTCODES
  test2007_30.f03 test2007_31.f03 test2007_35.f03 test2007_37.f03
  test2007_41.f03 test2007_42.f03 test2007_43.f03 test2007_44.f03
  test2007_45.f03 test2007_47.f03 test2007_98.f03 test2007_112.f03
  test2007_113.f03 test2007_114.f03 test2007_115.f03 test2007_116.f03
  test2007_120.f03 test2007_123.f03 test2007_130.f03 test2007_133.f03
  test2007_134.f03 test2007_174.f03 test2007_176.f03 test2010_176.f03
  test2010_177.f03 test2010_178.f03 test2010_179.f03 test2010_180.f03
  test2010_181.f03 test2011_24.f03 test2011_25.f03 test2011_27.f03
  test2011_28.f03 test2011_29.f03 test2011_30.f03 test2011_31.f03
  test2011_32.f03 test2011_33.f03 test2011_34.f03 test2011_35.f03
  test2011_Rice_attrs-pointer-array.f03 test2011_Rice_attrs-value-intent.f03
  test2011_Rice_bindc-name-2.f03 test2011_Rice_bindc-name-sub.f03
  test2011_Rice_bindc-name.f03 test2011_Rice_bindc.f03 test2007_172.f03
  test2007_173.f03 test2010_77.f03 test2010_106.f03 test2010_171.f03
  test2011_74.f03)

set(ROSE_FLAGS -rose:verbose 0 -rose:detect_dangling_pointers 2
               -I${CMAKE_CURRENT_SOURCE_DIR})

function(fortran_test input_file)
  add_test(
    NAME ${input_file}_translation
    COMMAND testTranslator ${ROSE_FLAGS} ${FORTRAN_FLAGS}
            -c ${CMAKE_CURRENT_SOURCE_DIR}/${input_file})
  add_test(
    NAME ${input_file}_graph_generation
    COMMAND testGraphGeneration ${ROSE_FLAGS} ${FORTRAN_FLAGS}
            -c ${CMAKE_CURRENT_SOURCE_DIR}/${input_file})
  add_test(
    NAME ${input_file}_token_generation
    COMMAND testTokenGeneration ${ROSE_FLAGS} ${FORTRAN_FLAGS}
            -c ${CMAKE_CURRENT_SOURCE_DIR}/${input_file})
  set_tests_properties(${input_file}_graph_generation
    PROPERTIES DEPENDS ${input_file}_translation)
  set_tests_properties(${input_file}_token_generation
    PROPERTIES DEPENDS ${input_file}_graph_generation)
endfunction()

set(FORTRAN_FLAGS -rose:f77)
foreach(F77_test ${F77_TESTCODES})
  fortran_test(${F77_test})
endforeach()

set(FORTRAN_FLAGS -rose:f90)
foreach(F90_test ${F90_TESTCODES})
  fortran_test(${F90_test})
endforeach()

set(FORTRAN_FLAGS -rose:skip_syntax_check -rose:skipfinalCompileStep
                  -rose:f2003)
foreach(F03_test ${F03_TESTCODES})
  fortran_test(${F03_test})
endforeach()
