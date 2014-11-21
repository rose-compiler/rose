add_library( phpFrontend OBJECT PHPFrontend.C)
add_dependencies(phpFrontend rosetta_generated)
install(FILES  PHPFrontend.h DESTINATION include)
