
include_directories(${OPENGL_INCLUDE_DIR})

if(GLUT_FOUND)
  add_executable(vizzBinary vizzBinary.C)
  target_link_libraries(vizzBinary
    ROSE_DLL ${Boost_LIBRARIES} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})
endif()

add_executable(test test.C)
target_link_libraries(test
  ROSE_DLL ${Boost_LIBRARIES} ${OPENGL_LIBRARIES})

