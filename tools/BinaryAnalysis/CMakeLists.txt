if(ENABLE-BINARY-ANALYSIS)
  include_directories(. ${ROSE_INCLUDES})

  add_library(bat SHARED batSupport.C)
  set_target_properties(bat PROPERTIES OUTPUT_NAME "batSupport")
  target_link_libraries(bat ROSE_DLL)
  install(TARGETS bat DESTINATION ${ROSE_LIB_DIR_NAME})

  add_executable(bat-ana bat-ana.C)
  target_link_libraries(bat-ana bat ROSE_DLL)
  install(TARGETS bat-ana DESTINATION bin)

  add_executable(bat-cc bat-cc.C)
  target_link_libraries(bat-cc bat ROSE_DLL)
  install(TARGETS bat-cc DESTINATION bin)

  add_executable(bat-cfg bat-cfg.C)
  target_link_libraries(bat-cfg bat ROSE_DLL)
  install(TARGETS bat-cfg DESTINATION bin)

  add_executable(bat-cg bat-cg.C)
  target_link_libraries(bat-cg bat ROSE_DLL)
  install(TARGETS bat-cg DESTINATION bin)

  add_executable(bat-check-execution bat-check-execution.C)
  target_link_libraries(bat-check-execution bat ROSE_DLL)
  install(TARGETS bat-check-execution DESTINATION bin)

  add_executable(bat-container bat-container.C)
  target_link_libraries(bat-container bat ROSE_DLL)
  install(TARGETS bat-container DESTINATION bin)

  add_executable(bat-dataflow bat-dataflow.C)
  target_link_libraries(bat-dataflow bat ROSE_DLL)
  install(TARGETS bat-dataflow DESTINATION bin)

  add_executable(bat-debug-semantics bat-debug-semantics.C)
  target_link_libraries(bat-debug-semantics bat ROSE_DLL)
  install(TARGETS bat-debug-semantics DESTINATION bin)

  add_executable(bat-delta-bijection bat-delta-bijection.C)
  target_link_libraries(bat-delta-bijection bat ROSE_DLL)
  install(TARGETS bat-delta-bijection DESTINATION bin)

  add_executable(bat-dis bat-dis.C)
  target_link_libraries(bat-dis bat ROSE_DLL)
  install(TARGETS bat-dis DESTINATION bin)

  add_executable(bat-dwarf-lines bat-dwarf-lines.C)
  target_link_libraries(bat-dwarf-lines bat ROSE_DLL)
  install(TARGETS bat-dwarf-lines DESTINATION bin)

  add_executable(bat-flir-ascribe bat-flir-ascribe.C)
  target_link_libraries(bat-flir-ascribe bat ROSE_DLL)
  install(TARGETS bat-flir-ascribe DESTINATION bin)

  add_executable(bat-flir-insert bat-flir-insert.C)
  target_link_libraries(bat-flir-insert bat ROSE_DLL)
  install(TARGETS bat-flir-insert DESTINATION bin)

  add_executable(bat-flir-lib bat-flir-lib.C)
  target_link_libraries(bat-flir-lib bat ROSE_DLL)
  install(TARGETS bat-flir-lib DESTINATION bin)

  add_executable(bat-insnfreq bat-insnfreq.C)
  target_link_libraries(bat-insnfreq bat ROSE_DLL)
  install(TARGETS bat-insnfreq DESTINATION bin)

  add_executable(bat-insnfreq-cmp bat-insnfreq-cmp.C)
  target_link_libraries(bat-insnfreq-cmp bat ROSE_DLL)
  install(TARGETS bat-insnfreq-cmp DESTINATION bin)

  add_executable(bat-entropy bat-entropy.C)
  target_link_libraries(bat-entropy bat ROSE_DLL)
  install(TARGETS bat-entropy DESTINATION bin)

  add_executable(bat-lsb bat-lsb.C)
  target_link_libraries(bat-lsb bat ROSE_DLL)
  install(TARGETS bat-lsb DESTINATION bin)

  add_executable(bat-lsd bat-lsd.C)
  target_link_libraries(bat-lsd bat ROSE_DLL)
  install(TARGETS bat-lsd DESTINATION bin)

  add_executable(bat-lsf bat-lsf.C)
  target_link_libraries(bat-lsf bat ROSE_DLL)
  install(TARGETS bat-lsf DESTINATION bin)

  add_executable(bat-lsv bat-lsv.C)
  target_link_libraries(bat-lsv bat ROSE_DLL)
  install(TARGETS bat-lsv DESTINATION bin)

  add_executable(bat-mem bat-mem.C)
  target_link_libraries(bat-mem bat ROSE_DLL)
  install(TARGETS bat-mem DESTINATION bin)

  add_executable(bat-native-trace bat-native-trace.C)
  target_link_libraries(bat-native-trace bat ROSE_DLL)
  install(TARGETS bat-native-trace DESTINATION bin)

  add_executable(bat-pointers bat-pointers.C)
  target_link_libraries(bat-pointers bat ROSE_DLL)
  install(TARGETS bat-pointers DESTINATION bin)

  add_executable(bat-prop bat-prop.C)
  target_link_libraries(bat-prop bat ROSE_DLL)
  install(TARGETS bat-prop DESTINATION bin)

  add_executable(bat-scan-magic bat-scan-magic.C)
  target_link_libraries(bat-scan-magic bat ROSE_DLL)
  install(TARGETS bat-scan-magic DESTINATION bin)

  add_executable(bat-similar-functions bat-similar-functions.C)
  target_link_libraries(bat-similar-functions bat ROSE_DLL)
  install(TARGETS bat-similar-functions DESTINATION bin)

  add_executable(bat-simplify bat-simplify.C)
  target_link_libraries(bat-simplify bat ROSE_DLL)
  install(TARGETS bat-simplify DESTINATION bin)

  add_executable(bat-stack-deltas bat-stack-deltas.C)
  target_link_libraries(bat-stack-deltas bat ROSE_DLL)
  install(TARGETS bat-stack-deltas DESTINATION bin)

  add_executable(bat-to-c bat-to-c.C)
  target_link_libraries(bat-to-c bat ROSE_DLL)
  install(TARGETS bat-to-c DESTINATION bin)

  add_executable(bat-trace bat-trace.C)
  target_link_libraries(bat-trace bat ROSE_DLL)
  install(TARGETS bat-trace DESTINATION bin)

  add_executable(bat-var bat-var.C)
  target_link_libraries(bat-var bat ROSE_DLL)
  install(TARGETS bat-var DESTINATION bin)

  add_executable(bat-xml-to-json bat-xml-to-json.C)
  target_link_libraries(bat-xml-to-json bat ROSE_DLL)
  install(TARGETS bat-xml-to-json DESTINATION bin)

  install(FILES batSupport.h DESTINATION ${INCLUDE_INSTALL_DIR})

endif()
