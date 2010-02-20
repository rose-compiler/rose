// this test program reads the input file to generate an AST, and then:
//   1) Generates an output source file (as the initial source file).
//   2) Sets up the file I/O support
//   3) Writes the AST to disk
//   4) Clears the existing memory pools
//   5) Reads the AST from disk
//   6) Generates the source file from the AST (using backendGeneratesSourceCodeButCompilesUsingOriginalInputFile())
//   7) Compares the initial source frile with the second source file using diff.

#include "rose.h"
 
using namespace std;

int
main ( int argc, char * argv[] )
   {
     TimingPerformance timer ("main execution time (sec) = ",true);
     SgProject* project_identity = frontend(argc,argv);
     SgProject* project_test = project_identity;
     ROSE_ASSERT (project_identity != NULL);

     int status = 0;

  // DQ (2/20/2010): Autoconf configure tests can't hae I/O like this...
  // printf ("project_identity->get_outputFileName() = %s \n",project_identity->get_outputFileName().c_str());
  // printf ("Writing the initial file to disk... \n");

  // backend(project_identity);
     backendGeneratesSourceCodeButCompilesUsingOriginalInputFile(project_identity);

  // We need this to avoid some cases that are common in configure tests but it not halded for the AST file I/O.
     if (project_identity->get_compileOnly() == true && project_identity->get_Cxx_only() == true && project_identity->get_outputFileName() != "a.out")
        {
     string fileName = project_identity->get_outputFileName();
  // Save the generated source file to be the original file (to be used to compare against later).
     string moving = "mv rose_" + fileName + ".C rose_" + fileName + "_identity.C";
     status = system ( moving.c_str() );
     ROSE_ASSERT(status == 0);

#if 0
  // Debugging information
     AST_FILE_IO::printListOfPoolSizes() ;
#endif

     printf ("Calling File I/O initialization... \n");
     AST_FILE_IO::startUp( project_test ) ;

  // DQ (12/12/2009): Allow output only when run in verbose mode to limit spew in testing.
     if (SgProject::get_verbose() > 0)
          AST_FILE_IO::printListOfPoolSizes() ;

#if 0
  // This will move everything into the EasyStorageClasses and then reconstruct 
  // it from scratch. As a result it is a good test without and File I/O.
     AST_FILE_IO::compressAstInMemoryPool();
#endif

     printf ("Writing the AST to disk... \n");
     AST_FILE_IO::writeASTToFile ( fileName + ".binary" );

  // delete the memroy pools and prepare them for reading back the ast the file IO
     printf ("Clear existing memory pools... \n");
     AST_FILE_IO::clearAllMemoryPools( );

     printf ("Read in the AST from disk... \n");
     project_test = (SgProject*) (AST_FILE_IO :: readASTFromFile ( fileName + ".binary" ) );

  // AST_FILE_IO::printListOfPoolSizes();

#if 0
     cout << "Resetting the memory pools to hold a valid linked list ... " << flush;
     AST_FILE_IO::resetValidAstAfterWriting( );
     cout << "done " << endl;
#endif

#if 0
     cout << "Staring AstTests ...... " << flush;
     AstTests::runAllTests(project_test);
     cout << "done " << endl;
#endif
 
#if 0
     cout << "Second try ... " << flush;
     AST_FILE_IO::setStaticDataOfAst(0);
     AstTests::runAllTests(project_test);
     cout << "done " << endl;
#endif

     printf ("Writing the second (generated source file from AST read from disk)... \n");
  // backend(project_test);
     backendGeneratesSourceCodeButCompilesUsingOriginalInputFile(project_test);

#if 1
     printf ("Test initial source file against second source file using diff... \n");
     string diff = "diff  rose_"+ fileName + ".C rose_" + fileName + "_identity.C" ;  
     if ( system ( diff.c_str() ) == 0 ) 
        {
          cout << "!!!!!!!!!!!!!  files are identical !!!!!!!!!!!!"   << endl;
        }
       else 
        {
          cout << "********* Problem: Files seem not to match ****"   << endl;
          exit ( -1 );
        }
#endif
        }

     return 0;
   }                                                                                                                                                                                                     
