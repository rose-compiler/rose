                                                                                  
//#define                 OUTLINE_DEBUG                                                                                                   
#include "rose.h"
 
#include <unistd.h>
#include "AstPerformance.h"

// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

int
main ( int argc, char * argv[] )
   {
     TimingPerformance timer ("main execution time (sec) = ",true);
     SgProject* project_identity = frontend(argc,argv);
     SgProject* project_test = project_identity;
     ROSE_ASSERT (project_identity != NULL);

     printf ("project_identity->get_outputFileName() = %s \n",project_identity->get_outputFileName().c_str());

#if 1    
     backend(project_identity);
     std::string fileName =  project_identity->get_outputFileName() ;
#endif
#if 1
     std::string moving = "mv rose_" + fileName + ".C rose_" + fileName + "_identity.C" ;
 
     system ( moving.c_str() );
#endif

#if 0
     AST_FILE_IO::printListOfPoolSizes() ;
#endif

#if 1
  // DQ (12/12/2009): Allow output only when run in verbose mode to limit spew in testing.
     if (SgProject::get_verbose() > 0)
        {
          AST_FILE_IO::printListOfPoolSizes() ;

          int memoryUsageSize = memoryUsage();
          printf ("Alternative output from memoryUsage() = %d \n",memoryUsageSize);

          printf ("Calling AstNodeStatistics::traversalStatistics(project_test) \n");
          std::cout << AstNodeStatistics::traversalStatistics(project_test);

          printf ("Calling AstNodeStatistics::IRnodeUsageStatistics \n");
          std::cout << AstNodeStatistics::IRnodeUsageStatistics();
        }
#endif

#if 1
     std :: cout  << std::endl<< "Here we call the AST_FILE_IO :: start_up ..." << std::endl;
     AST_FILE_IO :: startUp( project_test ) ;
     std :: cout  << std::endl<< "AST_FILE_IO :: start_up done ... " << std::endl;
#endif

#if 1
  // DQ (12/12/2009): Allow output only when run in verbose mode to limit spew in testing.
     if (SgProject::get_verbose() > 0)
        {
          AST_FILE_IO::printListOfPoolSizes() ;
#if 0
          int memoryUsageSize = memoryUsage();
          printf ("Alternative output from memoryUsage() = %d \n",memoryUsageSize);

          printf ("Calling AstNodeStatistics::traversalStatistics(project_test) \n");
          std::cout << AstNodeStatistics::traversalStatistics(project_test);

          printf ("Calling AstNodeStatistics::IRnodeUsageStatistics \n");
          std::cout << AstNodeStatistics::IRnodeUsageStatistics();
#endif
        }
#endif


#if 0 
  // This will move everything into the EasyStorageClasses and then reconstruct 
  // it from scratch. As a result it is a good test without and File I/O.
     std :: cout  << std::endl<< "Here we call the AST_FILE_IO :: compressAstInMemoryPool() ..." << std::endl;
     AST_FILE_IO :: compressAstInMemoryPool();
     std :: cout  << std::endl<< "AST_FILE_IO :: compressAstInMemoryPool() done ... " << std::endl;
#endif
 
#if 1 
     std :: cout  << std::endl<< "Here we call the AST_FILE_IO :: writeASTToFile ..." << std::endl;
     AST_FILE_IO :: writeASTToFile ( fileName + ".binary" );
     std :: cout  << std::endl<< "AST_FILE_IO :: writeASTToFile done ... " << std::endl;
#endif

  // DQ (6/24/2006): Test just this parts first
  // return 0;

#if 1
     std :: cout  << std::endl<< "Here we delete the memory pools and prepare them for reading back the ast the file IO " << std::endl;

     AST_FILE_IO :: clearAllMemoryPools( );

     std :: cout  << std::endl<< "Here we call the AST_FILE_IO :: readASTFromFile ..." << std::endl;
     project_test = (SgProject*) (AST_FILE_IO :: readASTFromFile ( fileName + ".binary" ) );
     std :: cout  << std::endl<< "AST_FILE_IO :: readASTFromFile done ... " << std::endl;

  // AST_FILE_IO::printListOfPoolSizes() ;
#endif

#if 0
     std::cout << "Resetting the memory pools to hold a valid linked list ... " << std::flush;
     AST_FILE_IO :: resetValidAstAfterWriting( );
     std::cout << "done " << std::endl;
#endif

#if 0
     std :: cout << "Starting AstTests ...... "   << flush;
     AstTests::runAllTests(project_test);
     std :: cout << "done "   << std::endl;
#endif
 
#if 0
     std::cout << "Second try ... " << std::flush;
     AST_FILE_IO::setStaticDataOfAst(0);
     AstTests::runAllTests(project_test);
     std :: cout << "done "   << std::endl;
#endif

#if 1
     std :: cout  << std::endl<< " Writing file ......." << std::endl;
     backend(project_test);
     std :: cout  << std::endl<< " File to disk ! " << std::endl;
#endif

#if 1
     std::string diff = "diff  rose_"+ fileName + ".C rose_" + fileName + "_identity.C" ;
  
     if ( system ( diff.c_str() ) == 0 ) 
        {
          std :: cout << "!!!!!!!!!!!!!  files are identical !!!!!!!!!!!!"   << std::endl;
        }
       else 
        {
          std :: cout << "********* Problem: Files seem not to match ****"   << std::endl;
          exit ( -1 );
        }
#endif

     return 0;
   }                                                                                                                                                                                                     
