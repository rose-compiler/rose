QT4_WRAP_CPP(BeautifiedAst_MOC_CPP BAstModel.h BAstView.h)
QT4_ADD_RESOURCES(BeautifiedAst_QRC_CPP BeautifiedAst.qrc)

add_library(BeautifiedAst OBJECT
  BAstModel.cpp BAstNode.cpp BAstView.cpp
  ${BeautifiedAst_MOC_CPP} ${BeautifiedAst_QRC_CPP})
