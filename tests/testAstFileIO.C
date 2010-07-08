// this test program reads the input file to generate an AST, and then:
//   1) Generates an output source file (as the initial source file).
//   2) Sets up the file I/O support
//   3) Writes the AST to disk
//   4) Clears the existing memory pools
//   5) Reads the AST from disk
//   6) Generates the source file from the AST (using backendGeneratesSourceCodeButCompilesUsingOriginalInputFile())
//   7) Compares the initial source frile with the second source file using diff.

#include "rose.h"

#define DEBUG_FILE_IO 0
 
using namespace std;

void testAST( SgProject* project )
   {
     class Traversal : public SgSimpleProcessing
        {
          public:
               Traversal() {}
               void visit ( SgNode* n )
                  {
                    SgLocatedNode* locatedNode = isSgLocatedNode(n); 
                    if (locatedNode != NULL)
                       {
                         AttachedPreprocessingInfoType* comments = locatedNode->getAttachedPreprocessingInfo();

                         if (comments != NULL)
                            {
                              printf ("Found attached comments (at %p of type: %s): \n",locatedNode,locatedNode->sage_class_name());
                              AttachedPreprocessingInfoType::iterator i;
                              for (i = comments->begin(); i != comments->end(); i++)
                                 {
                                   ROSE_ASSERT ( (*i) != NULL );
                                   printf ("          Attached Comment (relativePosition=%s): %s\n",
                                        ((*i)->getRelativePosition() == PreprocessingInfo::before) ? "before" : "after",
                                        (*i)->getString().c_str());
#if 1
                                // This does not appear to be a valid object when read in from an AST file.
                                   printf ("Comment/Directive getNumberOfLines = %d getColumnNumberOfEndOfString = %d \n",(*i)->getNumberOfLines(),(*i)->getColumnNumberOfEndOfString());
#endif
#if 0
                                // This does not appear to be a valid object when read in from an AST file.
                                   (*i)->get_file_info()->display("comment/directive location");
#endif
                                 }
                            }
                       }
                  }
        };

     Traversal counter;
     counter.traverse(project,preorder);
   }


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
  // backendGeneratesSourceCodeButCompilesUsingOriginalInputFile(project_identity);

#if DEBUG_FILE_IO
     printf ("project_identity->get_compileOnly()    = %s \n",project_identity->get_compileOnly() ? "true" : "false");
     printf ("project_identity->get_Cxx_only()       = %s \n",project_identity->get_Cxx_only() ? "true" : "false");
     printf ("project_identity->get_outputFileName() = %s \n",project_identity->get_outputFileName().c_str());
#endif

  // We need this to avoid some cases that are common in configure tests but it not halded for the AST file I/O.
  // if (project_identity->get_Cxx_only() == true && project_identity->get_outputFileName() != "a.out")
  // if (project_identity->get_compileOnly() == true && project_identity->get_Cxx_only() == true && project_identity->get_outputFileName() != "a.out")
     if (project_identity->get_compileOnly() == true && project_identity->get_Cxx_only() == true)
        {
       // Debugging support.
       // project_identity->display("testAstFileIO");

          status = backendGeneratesSourceCodeButCompilesUsingOriginalInputFile(project_identity);
          ROSE_ASSERT(status == 0);

          ROSE_ASSERT(project_identity->numberOfFiles() == 1);
          SgFile* file = project_identity->operator[](0);
          ROSE_ASSERT(file != NULL);

       // string fileName = project_identity->get_outputFileName();
          string fileName      = file->get_unparse_output_filename();
          string inputFileName = file->get_sourceFileNameWithoutPath();

       // Save the generated source file to be the original file (to be used to compare against later).
       // string moving = "mv rose_" + fileName + ".C rose_" + fileName + "_identity.C";
          string moving = "mv " + fileName + " " + fileName + "_identity";
       // printf ("moving = %s \n",moving.c_str());

          status = system ( moving.c_str() );
          ROSE_ASSERT(status == 0);

#if DEBUG_FILE_IO
          printf ("BEFORE WRITING AST: Size of AST = %d \n",numberOfNodes());
#endif
       // testAST(project_test);

#if 0
  // Debugging information
     AST_FILE_IO::printListOfPoolSizes() ;
#endif

#if 0
  // DQ (2/26/2010): Output an example of the value of p_freepointer for debugging.
     printf ("project_test->get_freepointer()  = %p \n",project_test->get_freepointer());
  // printf ("AST_FILE_IO::vectorOfASTs.size() = %zu \n",AST_FILE_IO::vectorOfASTs.size());
     AST_FILE_IO::display("Before writing the AST");
#endif

#if DEBUG_FILE_IO
     printf ("Calling File I/O initialization... \n");
#endif
     AST_FILE_IO::startUp( project_test );

  // DQ (12/12/2009): Allow output only when run in verbose mode to limit spew in testing.
     if (SgProject::get_verbose() > 0)
          AST_FILE_IO::printListOfPoolSizes() ;

#if 0
  // This will move everything into the EasyStorageClasses and then reconstruct 
  // it from scratch. As a result it is a good test without and File I/O.
     AST_FILE_IO::compressAstInMemoryPool();
#endif

#if DEBUG_FILE_IO
     printf ("Writing the AST to disk... \n");
#endif
     AST_FILE_IO::writeASTToFile ( inputFileName + ".binary" );

  // delete the memroy pools and prepare them for reading back the ast the file IO
#if DEBUG_FILE_IO
     printf ("Clear existing memory pools... \n");
#endif
     AST_FILE_IO::clearAllMemoryPools( );

#if DEBUG_FILE_IO
     printf ("Read in the AST from disk... \n");
#endif
     project_test = (SgProject*) (AST_FILE_IO :: readASTFromFile ( inputFileName + ".binary" ) );

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

#if DEBUG_FILE_IO
     printf ("AFTER READING AST: Size of AST = %d \n",numberOfNodes());
#endif

  // testAST(project_test);

#if DEBUG_FILE_IO
     printf ("Writing the second (generated source file from AST read from disk)... \n");
#endif
  // backend(project_test);
     status = backendGeneratesSourceCodeButCompilesUsingOriginalInputFile(project_test);
     ROSE_ASSERT(status == 0);

#if 1
#if DEBUG_FILE_IO
     printf ("Test initial source file against second source file using diff... \n");
#endif
  // string diff = "diff  rose_"+ fileName + ".C rose_" + fileName + "_identity.C" ;  
     string diff = "diff "+ fileName + " " + fileName + "_identity";
     status = system ( diff.c_str() );
#if DEBUG_FILE_IO
     if ( status == 0 ) 
        {
          cout << "!!!!!!!!!!!!!  files are identical !!!!!!!!!!!!"   << endl;
        }
       else 
        {
          cout << "********* Problem: Files seem not to match ****"   << endl;
       // exit ( -1 );
        }
#endif
#endif
        }
       else
        {
       // return backendGeneratesSourceCodeButCompilesUsingOriginalInputFile(project_identity);
#if DEBUG_FILE_IO
          printf ("***** Calling backend without any File IO testing ***** \n");
#endif
          status = backendCompilesUsingOriginalInputFile(project_identity);
        }

  // return 0;
     return status;
   }                                                                                                                                                                                                     
