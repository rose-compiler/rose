# Pei-Hung (07/28/16) loopAdapter.cpp is only in EXTRA_DIST for Autotool
# Remove it from add_library due to return types covariant

add_library(abstractHandle OBJECT
  abstract_handle.cpp roseAdapter.cpp abstract_handle.cpp 
  roseAdapter.cpp)
add_dependencies(abstractHandle rosetta_generated)

########### install files ###############
install(FILES abstract_handle.h roseAdapter.h
        DESTINATION ${INCLUDE_INSTALL_DIR})
