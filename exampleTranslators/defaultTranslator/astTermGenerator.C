// A translator to dump AST terms within an input source file
// Liao
#include "rose.h"
#include "AstTerm.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>
using namespace std;

class visitorTraversal:public AstSimpleProcessing
{ 
  public:
    virtual void visit (SgNode * n);
};

void 
//visitorTraversal::visit (SgNode * n)
visit (SgNode * n, ofstream& out)
{    
  if (SgDeclarationStatement* decl= isSgDeclarationStatement(n))
  {
    
    string filename= decl->get_file_info()->get_filename();
    string suffix = Rose::StringUtility ::fileNameSuffix(filename);

    //vector.tcc: This is an internal header file, included by other library headers
    if (suffix=="h" ||suffix=="hpp"|| suffix=="hh"||suffix=="H" ||suffix=="hxx"||suffix=="h++" ||suffix=="tcc")
      return ;

    // also check if it is compiler generated. Not from user code
    // skip compiler generated codes, mostly from template headers
    if (decl->get_file_info()->isCompilerGenerated())
      return;

    out<< AstTerm::astTermWithNullValuesToString(decl)<<endl;
  }
}

int
main (int argc, char *argv[])
{
  // DQ (4/6/2017): This will not fail if we skip calling ROSE_INITIALIZE (but
  // any warning message using the message looging feature in ROSE will fail).
  ROSE_INITIALIZE;

  SgProject *project = frontend (argc, argv);
  //  visitorTraversal exampleTraversal;
  //  exampleTraversal.traverse (project, preorder);
  if (project->get_fileList().size() >=1)
  {
    SgFilePtrList file_list = project->get_fileList();
    std::string firstFileName = Rose::StringUtility::stripPathFromFileName(file_list[0]->getFileName());

   ofstream out;
    out.open(firstFileName+".astTerm.txt");
    //out<< AstTerm::astTermWithNullValuesToString(project)<<endl; // too much from headers
    SgGlobal* global= isSgSourceFile(file_list[0])->get_globalScope();
    SgDeclarationStatementPtrList& decl_list = global->get_declarations();

    for (int i =0; i<  decl_list.size(); i++)
    {
      visit (decl_list[i], out);
    }

    out.close();
  }

  //  visitorTraversal exampleTraversal;
  //  exampleTraversal.traverse (project, preorder);


  return backend (project);
}

