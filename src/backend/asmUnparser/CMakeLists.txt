
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )

install(FILES  AsmUnparser.h AsmUnparser_compat.h AsmFunctionIndex.h DESTINATION ${INCLUDE_INSTALL_DIR})
