QT4_WRAP_UI(util_UI_CPP MetricBgDelegateConfig.ui)

QT4_WRAP_CPP(util_MOC_CPP
  MetricAttributeCollector.h MetricBgDelegate.h QRoseComponentProxy.h
  RoseGraphicsView.h RoseTreeView.h)

QT4_ADD_RESOURCES(util_QRC_CPP util.qrc)

include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${QT_QT3SUPPORT_INCLUDE_DIR})
add_definitions(-DQT3_SUPPORT)

add_library(util OBJECT
  AsmToSourceMapper.cpp AstDisplayInfo.cpp AstFilters.cpp ItemModelHelper.cpp
  LCS.cpp MetricAttributeCollector.cpp MetricBgDelegate.cpp
  QRoseComponentProxy.cpp RoseGraphicsView.cpp RoseTreeView.cpp
  SageMimeData.cpp SgNodeUtil.cpp
  ${util_UI_CPP} ${util_MOC_CPP} ${util_QRC_CPP})
