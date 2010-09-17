
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

#add_library( roseBinaryLoader STATIC ${roseBinaryLoader_SRC} ) 
#add_dependencies( roseBinaryLoader ROSETTA  )

########### install files ###############

install(FILES  BinaryLoader.h BinaryLoaderElf.h  DESTINATION ${INCLUDE_INSTALL_DIR})
