include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${Boost_INCLUDE_DIRS})

add_executable(bug125 bug125.C)
add_executable(qualifiedName qualifiedName.C)

target_link_libraries(bug125 ROSE_DLL ${link_with_libraries})
target_link_libraries(qualifiedName ROSE_DLL ${edg_lib} ${link_with_libraries})

if(enable-c)
  enable_testing()
  add_test(
    NAME testbug125
    COMMAND bug125 -c ${CMAKE_CURRENT_SOURCE_DIR}/inputbug125.C)

  if(HAVE_SYSTEM_TIME_H)
    add_test(
      NAME testqualifiedName
      COMMAND qualifiedName -c ${ROSE_TOP_SRC_DIR}/projects/autoTuning/tests/jacobi.c)
  endif()
endif()
