################################################################################
# The test programs, how to run them, and on which specimens they should run.
# All information about a particular test is localized to its few contiguous
# lines in this file.
################################################################################

if (HAVE_SYS_TIME_H)

  #-----------------------------------------------------------------------------
  add_executable(proFunSIG proFunSIG.C)
  target_link_libraries(proFunSIG ROSE_DLL EDG ${link_with_libraries})

  set(proFunSIG_SPECIMENS mf1.C mf3.C mf4.C mf5.C)
  foreach(specimen ${proFunSIG_SPECIMENS})
    add_test(
      NAME pfs_${specimen}
      COMMAND proFunSIG ${CMAKE_CURRENT_SOURCE_DIR}/${specimen}
    )
  endforeach()

  #-----------------------------------------------------------------------------
  add_executable(astProcessingTestInterproceduralCFG interproceduralCFG.C)
  target_link_libraries(astProcessingTestInterproceduralCFG ROSE_DLL EDG ${link_with_libraries})

  set(interproceduralCFG_SPECIMENS test11.C test12.C test13.C test14.C)
  set(proFunSIG_SPECIMENS mf1.C mf3.C mf4.C mf5.C)
  foreach(specimen ${interproceduralCFG_SPECIMENS})
    add_test(
      NAME ipcfg_${specimen}
      COMMAND astProcessingTestInterproceduralCFG ${CMAKE_CURRENT_SOURCE_DIR}/${specimen}
    )
  endforeach()

  #-----------------------------------------------------------------------------
  add_executable(f1 tfif1.C.maintest.C)
  target_link_libraries(f1 ROSE_DLL EDG ${link_with_libraries})

  add_test(
    NAME f1_tf1.C
    COMMAND f1 ${CMAKE_CURRENT_SOURCE_DIR}/tfif1.C
  )

  #-----------------------------------------------------------------------------
  add_executable(f2 tfif2.C.maintest.C)
  target_link_libraries(f2 ROSE_DLL EDG ${link_with_libraries})

  add_test(
    NAME f2_tf2.C
    COMMAND f2 ${CMAKE_CURRENT_SOURCE_DIR}/tfif2.C
  )

  #-----------------------------------------------------------------------------
  add_executable(f3 tfif3.C.maintest.C)
  target_link_libraries(f3 ROSE_DLL EDG ${link_with_libraries})

  add_test(
    NAME f3_tf3.C
    COMMAND f3 ${CMAKE_CURRENT_SOURCE_DIR}/tfif3.C
  )

  #-----------------------------------------------------------------------------
  add_executable(f4 tfif4.C.maintest.C)
  target_link_libraries(f4 ROSE_DLL EDG ${link_with_libraries})

  add_test(
    NAME f4_tf4.C
    COMMAND f4 ${CMAKE_CURRENT_SOURCE_DIR}/tfif4.C
  )

  #-----------------------------------------------------------------------------
  add_executable(ff1 tf1.C.maintest.C)
  target_link_libraries(ff1 ROSE_DLL EDG ${link_with_libraries})

  add_test(
    NAME ff1_tf1.C
    COMMAND ff1 ${CMAKE_CURRENT_SOURCE_DIR}/tf1.C
  )

  #-----------------------------------------------------------------------------
  add_executable(ff2 tf2.C.maintest.C)
  target_link_libraries(ff2 ROSE_DLL EDG ${link_with_libraries})

  add_test(
    NAME ff2_tf2.C
    COMMAND ff2 ${CMAKE_CURRENT_SOURCE_DIR}/tf2.C
  )

  #-----------------------------------------------------------------------------
  add_executable(ff3 tf3.C.maintest.C)
  target_link_libraries(ff3 ROSE_DLL EDG ${link_with_libraries})

  add_test(
    NAME ff3_tf3.C
    COMMAND ff3 ${CMAKE_CURRENT_SOURCE_DIR}/tf3.C
  )

  #-----------------------------------------------------------------------------
  add_executable(astTraversalTest astTraversalTest.C)
  target_link_libraries(astTraversalTest ROSE_DLL EDG ${link_with_libraries})

  add_test(
    NAME astTraversalTest_input1C
    COMMAND astTraversalTest -edg:w -c ${CMAKE_CURRENT_SOURCE_DIR}/input1.C
  )

  #-----------------------------------------------------------------------------
  add_executable(strictGraphTest strictGraphTest.C)
  target_link_libraries(strictGraphTest ROSE_DLL EDG ${link_with_libraries})

  add_test(
    NAME strictGraphTest
    COMMAND strictGraphTest
  )

  #-----------------------------------------------------------------------------
  add_executable(strictGraphTest2 strictGraphTest2.C)
  target_link_libraries(strictGraphTest2 ROSE_DLL EDG ${link_with_libraries})

  add_test(
    NAME strictGraphTest2
    COMMAND strictGraphTest2
  )

  #-----------------------------------------------------------------------------
  add_executable(strictGraphTest3 strictGraphTest3.C)
  target_link_libraries(strictGraphTest3 ROSE_DLL EDG ${link_with_libraries})

  add_test(
    NAME strictGraphTest3
    COMMAND strictGraphTest3
  )

  #-----------------------------------------------------------------------------
  add_executable(smtlibParser smtlibParser.C)
  target_link_libraries(smtlibParser ROSE_DLL EDG ${link_with_libraries})

  add_test(
    NAME smtlibParser_test11.C
    COMMAND smtlibParser ${CMAKE_CURRENT_SOURCE_DIR}/test11.C
  )

  #-----------------------------------------------------------------------------
  add_executable(sourcePTP sourcePTP.C)
  target_link_libraries(sourcePTP ROSE_DLL EDG ${link_with_libraries})

  add_test(
    NAME sourcePTP_test14.C
    COMMAND sourcePTP ${CMAKE_CURRENT_SOURCE_DIR}/test14.C)

  #-----------------------------------------------------------------------------
  add_executable(processnew3Down4SgIncGraph2 processnew3Down4SgIncGraph2.C)
  target_link_libraries(processnew3Down4SgIncGraph2
    ROSE_DLL EDG ${link_with_libraries})
  set(processnew3Down4SgIncGraph2_SPECIMENS test11.C test12.C test13.C test14.C)

  foreach(specimen ${processnew3Down4SgIncGraph2_SPECIMENS})
    add_test(
      NAME pnew2_${specimen}
      COMMAND processnew3Down4SgIncGraph2 ${CMAKE_CURRENT_SOURCE_DIR}/${specimen}
    )
  endforeach()

  #-----------------------------------------------------------------------------
  add_executable(processnew3Down4SgIncGraph3 processnew3Down4SgIncGraph3.C)
  target_link_libraries(processnew3Down4SgIncGraph3
    ROSE_DLL EDG ${link_with_libraries})
  set(processnew3Down4SgIncGraph3_SPECIMENS test11.C test12.C test13.C test14.C)

  set(processnew3Down4SgIncGraph3_SPECIMENS eif.C eif2.C eif3.C eif4.C)
  foreach(specimen ${processnew3Down4SgIncGraph3_SPECIMENS})
    add_test(
      NAME pnew2_${specimen}
      COMMAND processnew3Down4SgIncGraph3 ${CMAKE_CURRENT_SOURCE_DIR}/${specimen}
    )
  endforeach()

  if (enable-binary-analysis)
    #---------------------------------------------------------------------------
    add_executable(binaryPaths binaryPaths.C)
    target_link_libraries(binaryPaths ROSE_DLL EDG ${link_with_libraries})

    set(binaryPaths_SPECIMENS t1 t2 t3 t4)
    foreach(specimen ${binaryPaths_SPECIMENS})
      add_test(
        NAME bp_${specimen}
        COMMAND binaryPaths ${CMAKE_CURRENT_SOURCE_DIR}/${specimen}
      )
    endforeach()

    #---------------------------------------------------------------------------
    add_executable(bPTP bPTP.C)
    target_link_libraries(bPTP ROSE_DLL EDG ${link_with_libraries})

    set(bPTP_SPECIMENS t1 t2 t3 t4)
    foreach(specimen ${bPTP_SPECIMENS})
      add_test(
        NAME bptp_${specimen}
        COMMAND bPTP ${CMAKE_CURRENT_SOURCE_DIR}/${specimen}
      )
    endforeach()
  endif()

  #-----------------------------------------------------------------------------
  if (enable-yices)
    add_executable(yicesParser yicesParser.C)
    target_link_libraries(yicesParser ROSE_DLL EDG ${link_with_libraries})

    set(yicesParser_SPECIMENS
      efor1.C efor2.C efor3.C eif.C eif2.C eif3.C mf1.C mf10.C mf11.C mf12.C
      mf13.C mf2.C mf3.C mf8.C mf9.C mfEx.C mfEx2.C mfEx3.C mfEx4.C mfEx6.C
      mfEx7.C mfEx8.C tf1.C tf2.C tf3.C tfif1.C tfif2.C tfif3.C tfif4.C
      whiletest.C whiletest2.C whiletest3.C
    )
    foreach(specimen ${yicesParser_SPECIMENS})
      add_test(
        NAME yp_${specimen}
        COMMAND yicesParser ${CMAKE_CURRENT_SOURCE_DIR}/${specimen}
      )
    endforeach()
  endif()

endif()

