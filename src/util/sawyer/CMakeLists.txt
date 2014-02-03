
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

install(FILES Assert.h Message.h ProgressBar.h Sawyer.h DESTINATION ${INCLUDE_INSTALL_DIR})
