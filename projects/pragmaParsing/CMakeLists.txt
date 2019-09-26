add_executable(hcpragma hcpragma.C hcpragma.h)
target_link_libraries(hcpragma ROSE_DLL ${Boost_LIBRARIES})

add_executable(testParserBuilder testParserBuilder.cpp)
target_link_libraries(testParserBuilder ROSE_DLL ${Boost_LIBRARIES})

add_executable(buildFromString buildFromString.C)
target_link_libraries(buildFromString ROSE_DLL ${Boost_LIBRARIES})
