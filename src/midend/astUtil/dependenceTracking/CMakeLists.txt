
include_directories(${CMAKE_SOURCE_DIR}/src/util ${CMAKE_SOURCE_DIR}/src/midend/astUtil ${CMAKE_SOURCE_DIR}/src/midend/astUtil/astInterface ${CMAKE_SOURCE_DIR}/src/util/support ${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############


########### install files ###############

install(FILES  dependence_table.h dependence_analysis.h DESTINATION ${INCLUDE_INSTALL_DIR})

