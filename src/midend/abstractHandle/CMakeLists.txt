add_library(abstractHandle OBJECT
  abstract_handle.cpp roseAdapter.cpp abstract_handle.cpp loopAdapter.cpp
  roseAdapter.cpp)
add_dependencies(abstractHandle rosetta_generated)

########### install files ###############
install(FILES abstract_handle.h roseAdapter.h
        DESTINATION ${INCLUDE_INSTALL_DIR})
