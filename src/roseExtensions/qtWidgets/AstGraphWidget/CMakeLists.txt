QT4_WRAP_UI(AstGraphWidget_UI_CPP LayoutControl.ui)
QT4_WRAP_CPP(AstGraphWidget_MOC_CPP
  AstGraphWidget.h CallGraphWidget.h DisplayGraphNode.h)
QT4_ADD_RESOURCES(AstGraphWidget_QRC_CPP icons.qrc)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(AstGraphWidget OBJECT
  AstGraphWidget.cpp CallGraphWidget.cpp DisplayEdge.cpp DisplayGraphNode.cpp
  DisplayNode.cpp TreeLayoutGenerator.cpp treeTest.cpp
  ${AstGraphWidget_UI_CPP} ${AstGraphWidget_MOC_CPP} ${AstGraphWidget_QRC_CPP}
)
