/*
 * Parsing failsafe pragma text
by Liao, 1/29/2014
*/
#include "rose.h"
#include <iostream>
#include <string>
#include "failSafePragma.h"

using namespace std;
using namespace FailSafe;

class visitorTraversal : public AstSimpleProcessing
{
  protected:
    virtual void visit(SgNode* n);
};

void visitorTraversal::visit(SgNode* node)
{
  AttributeList* attributelist = getAttributeList(node);
  if (attributelist)
  {
    cout<<"Found FailSafe attribute attached to "<<node<<" "<<node->class_name()<<" at line "
      <<node->get_file_info()->get_line()<<endl;
    cout<<attributelist->toString()<<endl;//debug only for now
  }
#if 0  
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

