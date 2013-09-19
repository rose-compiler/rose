QT4_WRAP_CPP(RoseFileSelector_MOC_CPP RoseFileComboBox.h)

add_library(RoseFileSelector OBJECT
  RoseFileComboBox.cpp ${RoseFileSelector_MOC_CPP})
