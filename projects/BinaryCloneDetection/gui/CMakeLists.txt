
include_directories(${QT_QT3SUPPORT_INCLUDE_DIR})
add_definitions(-DQT3_SUPPORT)

add_executable(binaryCloneGui binaryCloneMainGui.C binaryCloneGui.C)
target_link_libraries(binaryCloneGui
  ROSE_DLL qrose RoseSQLite3xDatabase ${Boost_LIBRARIES}
)

#if !ROSE_BUILD_OS_IS_OSX
#	binaryCloneGui_LDADD += -lrt
#endif

#EXTRA_DIST = binaryCloneGui.h disks.xpm  folder.xpm  icons.h

