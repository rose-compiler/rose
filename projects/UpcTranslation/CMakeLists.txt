add_executable(roseupcc upc_translation.C roseupcc.C)
target_link_libraries(roseupcc ROSE_DLL ${Boost_LIBRARIES})
