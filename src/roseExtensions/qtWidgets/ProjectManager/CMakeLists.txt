QT4_WRAP_UI(ProjectManager_UI_CPP CmdLineEditDialog.ui)
QT4_WRAP_CPP(ProjectManager_MOC_CPP
  CmdLineEditDialog.h DependencyInfo.h Project.h ProjectView.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_library(ProjectManager OBJECT
  CmdLineEditDialog.cpp DependencyInfo.cpp Project.cpp
  ProjectManager.cpp ProjectView.cpp
  ${ProjectManager_UI_CPP} ${ProjectManager_MOC_CPP})
