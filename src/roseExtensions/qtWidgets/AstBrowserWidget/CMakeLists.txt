QT4_WRAP_CPP(AstBrowserWidget_MOC_CPP AstBrowserWidget.h QtAstModel.h)

add_library(AstBrowserWidget OBJECT
  AstBrowserWidget.cpp QtAstModel.cpp ${AstBrowserWidget_MOC_CPP})
