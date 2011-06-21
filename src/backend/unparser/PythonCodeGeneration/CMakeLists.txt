
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############


########### install files ###############

install(FILES  $(unparsePythonCodeGeneration_includeHeaders) DESTINATION ${INCLUDE_INSTALL_DIR})
