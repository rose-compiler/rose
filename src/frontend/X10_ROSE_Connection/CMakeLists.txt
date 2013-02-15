
include_directories(${CMAKE_SOURCE_DIR}/src/frontend/X10_ROSE_Connection ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(libX10Traversal_LIB_SRCS
    x10_main.C 
    x10.C)

add_library(libX10Traversal SHARED ${libX10Traversal_LIB_SRCS})

target_link_libraries(libX10Traversal ${KDE4_KDECORE_LIBS})

set_target_properties(libX10Traversal PROPERTIES VERSION 1.0.0 SOVERSION 1)
install(TARGETS libX10Traversal ${INSTALL_TARGETS_DEFAULT_ARGS})


########### next target ###############


########### install files ###############

install(FILES  DESTINATION ${INCLUDE_INSTALL_DIR})



