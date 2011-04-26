/*
 * Parsing OpenMP pragma text
by Liao, 9/17/2008
Last Modified: 9/19/2008
*/
#include "rose.h"
#include <iostream>
#include <string>
#include "OmpAttribute.h"
using namespace std;
using namespace OmpSupport;

class visitorTraversal : public AstSimpleProcessing
{
  protected: 
    virtual void visit(SgNode* n);
};

void visitorTraversal::visit(SgNode* node)
{
  // Some OmpAttribute may be attached redundantly,
  // e.g., omp for is attached to both the pragma and the loop. 
  OmpAttributeList* attributelist = getOmpAttributeList(node);
  if (attributelist)
  {
    cout<<"Found att attached to "<<node<<" "<<node->class_name()<<" at line "
      <<node->get_file_info()->get_line()<<endl;
    attributelist->print();//debug only for now
  }
#if 1  
  // Show OpenMP nodes also for -rose:openmp:ast_only
  if (SageInterface::isOmpStatement(node))
  {
    cout<<"----------------------------"<<endl;
    cout<<"Found a SgOmp Statement:"<<node->class_name()<<"@"<<node<<endl;
  //  cout<<isSgStatement(node)->unparseToString()<<endl;; 
    cout<<"file info: @"<< isSgStatement(node)->get_file_info() <<":"<<isSgStatement(node)->get_file_info()->get_filename();
    cout<<":"<<isSgStatement(node)->get_file_info()->get_line()<<endl;
// We now enforce OmpStatement nodes to have real file info objecs.
    ROSE_ASSERT(isSgStatement(node)->get_file_info()->isTransformation()==false);
  }
#endif  
}

// must have argc and argv here!!
int main(int argc, char * argv[])
{
  SgProject *project = frontend (argc, argv);

  AstTests::runAllTests(project);

  visitorTraversal myvisitor;
  myvisitor.traverseInputFiles(project,preorder);
  
  return backend(project);
}
