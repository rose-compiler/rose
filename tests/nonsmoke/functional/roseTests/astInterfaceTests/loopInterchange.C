/*
test code for loopInterchange
by Liao, 6/15/2009
*/
#include "rose.h"
#include <string>
#include <iostream>
#include "commandline_processing.h"

using namespace std;
using namespace AbstractHandle;

int main(int argc, char * argv[])

{
  std::string handle;
  int order=1,depth=2;
  // command line processing
  //--------------------------------------------------
  vector<std::string> argvList (argv, argv+argc);
  if (!CommandlineProcessing::isOptionWithParameter (argvList,"-rose:loopInterchange:","abstract_handle",handle, true)
     || !CommandlineProcessing::isOptionWithParameter (argvList,"-rose:loopInterchange:","depth",depth, true)
     || !CommandlineProcessing::isOptionWithParameter (argvList,"-rose:loopInterchange:","order",order, true))
   {
     cout<<"Usage: loopInterchange inputFile.c -rose:loopInterchange:abstract_handle <loop_handle_string> -rose:loopInterchange:depth D -rose:loopInterchange:order N"<<endl;
     return 0;
   }

  // Retrieve corresponding SgNode from abstract handle
  //--------------------------------------------------
  SgProject *project = frontend (argvList);
  SgStatement* stmt = NULL;
  ROSE_ASSERT(project != NULL);
  SgFilePtrList & filelist = project->get_fileList();
  SgFilePtrList::iterator iter= filelist.begin();
  for (;iter!=filelist.end();iter++)
  {
    SgSourceFile* sfile = isSgSourceFile(*iter);
    if (sfile != NULL)
    {     
      // prepare a file handle first
      abstract_node * file_node = buildroseNode(sfile);
      ROSE_ASSERT (file_node);
      abstract_handle* fhandle = new abstract_handle(file_node);
      ROSE_ASSERT (fhandle);
      // try to match the string and get the statement handle
      std::string cur_handle = handle;
      abstract_handle * shandle = new abstract_handle (fhandle,cur_handle);
      // it is possible that a handle is created but no matching IR node is found
      if (shandle != NULL)
      {
        if (shandle->getNode() != NULL)
        { // get SgNode from the handle
          SgNode* target_node = (SgNode*) (shandle->getNode()->getNode());
          ROSE_ASSERT(isSgStatement(target_node));
          stmt = isSgStatement(target_node);
          break; 
        }
      }
    } //end if sfile
  } // end for
  if (stmt==NULL)
  {
    cout<<"Cannot find a matching target from a handle:"<<handle<<endl;
    return 0;
  }

  // Interchange it
  //--------------------------------------------------
 if (isSgForStatement(stmt))
  {
    bool result=false;
    result = SageInterface::loopInterchange(isSgForStatement(stmt),depth, order);
    ROSE_ASSERT(result != false);
  }

//  AstPostProcessing(project);
  // run all tests
  AstTests::runAllTests(project);

  // Generate source code from AST and call the vendor's compiler
  return backend(project);
}

