add_executable(defaultTranslator defaultTranslator.C)
add_dependencies(defaultTranslator ROSE_DLL)
target_link_libraries(defaultTranslator ROSE_DLL ${Boost_LIBRARIES})
install(TARGETS defaultTranslator DESTINATION bin)

add_executable(preprocessingInfoDumper preprocessingInfoDumper.C)
add_dependencies(preprocessingInfoDumper ROSE_DLL)
target_link_libraries(preprocessingInfoDumper ROSE_DLL ${Boost_LIBRARIES})
install(TARGETS preprocessingInfoDumper DESTINATION bin)

add_executable(mangledNameDumper mangledNameDumper.C)
add_dependencies(mangledNameDumper ROSE_DLL)
target_link_libraries(mangledNameDumper ROSE_DLL ${Boost_LIBRARIES})
install(TARGETS mangledNameDumper DESTINATION bin)

add_executable(qualifiedNameDumper qualifiedNameDumper.C)
add_dependencies(qualifiedNameDumper ROSE_DLL)
target_link_libraries(qualifiedNameDumper ROSE_DLL ${Boost_LIBRARIES})
install(TARGETS qualifiedNameDumper DESTINATION bin)
