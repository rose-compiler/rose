# Path Variable to install the headers in this dir where autoconf installs 
set(NLOHMANN_INSTALL_DIR "${INCLUDE_INSTALL_DIR}/nlohmann") 

# Variable to store the headers to be installed
set(NLOHMANN_HEADERS json.hpp json.h) 

# install instructions
install(FILES ${NLOHMANN_HEADERS} DESTINATION ${NLOHMANN_INSTALL_DIR})
