QT4_WRAP_CPP(TreeModel_MOC_CPP ItemTreeModel.h)

add_library(TreeModel OBJECT
  ItemTreeModel.cpp ItemTreeNode.cpp ${TreeModel_MOC_CPP})
