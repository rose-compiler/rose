add_library( trans_tracking OBJECT transformationTracking.cpp)
add_dependencies(trans_tracking rosetta_generated)
install(FILES transformationTracking.h DESTINATION ${INCLUDE_INSTALL_DIR})

