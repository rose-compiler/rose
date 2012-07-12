
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### install files ###############

install(FILES  CompilerOutputReader.h CompilerOutputParser.h IncludingPreprocessingInfosCollector.h DESTINATION ${INCLUDE_INSTALL_DIR})

