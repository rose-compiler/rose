include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

########### install files ###############
install(FILES memory_object.h memory_object_impl.h DESTINATION ${INCLUDE_INSTALL_DIR})

