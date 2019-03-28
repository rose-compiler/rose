if(enable-binary-analysis)

  # dumpMemory
  add_executable(dumpMemory dumpMemory.C)
  target_link_libraries(dumpMemory ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS dumpMemory DESTINATION bin)

  # linearDisassemble
  add_executable(linearDisassemble linearDisassemble.C)
  target_link_libraries(linearDisassemble ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS linearDisassemble DESTINATION bin)

  # recursiveDisassemble
  add_executable(recursiveDisassemble recursiveDisassemble.C)
  target_link_libraries(recursiveDisassemble ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS recursiveDisassemble DESTINATION bin)

  # nativeExecutionTrace
  add_executable(nativeExecutionTrace nativeExecutionTrace.C)
  target_link_libraries(nativeExecutionTrace ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS nativeExecutionTrace DESTINATION bin)

  # checkExecutionAddresses
  add_executable(checkExecutionAddresses checkExecutionAddresses.C)
  target_link_libraries(checkExecutionAddresses ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS checkExecutionAddresses DESTINATION bin)

  # magicScanner
  add_executable(magicScanner magicScanner.C)
  target_link_libraries(magicScanner ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS magicScanner DESTINATION bin)

  # simulate
  add_executable(simulate simulate.C)
  target_link_libraries(simulate ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS simulate DESTINATION bin)

  # stringDecoder
  add_executable(stringDecoder stringDecoder.C)
  target_link_libraries(stringDecoder ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS stringDecoder DESTINATION bin)

  # findDeadCode
  add_executable(findDeadCode findDeadCode.C)
  target_link_libraries(findDeadCode ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS findDeadCode DESTINATION bin)

  # symbolicSimplifier
  add_executable(symbolicSimplifier symbolicSimplifier.C)
  target_link_libraries(symbolicSimplifier ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS symbolicSimplifier DESTINATION bin)

  # binaryToSource
  add_executable(binaryToSource binaryToSource.C)
  target_link_libraries(binaryToSource ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS binaryToSource DESTINATION bin)

  # trace
  add_executable(trace trace.C)
  target_link_libraries(trace ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS trace DESTINATION bin)

  # generatePaths
  add_executable(generatePaths generatePaths.C)
  target_link_libraries(generatePaths ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS generatePaths DESTINATION bin)

  # maxBijection
  add_executable(maxBijection maxBijection.C)
  target_link_libraries(maxBijection ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS maxBijection DESTINATION bin)

  # debugSemantics
  add_executable(debugSemantics debugSemantics.C)
  target_link_libraries(debugSemantics ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS debugSemantics DESTINATION bin)

  # x86-call-targets
  add_executable(x86-call-targets x86-call-targets.C)
  target_link_libraries(x86-call-targets ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS x86-call-targets DESTINATION bin)

  # rxml
  add_executable(rxml rxml.C)
  target_link_libraries(rxml ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS rxml DESTINATION bin)

  # dwarf-lines
  add_executable(dwarf-lines dwarf-lines.C)
  target_link_libraries(dwarf-lines ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS dwarf-lines DESTINATION bin)

  # xml2json
  add_executable(xml2json xml2json.C)
  target_link_libraries(xml2json ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS xml2json DESTINATION bin)
  
endif()
