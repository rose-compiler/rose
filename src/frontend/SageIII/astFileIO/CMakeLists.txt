
#add_library(astFileIO OBJECT AST_FILE_IO.C StorageClasses.C)

########### install files ###############

install(FILES  StorageClassMemoryManagement.h DESTINATION ${INCLUDE_INSTALL_DIR})
install(FILES  AstSpecificDataManagingClass.h DESTINATION ${INCLUDE_INSTALL_DIR})
