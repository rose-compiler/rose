QT4_WRAP_CPP(KiviatView_MOC_CPP KiviatView.h)

add_library(KiviatView OBJECT KiviatView.cpp ${KiviatView_MOC_CPP})
