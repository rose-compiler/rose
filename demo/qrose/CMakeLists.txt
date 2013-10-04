
add_definitions(-DQT3_SUPPORT)
include_directories(
  ${ROSE_INCLUDES} ${QT_QT3SUPPORT_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

########### next target ###############

set(t1_window_SRCS t1_window.C)
add_executable(t1_window ${t1_window_SRCS})
target_link_libraries(t1_window ROSE_DLL qrose ${Boost_LIBRARIES})


########### next target ###############

set(t2_layout_SRCS t2_layout.C)
add_executable(t2_layout ${t2_layout_SRCS})
target_link_libraries(t2_layout ROSE_DLL qrose ${Boost_LIBRARIES})


########### next target ###############

set(t3_buttons_SRCS t3_buttons.C)
add_executable(t3_buttons ${t3_buttons_SRCS})
target_link_libraries(t3_buttons ROSE_DLL qrose ${Boost_LIBRARIES})


########### next target ###############

set(t4_events_SRCS t4_events.C)
add_executable(t4_events ${t4_events_SRCS})
target_link_libraries(t4_events ROSE_DLL qrose ${Boost_LIBRARIES})


########### next target ###############

set(t5_components_SRCS t5_components.C)
add_executable(t5_components ${t5_components_SRCS})
target_link_libraries(t5_components ROSE_DLL qrose ${Boost_LIBRARIES})


########### next target ###############

set(t6_query1_SRCS t6_query1.C)
add_executable(t6_query1 ${t6_query1_SRCS})
target_link_libraries(t6_query1 ROSE_DLL qrose ${Boost_LIBRARIES})

