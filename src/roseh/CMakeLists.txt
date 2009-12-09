
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target   ###############
# tps (12/07/2009) Removed dependencies because all files recompile if one header file is changed
# introduced this initially so that precompiled headers can be introduced in Windows.

#set( roseh_src
#${CMAKE_SOURCE_DIR}/src/roseh/roseh.C
#)

#add_library( roseh STATIC ${roseh_src}  )
#add_dependencies( roseh ROSETTA )

#install(FILES  rose.h  DESTINATION ${INCLUDE_INSTALL_DIR})

