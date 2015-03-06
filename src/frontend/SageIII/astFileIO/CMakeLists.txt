
#add_library(astFileIO OBJECT AST_FILE_IO.C StorageClasses.C)
#add_dependencies(astFileIO rosetta_generated)

########### install files ###############

install(FILES  StorageClassMemoryManagement.h DESTINATION ${INCLUDE_INSTALL_DIR})
#install(FILES  AstSpecificDataManagingClass.h DESTINATION ${INCLUDE_INSTALL_DIR})
