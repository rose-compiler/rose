
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############


#add_library( midend_binary STATIC ${midend_binary_src}  )
#add_dependencies( midend_binary ROSETTA )


########### install files ###############

install(FILES  $(BINARY_DISASSEMBLY_HEADER_FILES) DESTINATION ${INCLUDE_INSTALL_DIR})
