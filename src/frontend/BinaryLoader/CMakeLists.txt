
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############

set(roseBinaryLoader_SRC
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryLoader/binaryLoader.C
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryLoader/BinaryLoaderGeneric.C
   ${CMAKE_SOURCE_DIR}/src/frontend/BinaryLoader/BinaryLoaderElf.C
   )

add_library( roseBinaryLoader STATIC ${roseBinaryLoader_SRC} ) 
add_dependencies( roseBinaryLoader ROSETTA  )

########### install files ###############

install(FILES  binaryLoader.h BinaryLoaderGeneric.h BinaryLoaderElf.h  DESTINATION ${INCLUDE_INSTALL_DIR})
