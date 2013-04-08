QT4_WRAP_CPP(NodeInfoWidget_MOC_CPP NodeInfoWidget.h)

add_library(NodeInfoWidget OBJECT NodeInfoWidget.cpp ${NodeInfoWidget_MOC_CPP})
