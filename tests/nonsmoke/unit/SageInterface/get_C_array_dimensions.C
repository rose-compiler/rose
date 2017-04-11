#include "rose.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;
using namespace SageInterface;

ofstream ofile; 

class visitorTraversal : public AstSimpleProcessing
{
  protected:
    virtual void visit(SgNode* n);
};

void visitorTraversal::visit(SgNode* node)
{
  if (SgLocatedNode* lnode = isSgLocatedNode(node))
  {
    //skip system headers
    if (insideSystemHeader (lnode))
      return; 

    if (SgVariableDeclaration* decl= isSgVariableDeclaration(node))
    {
      ofile<<"variable declaration at line "<< decl->get_file_info()->get_line() <<endl; 
      SgInitializedName* iname = getFirstInitializedName (decl);

      if (iname != NULL)
      {
        SgArrayType* atype = isSgArrayType (iname->get_type());
        if (atype != NULL)
        {
          vector <SgExpression* > dims = get_C_array_dimensions (*atype, *iname);
          for (size_t i=0; i< dims.size(); i++)
          {
            // Must redirect to a .output file to enable diff-based correctness checking and avoid screen spewing interruptions. 
            ofile<< dims[i]->unparseToString() <<endl; 
          }
        }
      }
    }
  }
}

int main(int argc, char * argv[])
{
  SgProject *project = frontend (argc, argv);
  
  SgFilePtrList fl = project->get_files();
  SgFile* firstfile = fl[0];
  ROSE_ASSERT (firstfile!=NULL);
  
  string filename = rose::StringUtility::stripPathFromFileName (firstfile->getFileName());
  string ofilename = filename+".get_C_array_dimensions.output";
  ofile.open(ofilename.c_str());
  visitorTraversal myvisitor;
  myvisitor.traverseInputFiles(project,preorder);
  ofile.close();

  return backend(project);
}

