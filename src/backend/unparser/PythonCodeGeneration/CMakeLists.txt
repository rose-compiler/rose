
########### next target ###############
if(ENABLE_PYTHON)
  add_library(unparsePython unparsePython.C)
endif()

########### install files ###############

install(FILES unparsePython.h DESTINATION ${INCLUDE_INSTALL_DIR})
