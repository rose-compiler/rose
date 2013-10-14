QT4_WRAP_CPP(TaskSystem_MOC_CPP
  CompilerOutputWidget.h GccTask.h RoseFrontendTask.h TaskInterface.h
  TaskList.h)

QT4_ADD_RESOURCES(TaskSystem_QRC_CPP res.qrc)

add_library(TaskSystem OBJECT
  CompilerOutputWidget.cpp GccTask.cpp RoseFrontendTask.cpp TaskList.cpp
  ${TaskSystem_MOC_CPP} ${TaskSystem_QRC_CPP})
