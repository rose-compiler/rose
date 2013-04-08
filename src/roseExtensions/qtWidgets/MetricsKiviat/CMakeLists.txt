QT4_WRAP_UI(MetricsKiviat_UI_CPP MetricsWidget.ui KiviatInfo.ui)
QT4_WRAP_CPP(MetricsKiviat_MOC_CPP MetricsKiviat.h KiviatInfo.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(MetricsKiviat OBJECT
  MetricsKiviat.cpp KiviatInfo.cpp
  ${MetricsKiviat_UI_CPP} ${MetricsKiviat_MOC_CPP})
