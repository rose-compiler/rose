add_executable(roseTestMerge testMerge.C)
target_link_libraries(roseTestMerge ROSE_DLL EDG ${link_with_libraries})

# This is a shortened list that tests more quickly.
set(TESTCODES
  mergeTest_01.C mergeTest_02.C mergeTest_03.C mergeTest_04.C mergeTest_05.C
  mergeTest_06.C mergeTest_08.C mergeTest_10.C mergeTest_11.C
  mergeTest_12.C mergeTest_13.C mergeTest_14.C mergeTest_15.C mergeTest_16.C
  mergeTest_17.C mergeTest_18.C mergeTest_19.C mergeTest_20.C mergeTest_21.C
  mergeTest_22.C mergeTest_23.C mergeTest_24.C mergeTest_25.C mergeTest_26.C
  mergeTest_27.C mergeTest_28.C mergeTest_29.C mergeTest_30.C mergeTest_31.C
  mergeTest_32.C mergeTest_33.C mergeTest_34.C mergeTest_35.C mergeTest_36.C
  mergeTest_37.C mergeTest_38.C mergeTest_43.C mergeTest_44.C mergeTest_46.C
  mergeTest_47.C mergeTest_48.C mergeTest_50.C mergeTest_51.C mergeTest_52.C
  mergeTest_53.C mergeTest_54.C mergeTest_55.C mergeTest_56.C mergeTest_57.C
  mergeTest_58.C mergeTest_60.C mergeTest_61.C mergeTest_62.C mergeTest_63.C
  mergeTest_64.C mergeTest_65.C mergeTest_66.C mergeTest_67.C mergeTest_68.C
  mergeTest_69.C mergeTest_70.C mergeTest_71.C mergeTest_73.C mergeTest_74.C
  mergeTest_76.C mergeTest_77.C mergeTest_78.C mergeTest_79.C mergeTest_80.C
  mergeTest_81.C mergeTest_82.C mergeTest_83.C mergeTest_84.C mergeTest_85.C
  mergeTest_86.C mergeTest_87.C mergeTest_88.C mergeTest_91.C mergeTest_92.C
  mergeTest_94.C mergeTest_95.C mergeTest_96.C mergeTest_97.C mergeTest_100.C
  mergeTest_101.C mergeTest_102.C mergeTest_103.C mergeTest_104.C mergeTest_105.C
  mergeTest_106.C mergeTest_107.C mergeTest_110.C mergeTest_111.C mergeTest_112.C
  mergeTest_114.C mergeTest_115.C mergeTest_116.C mergeTest_118.C mergeTest_119.C
  mergeTest_120.C mergeTest_121.C mergeTest_128.C mergeTest_129.C mergeTest_130.C
  mergeTest_131.C mergeTest_133.C mergeTest_134.C mergeTest_90.C
  mergeTest_124.C)

set(ROSE_FLAGS --edg:no_warnings -w --edg:restrict -rose:ast:merge)

foreach(file_to_test ${TESTCODES})
  add_test(
    NAME ${file_to_test}
    COMMAND roseTestMerge ${ROSE_FLAGS}
      -c ${CMAKE_CURRENT_SOURCE_DIR}/${file_to_test})
endforeach()
