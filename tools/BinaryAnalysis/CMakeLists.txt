if(enable-binary-analysis)
  include_directories(. ${ROSE_INCLUDES})

  add_library(bat SHARED batSupport.C)
  target_link_libraries(bat ROSE_DLL)

  add_executable(bat-ana bat-ana.C)
  target_link_libraries(bat-ana bat ROSE_DLL)

  add_executable(bat-cc bat-cc.C)
  target_link_libraries(bat-cc bat ROSE_DLL)

  add_executable(bat-cfg bat-cfg.C)
  target_link_libraries(bat-cfg bat ROSE_DLL)

  add_executable(bat-cg bat-cg.C)
  target_link_libraries(bat-cg bat ROSE_DLL)

  add_executable(bat-container bat-container.C)
  target_link_libraries(bat-container bat ROSE_DLL)

  add_executable(bat-dis bat-dis.C)
  target_link_libraries(bat-dis bat ROSE_DLL)

  add_executable(bat-insnfreq bat-insnfreq.C)
  target_link_libraries(bat-insnfreq bat ROSE_DLL)

  add_executable(bat-insnfreq-cmp bat-insnfreq-cmp.C)
  target_link_libraries(bat-insnfreq-cmp bat ROSE_DLL)

  add_executable(bat-entropy bat-entropy.C)
  target_link_libraries(bat-entropy bat ROSE_DLL)

  add_executable(bat-lsb bat-lsb.C)
  target_link_libraries(bat-lsb bat ROSE_DLL)

  add_executable(bat-lsd bat-lsd.C)
  target_link_libraries(bat-lsd bat ROSE_DLL)

  add_executable(bat-lsf bat-lsf.C)
  target_link_libraries(bat-lsf bat ROSE_DLL)

  add_executable(bat-mem bat-mem.C)
  target_link_libraries(bat-mem bat ROSE_DLL)

  add_executable(bat-prop bat-prop.C)
  target_link_libraries(bat-prop bat ROSE_DLL)

  add_executable(bat-stack-deltas bat-stack-deltas.C)
  target_link_libraries(bat-stack-deltas bat ROSE_DLL)

  add_executable(bat-trace bat-trace.C)
  target_link_libraries(bat-trace bat ROSE_DLL)

endif()
