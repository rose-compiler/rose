#include "rose.h"
#include "upc_translation.h" // remove this later on
#include <iostream>
using namespace std;

using namespace SageInterface;
using namespace SageBuilder;
using namespace upcTranslation;

int
main (int argc, char *argv[])
{
  SgProject *project = frontend (argc, argv);

  ROSE_ASSERT (project != NULL);

  // check if it is UPC source file
  bool isUPC = SageInterface::is_UPC_language(); 
  if (isUPC == false) 
  { 
    // linking stage processing for summarizing per-file allocs()/inits()
    transProject(project); 
    return backend(project);
  }
  //Initialize the translation environment, mostly setting up data sizes
  initUpcTranslation(); 

  //process SgFile one by one
  SgFilePtrList file_list = project->get_fileList();
  SgFilePtrList::iterator iter;
  for (iter= file_list.begin(); iter!=file_list.end(); iter++)
  {
    //SgFile* cur_file = *iter;
    SgSourceFile* cur_file= isSgSourceFile(*iter);
    SgGlobal* global_scope = cur_file->get_globalScope();
    pushScopeStack(global_scope);

    //1. Preorder translation process for shared and unshared data 
    addHeadersAndPreprocessorDecls(global_scope);
   // prototypes for helper functions   
    addHelperFunctionPrototypes(cur_file);
    transStaticallyAllocatedData(cur_file->get_globalScope());


    //2. Postorder translation process for UPC language constructs
    translationDriver driver;
    // postorder is better for translator, newly generated subtrees will not be visited. 
    //driver.traverseWithinFile(cur_file,preorder);
    driver.traverseWithinFile(cur_file,postorder);

    //3. Special handling for files with main() 
    // rename main() to user_main()
    SgFunctionDeclaration * mainFunc = findMain(cur_file);
    if (mainFunc) 
    {
      renameMainToUserMain(mainFunc);
      // Let the magic source file have this
      //generateNewMainFunction(cur_file);
    }

    // change output file's suffix to .c when necessary 
    // consider from .upc to .c for now
    setOutputFileName(cur_file); 
  } // end for SgFile

 
  AstTests::runAllTests(project);
  // project->unparse();
  return backend (project);
}
