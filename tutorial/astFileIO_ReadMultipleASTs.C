
#include "rose.h"

using namespace std;

int
main ( int argc, char * argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     assert ( 1 < argc );
     int numFiles = argc -1;
     std::vector<std::string> fileNames;
     for (int i= 0; i < numFiles; ++i)
        {
          fileNames.push_back(argv[i+1]) ;
        }

     std::cout << "################ In astFileRead.C ############## " << std::endl;
     std::cout << "#  Going to read " << numFiles << " files  " << std::endl;
     std::cout << "################################################ " << std::endl;

     std :: cout  << std::endl<< "Here we call the AST_FILE_IO :: readASTFromFile ..." << std::endl;
     for (int i= 0; i < numFiles; ++i)
        {
          std :: cout  << "Here we read .... " << fileNames[i] << std::endl;
          AST_FILE_IO :: readASTFromFile ( fileNames[i] + ".binary" );
        }
     std :: cout  << std::endl<< "AST_FILE_IO :: readASTFromFile done ... " << std::endl;

     AstData* ast = NULL;
     for (int i= 0; i < numFiles; ++i)
        {
          ast = AST_FILE_IO::getAst(i);
          AST_FILE_IO::setStaticDataOfAst(ast);
          AstTests::runAllTests(ast->getRootOfAst());
          backend(ast->getRootOfAst());
       }

  // Now do the AST Merge Operation (incomplete)

     return 0;
   }                                                                                                                                                                                                     
