include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

set(outlineTutorial_SRCS outlineTutorial.cc)
add_executable(outlineTutorial ${outlineTutorial_SRCS})
target_link_libraries(outlineTutorial ROSE_DLL ${Boost_LIBRARIES})

set(outlineIfs_SRCS outlineIfs.cc)
add_executable(outlineIfs ${outlineIfs_SRCS})
target_link_libraries(outlineIfs ROSE_DLL ${Boost_LIBRARIES})

set(outlinePreproc_SRCS outlinePreproc.cc)
add_executable(outlinePreproc ${outlinePreproc_SRCS})
target_link_libraries(outlinePreproc ROSE_DLL ${Boost_LIBRARIES})

# Install only those executables that are useful to users after ROSE is installed. If the main purpose of compiling
# the tool is to ensure that the source code is correct for the purposes of documentation in the various tutorials,
# then there's no point in installing that executable.
#install(TARGETS outline DESTINATION bin)
