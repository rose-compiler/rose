QT4_WRAP_UI(AstProperties_UI_CPP MyWidget.ui)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(AstProperties main.cpp ${AstProperties_UI_CPP})
target_link_libraries(AstProperties
  ROSE_DLL RoseQtWidgets ${Boost_LIBRARIES} ${QT_LIBRARIES})
