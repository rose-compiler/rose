// Example ROSE Translator used for testing ROSE infrastructure this 
// tests the ability of ROSE to process the source code and generate
// just the token stream. Then we the original input file and use the 
// vendor compiler to generate the object file or executable.
#include "rose.h"

int
main( int argc, char * argv[] )
   {
  // Introduces tracking of performance of ROSE at the top most level.
     TimingPerformance timer ("AST translation (main): time (sec) = ",true);

  // Build a vector of strings to represent the command line arguments.
     std::vector<std::string> sourceCommandline = std::vector<std::string>(argv, argv + argc);
     sourceCommandline.push_back("-rose:output_tokens");

     SgProject* project = frontend(sourceCommandline);

     AstTests::runAllTests(project);

     SgFilePtrList & fileList = project->get_fileList();
     for (size_t i=0; i < fileList.size(); i++)
        {
          SgSourceFile* sourceFile = isSgSourceFile(fileList[i]);
          if (sourceFile != NULL)
             {
               size_t numberOfTokens = sourceFile->get_token_list().size();
               printf ("Number of tokens in file %s = %zu \n",sourceFile->get_sourceFileNameWithPath().c_str(),numberOfTokens);
               if (numberOfTokens == 0)
                  {
                 // We output an error, but since this test is only presently valid for fortran files it is not serious.
                    if (sourceFile->get_Fortran_only() == true)
                       {
                         printf ("Warning: numberOfTokens in file equal zero (could be an error). \n");
                       }
                      else
                       {
                         printf ("Warning: this test is only valid for Fortran files at present. \n");
                       }
                  }
             }
            else
             {
               printf ("Warning, token evaluation only valid for source files. \n");
             }
        }

  // Output statistics about how ROSE was used...
     if (project->get_verbose() > 1)
        {
          std::cout << AstNodeStatistics::traversalStatistics(project);
          std::cout << AstNodeStatistics::IRnodeUsageStatistics();
        }

  // Just set the project, the report will be generated upon calling the destructor for "timer"
  // Use option "-rose:verbose 2" to see the report.
     timer.set_project(project);

  // Skip calling the typical backend for ROSE (not required for just testing analysis)
  // This backend calls the backend compiler using the original input source file list.
     return backendCompilesUsingOriginalInputFile(project);
   }
