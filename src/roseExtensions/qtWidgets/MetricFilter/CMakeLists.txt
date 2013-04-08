QT4_WRAP_UI(MetricFilter_UI_CPP MetricFilter.ui)
QT4_WRAP_CPP(MetricFilter_MOC_CPP MetricFilter.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(MetricFilter OBJECT
  MetricFilter.cpp ${MetricFilter_UI_CPP} ${MetricFilter_MOC_CPP})
