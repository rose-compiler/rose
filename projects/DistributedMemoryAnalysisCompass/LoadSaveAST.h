#ifndef __LOADSAVE_AST___
#define __LOADSAVE_AST___


namespace LoadSaveAST {

/* ******************************************************
 * save AST to file
 * ******************************************************/
  void saveAST(std::string filename, SgProject* project) {
  std::cout << "ROSE: AST SAVE .. project: " << filename << std::endl;

  //  REGISTER_ATTRIBUTE_FOR_FILE_IO(UserDefinedAttribute);
  AST_FILE_IO::startUp( project ) ;
  std::cout << "ROSE: AST SAVE ... startup done." << std::endl;
  AST_FILE_IO::writeASTToFile ( filename  );
  std::cout << "ROSE: AST SAVE ... writeToFile done." << std::endl;
  }


  /* ******************************************************
   * load the AST from file
   * ******************************************************/
  SgProject* loadAST(std::string filename){
    std::cout << "ROSE: AST LOAD." << std::endl;
    double memusage = ROSE_MemoryUsage().getMemoryUsageMegabytes();

    //REGISTER_ATTRIBUTE_FOR_FILE_IO(UserDefinedAttribute);
    AST_FILE_IO::clearAllMemoryPools();
    SgProject* project = (SgProject*)
      (AST_FILE_IO::readASTFromFile ( filename  ) );

    double memusageend = ROSE_MemoryUsage().getMemoryUsageMegabytes();
    std::cout << "Memory usage for ROSE : " << (memusageend-memusage) << " MB.  Total MEM-USAGE: " << (memusageend) << " MB." << std::endl;
    std::cout << "ROSE: AST LOAD COMPLETE -----------------------------------  " << std::endl;
    return project;
  }

}


#endif
