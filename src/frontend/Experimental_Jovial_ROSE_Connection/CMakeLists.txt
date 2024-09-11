# Include directories
if("${ATERM_INSTALL_PATH}" STREQUAL "") 
  message(FATAL_ERROR "ATERM_INSTALL_PATH was not set appropriately, it is an empty string") 
endif() 

set(ATERM_INCLUDE_DIR "${ATERM_INSTALL_PATH}/include")

include_directories(
    ${ROSE_INCLUDES}               # Add ROSE includes (defined elsewhere)
    ${ATERM_INCLUDE_DIR}           # Add ATERM includes
    ${CMAKE_SOURCE_DIR}/src/frontend/Experimental_General_Language_Support
)

# Library target for libexperimentalRoseJovial
add_library(experimentalRoseJovial STATIC 
    jovial_support.C 
    ATermToSageJovialTraversal.C 
    preprocess.C
)

# Link to aterm 
target_link_libraries(experimentalRoseJovial PUBLIC ${ATERM_INSTALL_PATH}/lib/libATerm.so)


# Executable target for jovial_preprocess
#add_executable(jovial_preprocess jovial_preprocess.C)
#target_link_libraries(jovial_preprocess PRIVATE experimentalRoseJovial)

# Install header files
#install(FILES jovial_support.h DESTINATION include)

# Ensure internal headers are not installed
#set_property(TARGET experimentalRoseJovial PROPERTY PUBLIC_HEADER "jovial_support.h")

# Specify non-installed internal headers
#set_target_properties(experimentalRoseJovial PROPERTIES PRIVATE_HEADER "ATermToSageJovialTraversal.h")
