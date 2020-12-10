// This translator reads the input file, and then builds a second AST of the same 
// input file using a different name and addes the file to the file list.
// This test allows us to test the reading of a file a second time and also
// test the support for transformations of the functions defined in the second 
// file into prototypes.  This support is required for the outlining feasture 
// that outlines generated functions into a seperate file.


#include "rose.h"

using namespace std;

string getFileExt(const string& s) 
   {
     size_t i = s.rfind('.', s.length());
     if (i != string::npos)
        {
          return(s.substr(i+1, s.length() - i));
        }

     return("");
   }


// SgSourceFile* getLibSourceFile(SgBasicBlock* target) 
SgSourceFile*
getLibSourceFile(SgSourceFile* target) 
   {
     SgProject * project = SageInterface::getEnclosingNode<SgProject> (target);
     ROSE_ASSERT(project != NULL);

  // SgFile* input_file = getEnclosingNode<SgFile> (target);
     SgFile* input_file = target;
     ROSE_ASSERT(input_file != NULL);

     string input_file_name = input_file->get_file_info()->get_filenameString();

  // string new_file_name =   generateLibSourceFileName (target);
  // string new_file_name =   "second_file_" + input_file_name;

  // string new_file_name = string("second_file_") + Rose::utility_stripPathFromFileName(input_file_name);

     string filenameWithoutPath = Rose::utility_stripPathFromFileName(input_file_name);
     printf ("filename without path: filenameWithoutPath = %s \n",filenameWithoutPath.c_str());

    size_t lastindex = filenameWithoutPath.find_last_of("."); 
    ROSE_ASSERT(lastindex != std::string::npos);

    string filenameWithoutExtension = filenameWithoutPath.substr(0, lastindex);

     printf ("filenameWithoutExtension = %s \n",filenameWithoutExtension.c_str());

#if 0
     printf ("Exit as a test! \n");
     ROSE_ASSERT(false);
#endif

 // Get the actual filename extension.
 // std::string filenameExtension = ".C";
     std::string filenameExtension = getFileExt(filenameWithoutPath);

     printf ("filename without extension: filenameWithoutExtension = %s \n",filenameWithoutExtension.c_str());

     std::string filenamePath = Rose::getPathFromFileName(input_file_name);

     string new_file_name = filenamePath + "/" + filenameWithoutExtension + "_2ndFile"+ "." + filenameExtension;

  // new_file_name = filenamePath + "/" + new_file_name;

     printf ("filename with path: new_file_name    = %s \n",new_file_name.c_str());

#if 0
     printf ("Exit as a test! \n");
     ROSE_ASSERT(false);
#endif

  // Search if the lib file already exists. 
     SgFilePtrList file_list = project->get_files();
     SgFilePtrList::iterator iter;

  // cout<<"debugging: getLibSourceFile(): checking current file list of count of "<<file_list.size() <<endl;

  // This code finds the new file if it already exists (we want to be able to reuse it).
     SgSourceFile* new_file = NULL;
     for (iter = file_list.begin(); iter!=file_list.end(); iter++)
        {
          SgFile* cur_file = *iter;
          SgSourceFile * sfile = isSgSourceFile(cur_file);
          if (sfile != NULL)
             {
               std::string cur_file_name =sfile->get_file_info()->get_filenameString();
            // cout<<"\t Debug: compare cur vs. new file name:"<<cur_file_name <<" vs. " << new_file_name <<endl;
               if (cur_file_name == new_file_name)
                  {
                    new_file = sfile;
                    break;
                  }
             }
        } // end for SgFile

     printf ("new_file = %p \n",new_file);
    
     if (new_file == NULL)
        {
       // par1: input file, par 2: output file name, par 3: the project to attach the new file
       // to simplify the lib file generation, we copy entire original source file to it, then later append outlined functions
          new_file = isSgSourceFile(SageBuilder::buildSourceFile(input_file_name, new_file_name, project));
       // buildFile() will set filename to be input file name by default. 
       // we have to rename the input file to be output file name. This is used to avoid duplicated creation later on
          new_file->get_file_info()->set_filenameString(new_file_name);

#if 0
       // DQ (3/28/2019): The conversion of functions with definitions to function prrototypes must preserve the 
       // associated comments and CPP directives (else the #includes will be missing and types will not be defined.
       // This is an issue with the astOutliner test code jacobi.c.
       // DQ (3/20/2019): Need to eliminate possible undefined symbols in this file when it will be compiled into 
       // a dynamic shared library.  Any undefined symbols will cause an error when loading the library using dlopen().
       // convertFunctionDefinitionsToFunctionPrototypes(new_file);
          SageInterface::convertFunctionDefinitionsToFunctionPrototypes(new_file);
#endif
        }

  // new_file = isSgSourceFile(buildFile(new_file_name, new_file_name));
     ROSE_ASSERT(new_file != NULL);
     return new_file;
   }

int main( int argc, char * argv[] )
   {
  // DQ (3/5/2017): This will not fail if we skip calling ROSE_INITIALIZE (but the test for Rose::Diagnostics::isInitialized() 
  // is then required, however it will fail to output message that we enable explicitly below.
  // Initialize and check compatibility. See Rose::initialize
     ROSE_INITIALIZE;

  // Generate the ROSE AST (note ROSE_INITIALIZE will be called by the frontend, if not called explicitly above in main()).
     SgProject* project = frontend(argc,argv);

  // AST consistency tests (optional for users, but this enforces more of our tests)
     AstTests::runAllTests(project);

     SgSourceFile* firstFile = isSgSourceFile(project->get_files()[0]);
     ROSE_ASSERT(firstFile != NULL);
     SgSourceFile* secondFile = getLibSourceFile(firstFile); 

  // AST consistency tests (optional for users, but this enforces more of our tests)
     AstTests::runAllTests(project);

#if 0
  // DQ (9/8/2017): Debugging ROSE_ASSERT.
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

#if 1
  // Output an optional graph of the AST (just the tree, when active)
     printf ("Generating a dot file... (ROSE Release Note: turn off output of dot files before committing code) \n");
     generateDOT ( *project );
     // generateAstGraph(project, 2000);
#endif

#if 1
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 10000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
#endif

#if 0
     SgNode::get_globalTypeTable()->print_typetable();
#endif

#if 0
  // DQ (9/8/2017): Debugging ROSE_ASSERT.
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

  // regenerate the source code and call the vendor 
  // compiler, only backend error code is reported.
     return backend(project);
   }
