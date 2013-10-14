QT4_WRAP_UI(MetricsConfig_UI_CPP MetricsConfig.ui)
QT4_WRAP_CPP(MetricsConfig_MOC_CPP MetricsConfig.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(MetricsConfig OBJECT
  MetricsConfig.cpp MetricsInfo.cpp
  ${MetricsConfig_UI_CPP} ${MetricsConfig_MOC_CPP})
