
#include "rose.h"
#include <vector>
 
using namespace std;

int
main ( int argc, char * argv[] )
{
#if 1
     assert ( 1 < argc );
     int numFiles = argc -1;
     std::vector<std::string> fileNames;
     for (int i= 0; i < numFiles; ++i)
        {
          fileNames.push_back(argv[i+1]) ;
        }
#endif
      std::cout << "################ In astFileRead.C ############## " << std::endl;
      std::cout << "#  Going to read " << numFiles << " files  " << std::endl;
      std::cout << "################################################ " << std::endl;

#if 1 
         std :: cout  << std::endl<< "Here we call the AST_FILE_IO :: readASTFromFile ..." << std::endl;
         for (int i= 0; i < numFiles; ++i)
             {
               std :: cout  << "Here we read .... " << fileNames[i] << std::endl;
               AST_FILE_IO :: readASTFromFile ( fileNames[i] + ".binary" );
             }
         std :: cout  << std::endl<< "AST_FILE_IO :: readASTFromFile done ... " << std::endl;
#endif
#if 1
         AstData* ast = NULL;
         for (int i= 0; i < numFiles; ++i)
             {
               ast = AST_FILE_IO::getAst(i);
               AST_FILE_IO::setStaticDataOfAst(ast);
               AstTests::runAllTests(ast->getRootOfAst());
               backend(ast->getRootOfAst());
             }
#endif
     return 0;
   }                                                                                                                                                                                                     
