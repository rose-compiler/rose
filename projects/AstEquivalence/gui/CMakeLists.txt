
include_directories(${ROSE_SOURCE_DIR}/src/roseExtensions/sqlite3x)

add_executable(compilerFlagsGui compilerFlagsMainGui.C compilerFlagsGui.C)
target_link_libraries(compilerFlagsGui
  ROSE_DLL ${Boost_LIBRARIES} qrose RoseSQLite3xDatabase
)

#EXTRA_DIST =  disks.xpm  folder.xpm compilerFlagsGui.h  icons.h
