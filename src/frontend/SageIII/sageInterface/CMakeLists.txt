
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############
#add_library( libsageInterface STATIC ${libsageInterface_SRC}  )
#add_dependencies( libsageInterface ROSETTA )

########### install files ###############

install(
  FILES  sageInterface.h sageBuilder.h sageBuilderAsm.h integerOps.h abiStuff.h
  DESTINATION ${INCLUDE_INSTALL_DIR}
  )
