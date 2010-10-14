
include_directories(${CMAKE_SOURCE_DIR}/src/frontend/ECJ_ROSE_Connection ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(libJavaTraversal_LIB_SRCS
    openJavaParser_main.C 
    ecj.C)

add_library(libJavaTraversal SHARED ${libJavaTraversal_LIB_SRCS})

target_link_libraries(libJavaTraversal ${KDE4_KDECORE_LIBS})

set_target_properties(libJavaTraversal PROPERTIES VERSION 1.0.0 SOVERSION 1)
install(TARGETS libJavaTraversal ${INSTALL_TARGETS_DEFAULT_ARGS})


########### next target ###############


########### install files ###############

install(FILES  DESTINATION ${INCLUDE_INSTALL_DIR})



