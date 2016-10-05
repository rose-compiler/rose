#include "rose.h"
 
#include <unistd.h>
#include "AstPerformance.h"

// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

int
main ( int argc, char * argv[] )
   {
  // TimingPerformance timer ("main execution time (sec) = ");
     SgProject* project_identity = frontend(argc,argv);
     SgProject* project_test = project_identity;
     ROSE_ASSERT (project_identity != NULL);

#if 1
     std :: cout << "Starting initial AstTests ...... "   << flush;
     AstTests::runAllTests(project_test);
     std :: cout << "done "   << std::endl;
#endif
 
     std :: cout  << std::endl<< "Here we call the AST_FILE_IO :: start_up ..." << std::endl;
     AST_FILE_IO :: startUp( project_test ) ;
     std :: cout  << std::endl<< "AST_FILE_IO :: start_up done ... " << std::endl;

  // AST_FILE_IO::printListOfPoolSizes();

#if 1
  // This will move everything into the EasyStorageClasses and then reconstruct 
  // it from scratch. As a result it is a good test without and File I/O.
     std :: cout  << std::endl<< "Here we call the AST_FILE_IO :: compressAstInMemoryPool() ..." << std::endl;
     AST_FILE_IO :: compressAstInMemoryPool();
     std :: cout  << std::endl<< "AST_FILE_IO :: compressAstInMemoryPool() done ... " << std::endl;
#endif

#if 1
     std :: cout << "Starting tests after compression AstTests ...... "   << flush;
     AstTests::runAllTests(project_test);
     std :: cout << "done "   << std::endl;
#endif

#if 1
     std :: cout  << std::endl<< " Unparsing file ......." << std::endl;
     backend(project_test);
     std :: cout  << std::endl<< " File to disk ! " << std::endl;
#endif

     return 0;
   }                                                                                                                                                                                                     
