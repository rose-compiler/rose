/*

Hi Dan,

As we talked about on the phone, I have written a small main function showing that copying the SgGlobal object works well and the deep copy mechanism seems to work.
(I can output the original SgProject and the modification done to the copy of it (copy from SgGlobal) doesn't affect the original one. But I cannot output the copied object, so I cannot tell how the copy is "behaving".)

As I also want to output/unparse the copy of the SgGlobal object:
Is it the right approach to add the SgGlobal root to the file  (like file.set_root(root_copy);)? And then unparse the file/sgproject?
When I do that, I only get an error message at runtime saying:
home/hauge2/ROSE/src/backend/unparser/unparse_stmt.C:1047: void Unparser::unparseFuncDefnStmt(SgStatement*, SgUnparse_Info&): Assertion `funcdefn_stmt->get_declaration() != __null' failed.

Or is there another way of outputting the copied part?


I'm attaching the small main function (and a small testfile) with some comments showing what is working and what is not working.
In summary this is:
Not working:
- SgProject::copy and SgFile::copy (most likely for the same reason, the copy constructor of SgFile)
- unparsing a file/sgproject after setting the modified copy of the SgGlobal as the root of the file.

Working:
- SgGlobal::copy and the deep copy mechanism.


-- 
Vera

*/
#if 0
#include "rose.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif



int main(int argc, char *argv[]){
  
  SgProject* sgproject = frontend(argc, argv);
  // SgProject *sgproject_copy = static_cast<SgProject*>(sgproject->copy(SgTREE_COPY)); 
  // This copy of the sgproject fails due to the copy function of the SgFile fails (aborts)
  
  SgFile &file = sgproject->get_file(0);
  // SgFile *file_copy = static_cast<SgFile*>(file.copy(SgTREE_COPY));
  // Calling the copy function of SgFile fails:
  // Before aborting the execution, the following error message is displayed:
  // "This is the default constructor, use SgFile (argc,argv) instead"
  
  SgGlobal *root = file.get_root();
  SgGlobal *root_copy = static_cast<SgGlobal*>(root->copy(SgTREE_COPY));
  // Copying the SgGlobal object is OK
  
  // removing the return statement...from the copy!
  list<SgNode*> returnstmt_list = NodeQuery::querySubTree(root_copy, V_SgReturnStmt);
  SgStatement* stmt = isSgStatement(*returnstmt_list.begin());
  LowLevelRewrite::remove(stmt);
  
  sgproject->unparse();  // should output unmodified sgproject and it does. The deep copy
                         // mechanism works for the SgGlobal object. 

  // moving the output file to a new file, so that we can unparse the sgproject again without
  // overwriting the results
  char *outputfile = file.get_unparse_output_filename();
  string move = "mv " + string(outputfile) + " orig_" + string(outputfile); 
  system(move.c_str());

  // want to output modified sgproject
  file.set_root(root_copy);
  //sgproject->set_file(file);
  sgproject->unparse(); // or use: file.unparse();?
  
  // Unparsing the sgproject after adding the root_copy to the file-object (I want
  // to unparse the modified copy...), gives me an error message:
  //  /home/hauge2/ROSE/src/backend/unparser/unparse_stmt.C:1047: void Unparser::unparseFuncDefnStmt(SgStatement*, SgUnparse_Info&): Assertion `funcdefn_stmt->get_declaration() != __null' failed.
  
  return 0;
}




int main()
{

  int x;
  x = 4;

  return 0;
}
#endif
