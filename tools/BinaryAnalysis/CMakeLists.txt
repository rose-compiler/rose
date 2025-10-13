add_library(bat SHARED batSupport.C)
set_target_properties(bat PROPERTIES OUTPUT_NAME "batSupport")
target_include_directories(bat PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>
)
target_link_libraries(bat PUBLIC Rose::rose)

# Export bat library for external consumption
install(TARGETS bat
  EXPORT RoseTargets
  DESTINATION ${ROSE_LIB_DIR_NAME})
install(FILES batSupport.h DESTINATION ${INCLUDE_INSTALL_DIR})

if(ENABLE_BINARY_ANALYSIS)
  add_executable(bat-ana bat-ana.C)
  target_link_libraries(bat-ana PRIVATE bat)
  install(TARGETS bat-ana DESTINATION bin)

  add_executable(bat-cc bat-cc.C)
  target_link_libraries(bat-cc PRIVATE bat)
  install(TARGETS bat-cc DESTINATION bin)

  add_executable(bat-cfg bat-cfg.C)
  target_link_libraries(bat-cfg PRIVATE bat)
  install(TARGETS bat-cfg DESTINATION bin)

  add_executable(bat-cg bat-cg.C)
  target_link_libraries(bat-cg PRIVATE bat)
  install(TARGETS bat-cg DESTINATION bin)

  add_executable(bat-check-execution bat-check-execution.C)
  target_link_libraries(bat-check-execution PRIVATE bat)
  install(TARGETS bat-check-execution DESTINATION bin)

  add_executable(bat-container bat-container.C)
  target_link_libraries(bat-container PRIVATE bat)
  install(TARGETS bat-container DESTINATION bin)

  add_executable(bat-dataflow bat-dataflow.C)
  target_link_libraries(bat-dataflow PRIVATE bat)
  install(TARGETS bat-dataflow DESTINATION bin)

  add_executable(bat-ddg bat-ddg.C)
  target_link_libraries(bat-ddg PRIVATE bat)
  install(TARGETS bat-ddg DESTINATION bin)

  add_executable(bat-debug-semantics bat-debug-semantics.C)
  target_link_libraries(bat-debug-semantics PRIVATE bat)
  install(TARGETS bat-debug-semantics DESTINATION bin)

  add_executable(bat-delta-bijection bat-delta-bijection.C)
  target_link_libraries(bat-delta-bijection PRIVATE bat)
  install(TARGETS bat-delta-bijection DESTINATION bin)

  add_executable(bat-dis bat-dis.C)
  target_link_libraries(bat-dis PRIVATE bat)
  install(TARGETS bat-dis DESTINATION bin)

  add_executable(bat-dwarf-lines bat-dwarf-lines.C)
  target_link_libraries(bat-dwarf-lines PRIVATE bat)
  install(TARGETS bat-dwarf-lines DESTINATION bin)

  add_executable(bat-flir-ascribe bat-flir-ascribe.C)
  target_link_libraries(bat-flir-ascribe PRIVATE bat)
  install(TARGETS bat-flir-ascribe DESTINATION bin)

  add_executable(bat-flir-insert bat-flir-insert.C)
  target_link_libraries(bat-flir-insert PRIVATE bat)
  install(TARGETS bat-flir-insert DESTINATION bin)

  add_executable(bat-flir-lib bat-flir-lib.C)
  target_link_libraries(bat-flir-lib PRIVATE bat)
  install(TARGETS bat-flir-lib DESTINATION bin)

  add_executable(bat-insnfreq bat-insnfreq.C)
  target_link_libraries(bat-insnfreq PRIVATE bat)
  install(TARGETS bat-insnfreq DESTINATION bin)

  add_executable(bat-insnfreq-cmp bat-insnfreq-cmp.C)
  target_link_libraries(bat-insnfreq-cmp PRIVATE bat)
  install(TARGETS bat-insnfreq-cmp DESTINATION bin)

  add_executable(bat-entropy bat-entropy.C)
  target_link_libraries(bat-entropy PRIVATE bat)
  install(TARGETS bat-entropy DESTINATION bin)

  add_executable(bat-lsb bat-lsb.C)
  target_link_libraries(bat-lsb PRIVATE bat)
  install(TARGETS bat-lsb DESTINATION bin)

  add_executable(bat-lsd bat-lsd.C)
  target_link_libraries(bat-lsd PRIVATE bat)
  install(TARGETS bat-lsd DESTINATION bin)

  add_executable(bat-lsf bat-lsf.C)
  target_link_libraries(bat-lsf PRIVATE bat)
  install(TARGETS bat-lsf DESTINATION bin)

  add_executable(bat-lsv bat-lsv.C)
  target_link_libraries(bat-lsv PRIVATE bat)
  install(TARGETS bat-lsv DESTINATION bin)

  add_executable(bat-mem bat-mem.C)
  target_link_libraries(bat-mem PRIVATE bat)
  install(TARGETS bat-mem DESTINATION bin)

  add_executable(bat-native-trace bat-native-trace.C)
  target_link_libraries(bat-native-trace PRIVATE bat)
  install(TARGETS bat-native-trace DESTINATION bin)

  add_executable(bat-pointers bat-pointers.C)
  target_link_libraries(bat-pointers PRIVATE bat)
  install(TARGETS bat-pointers DESTINATION bin)

  add_executable(bat-prop bat-prop.C)
  target_link_libraries(bat-prop PRIVATE bat)
  install(TARGETS bat-prop DESTINATION bin)

  add_executable(bat-rwsets bat-rwsets.C)
  target_link_libraries(bat-rwsets PRIVATE bat)
  install(TARGETS bat-rwsets DESTINATION bin)

  add_executable(bat-scan-magic bat-scan-magic.C)
  target_link_libraries(bat-scan-magic PRIVATE bat)
  install(TARGETS bat-scan-magic DESTINATION bin)

  add_executable(bat-similar-functions bat-similar-functions.C)
  target_link_libraries(bat-similar-functions PRIVATE bat)
  install(TARGETS bat-similar-functions DESTINATION bin)

  add_executable(bat-simplify bat-simplify.C)
  target_link_libraries(bat-simplify PRIVATE bat)
  install(TARGETS bat-simplify DESTINATION bin)

  add_executable(bat-stack-deltas bat-stack-deltas.C)
  target_link_libraries(bat-stack-deltas PRIVATE bat)
  install(TARGETS bat-stack-deltas DESTINATION bin)

  add_executable(bat-to-c bat-to-c.C)
  target_link_libraries(bat-to-c PRIVATE bat)
  install(TARGETS bat-to-c DESTINATION bin)

  add_executable(bat-trace bat-trace.C)
  target_link_libraries(bat-trace PRIVATE bat)
  install(TARGETS bat-trace DESTINATION bin)

  add_executable(bat-var bat-var.C)
  target_link_libraries(bat-var PRIVATE bat)
  install(TARGETS bat-var DESTINATION bin)

  add_executable(bat-xml-to-json bat-xml-to-json.C)
  target_link_libraries(bat-xml-to-json PRIVATE bat)
  install(TARGETS bat-xml-to-json DESTINATION bin)

  # Also include these targets to build under the `tools` custom target.
  add_dependencies(tools
  bat
  bat-ana
  bat-cc
  bat-cfg
  bat-cg
  bat-check-execution
  bat-container
  bat-dataflow
  bat-debug-semantics
  bat-delta-bijection
  bat-dis
  bat-dwarf-lines
  bat-entropy
  bat-flir-ascribe
  bat-flir-insert
  bat-flir-lib
  bat-insnfreq
  bat-insnfreq-cmp
  bat-lsb
  bat-lsd
  bat-lsf
  bat-lsv
  bat-mem
  bat-native-trace
  bat-pointers
  bat-prop
  bat-rwsets
  bat-scan-magic
  bat-similar-functions
  bat-simplify
  bat-stack-deltas
  bat-to-c
  bat-trace
  bat-var
  bat-xml-to-json)

endif()
