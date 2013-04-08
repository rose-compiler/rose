include_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${QT_QT3SUPPORT_INCLUDE_DIR}
  ${QT_QTXML_INCLUDE_DIR})
add_definitions(-DQT3_SUPPORT)

add_subdirectory(QCodeEdit)

QT4_WRAP_CPP(QCodeEditWidget_MOC_CPP QCodeEditWidget.h)
QT4_ADD_RESOURCES(BeautifiedAst_QRC_CPP example.qrc  QCodeEdit.qrc)

add_library(QCodeEditWidget
  QCodeEditWidget.cpp ${QCodeEditWidget_MOC_CPP} ${QCodeEditWidget_QRC_CPP}
  $<TARGET_OBJECTS:qcodeedit>)
