add_executable(defaultTranslator defaultTranslator.C)
target_link_libraries(defaultTranslator ROSE_DLL ${Boost_LIBRARIES})

add_executable(preprocessingInfoDumper preprocessingInfoDumper.C)
target_link_libraries(preprocessingInfoDumper ROSE_DLL ${Boost_LIBRARIES})

add_executable(mangledNameDumper mangledNameDumper.C)
target_link_libraries(mangledNameDumper ROSE_DLL ${Boost_LIBRARIES})

add_executable(qualifiedNameDumper qualifiedNameDumper.C)
target_link_libraries(qualifiedNameDumper ROSE_DLL ${Boost_LIBRARIES})
