#include "rose.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "OmpSupport.h"
using namespace std;
using namespace OmpSupport;
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

    if (SgForStatement* forloop= isSgForStatement(node))
    {
      ofile<<"for loop at line "<< forloop->get_file_info()->get_line() <<endl; 
      std::vector< SgVarRefExp * > ref_vec; 
      collectVarRefs (forloop, ref_vec);
      for (std::vector< SgVarRefExp * >::iterator iter = ref_vec.begin(); iter!= ref_vec.end(); iter ++) 
      {
        SgSymbol* s = (*iter)->get_symbol();
        omp_construct_enum atr = getDataSharingAttribute (*iter);
        // will redirect to a .output file to enable diff-based correctness checking
        ofile<<s->get_name()<<"\t"<<toString(atr) <<endl; 
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
  
  string filename = Rose::StringUtility::stripPathFromFileName (firstfile->getFileName());
  string ofilename = filename+".output";
  ofile.open(ofilename.c_str());
  visitorTraversal myvisitor;
  myvisitor.traverseInputFiles(project,preorder);
  ofile.close();

  return backend(project);
}

