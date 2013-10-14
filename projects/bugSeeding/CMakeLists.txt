
add_executable(bugSeeding
  bugSeeding.C bugSeedingSupport.C bugSeedingAttributes.C
  bufferOverFlowSecurityFlaw.C
)
target_link_libraries(bugSeeding ROSE_DLL ${Boost_LIBRARIES})

