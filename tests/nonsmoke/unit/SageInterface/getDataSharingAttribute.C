#include "rose.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "OmpSupport.h"
using namespace std;
using namespace OmpSupport;
using namespace SageInterface;
// using a log file to avoid new screen output from interfering with correctness checking
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

// test over all relevant OpenMP regions, not just for loops now.
    //if (SgForStatement* forloop= isSgForStatement(node))
    if (SgOmpBodyStatement* omp_region = isSgOmpBodyStatement(node))
    {
      ofile<<"-----------------------------------------------"<<endl;
      ofile<<"OpenMP region "<< omp_region->class_name() <<" @ "<< omp_region->get_file_info()->get_line() <<endl; 
      std::vector< SgVarRefExp * > ref_vec; 
      SgStatement* body = omp_region->get_body();
      collectVarRefs (body, ref_vec);
      for (std::vector< SgVarRefExp * >::iterator iter = ref_vec.begin(); iter!= ref_vec.end(); iter ++) 
      {
        SgVarRefExp* var_ref = (*iter); 
        SgSymbol* s = var_ref->get_symbol();
        // omp_construct_enum  OmpSupport::getDataSharingAttribute(SgVarRefExp* varRef); 
        omp_construct_enum atr = getDataSharingAttribute (*iter);
        // write to a .output file to enable diff-based correctness checking
        ofile<<s->get_name()<<"@" <<var_ref->get_file_info()->get_line()<<":"<<var_ref->get_file_info()->get_col() <<"\t"<<toString(atr) <<endl; 
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
  string ofilename = filename+".getDataSharingAttribute.output";
  ofile.open(ofilename.c_str());
  visitorTraversal myvisitor;
  myvisitor.traverseInputFiles(project,preorder);
  ofile.close();

  return backend(project);
}

