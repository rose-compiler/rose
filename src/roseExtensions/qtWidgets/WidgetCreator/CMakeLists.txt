include_directories( ${QT_QT3SUPPORT_INCLUDE_DIR})
add_definitions(-DQT3_SUPPORT)

QT4_WRAP_CPP(WidgetCreator_MOC_CPP
  AsmInstructionBarCreator.h AsmViewCreator.h AstBrowserWidgetCreator.h
  AstGraphWidgetCreator.h BeautifiedAstCreator.h CallGraphWidgetCreator.h
  MetricsKiviatCreator.h NodeInfoWidgetCreator.h QRQueryBoxCreator.h
  QRSourceBoxCreator.h QRTreeBoxCreator.h RoseCodeEditCreator.h
  RoseFileSelectorCreator.h SrcBinViewCreator.h SubWindowFactory.h)

add_library(WidgetCreator OBJECT
  AsmInstructionBarCreator.cpp AsmViewCreator.cpp AstBrowserWidgetCreator.cpp
  AstGraphWidgetCreator.cpp BeautifiedAstCreator.cpp CallGraphWidgetCreator.cpp
  MetricsKiviatCreator.cpp NodeInfoWidgetCreator.cpp QRQueryBoxCreator.cpp
  QRSourceBoxCreator.cpp QRTreeBoxCreator.cpp RoseCodeEditCreator.cpp
  RoseFileSelectorCreator.cpp SrcBinViewCreator.cpp SubWindowFactory.cpp
  WidgetCreatorInterface.cpp ${WidgetCreator_MOC_CPP})
