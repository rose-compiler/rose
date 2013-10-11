QT4_WRAP_CPP(PropertyTreeWidget_MOC_CPP
  PropertyTreeModel.h PropertyTreeWidget.h)

add_library(PropertyTreeWidget OBJECT
  PropertyTreeModel.cpp PropertyTreeWidget.cpp ${PropertyTreeWidget_MOC_CPP})
