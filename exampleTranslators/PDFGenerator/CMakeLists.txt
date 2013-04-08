add_executable(pdfGenerator pdfGenerator.C)
target_link_libraries(pdfGenerator ROSE_DLL ${Boost_LIBRARIES})
