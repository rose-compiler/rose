if(enable-binary-analysis)

  add_executable(rose-dump-memory dumpMemory.C)
  target_link_libraries(rose-dump-memory ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS rose-dump-memory DESTINATION bin)

  add_executable(rose-linear-disassemble linearDisassemble.C)
  target_link_libraries(rose-linear-disassemble ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS rose-linear-disassemble DESTINATION bin)

  add_executable(rose-recursive-disassemble recursiveDisassemble.C)
  target_link_libraries(rose-recursive-disassemble ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS rose-recursive-disassemble DESTINATION bin)

  add_executable(rose-native-execution-trace nativeExecutionTrace.C)
  target_link_libraries(rose-native-execution-trace ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS rose-native-execution-trace DESTINATION bin)

  add_executable(rose-check-execution-addresses checkExecutionAddresses.C)
  target_link_libraries(rose-check-execution-addresses ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS rose-check-execution-addresses DESTINATION bin)

  add_executable(rose-magic-scanner magicScanner.C)
  target_link_libraries(rose-magic-scanner ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS rose-magic-scanner DESTINATION bin)

  add_executable(rose-simulate simulate.C)
  target_link_libraries(rose-simulate ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS rose-simulate DESTINATION bin)

  add_executable(rose-string-decoder stringDecoder.C)
  target_link_libraries(rose-string-decoder ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS rose-string-decoder DESTINATION bin)

  add_executable(rose-find-dead-code findDeadCode.C)
  target_link_libraries(rose-find-dead-code ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS rose-find-dead-code DESTINATION bin)

  add_executable(rose-symbolic-simplifier symbolicSimplifier.C)
  target_link_libraries(rose-symbolic-simplifier ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS rose-symbolic-simplifier DESTINATION bin)

  add_executable(rose-binary-to-source binaryToSource.C)
  target_link_libraries(rose-binary-to-source ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS rose-binary-to-source DESTINATION bin)

  add_executable(rose-trace trace.C)
  target_link_libraries(rose-trace ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS rose-trace DESTINATION bin)

  add_executable(rose-generate-paths generatePaths.C)
  target_link_libraries(rose-generate-paths ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS rose-generate-paths DESTINATION bin)

  add_executable(rose-max-bijection maxBijection.C)
  target_link_libraries(rose-max-bijection ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS rose-max-bijection DESTINATION bin)

  add_executable(rose-debug-semantics debugSemantics.C)
  target_link_libraries(rose-debug-semantics ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS rose-debug-semantics DESTINATION bin)

  add_executable(rose-x86-call-targets x86-call-targets.C)
  target_link_libraries(rose-x86-call-targets ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS rose-x86-call-targets DESTINATION bin)

  add_executable(rose-rxml rxml.C)
  target_link_libraries(rose-rxml ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS rose-rxml DESTINATION bin)

  add_executable(rose-dwarf-lines dwarf-lines.C)
  target_link_libraries(rose-dwarf-lines ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS rose-dwarf-lines DESTINATION bin)

  add_executable(rose-xml2json xml2json.C)
  target_link_libraries(rose-xml2json ROSE_DLL ${Boost_LIBRARIES})
  install(TARGETS rose-xml2json DESTINATION bin)
  
endif()
