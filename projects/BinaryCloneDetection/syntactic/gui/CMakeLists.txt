include_directories(${QT_QT3SUPPORT_INCLUDE_DIR})
add_definitions(-DQT3_SUPPORT)

add_executable(binaryCloneGui binaryCloneMainGui.C binaryCloneGui.C)
target_link_libraries(binaryCloneGui ROSE_DLL qrose ${Boost_LIBRARIES})
