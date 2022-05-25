include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

########### next target ###############
if(enable-python)
  add_library(unparsePython unparsePython.C)

  # fix: Cxx_Grammar.h: No such file or directory
  add_dependencies(unparsePython rosetta_generated)
endif()

########### install files ###############

install(FILES unparsePython.h DESTINATION ${INCLUDE_INSTALL_DIR})
