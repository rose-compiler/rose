
include_directories(${KDE4_INCLUDES} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} )


########### next target ###############


########### install files ###############

install(
  FILES ASTeval.h ASTfactory.h ASTvisitor.h  error_config.h poetAST.h
        poet_config.h poet_yacc.h SymbolTable.h timing.h
  DESTINATION ${INCLUDE_INSTALL_DIR})

