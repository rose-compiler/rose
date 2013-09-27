QT4_WRAP_UI(SrcBinView_UI_CPP SrcBinView.ui)
QT4_WRAP_CPP(SrcBinView_MOC_CPP SrcBinView.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(SrcBinView OBJECT
  SrcBinView.cpp ${SrcBinView_UI_CPP} ${SrcBinView_MOC_CPP})
