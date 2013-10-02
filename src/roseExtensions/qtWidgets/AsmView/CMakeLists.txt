QT4_WRAP_UI(AsmView_UI_CPP ConfigureAsmViewDelegate.ui)
QT4_WRAP_CPP(AsmView_MOC_CPP AsmView.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(AsmView OBJECT
  AsmTreeNode.cpp AsmView.cpp ${AsmView_UI_CPP} ${AsmView_MOC_CPP})
