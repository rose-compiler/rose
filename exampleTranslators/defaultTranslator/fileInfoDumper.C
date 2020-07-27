// A translator to dump Sg_File_Info information within an input source file
// Liao
#include "rose.h"
using namespace std;

class visitorTraversal:public AstSimpleProcessing
{
  public:
    virtual void visit (SgNode * n);
};

void
visitorTraversal::visit (SgNode * n)
{
  if (SgStatement* stmt = isSgStatement(n))
  {
    cout<<"-------------------------"<<endl;
    cout<<"Start and end file info for a SgStatement of type:\t"<< n->class_name()<<endl;
    cout<<stmt->get_startOfConstruct()->get_filename();;
    cout<<":"<<stmt->get_startOfConstruct()->get_line()<<endl;;
    cout<<stmt->get_endOfConstruct()->get_filename();
    cout<<":"<<stmt->get_endOfConstruct()->get_line()<<endl;
  }
}

int
main (int argc, char *argv[])
{
  // DQ (4/6/2017): This will not fail if we skip calling ROSE_INITIALIZE (but
  // any warning message using the message looging feature in ROSE will fail).
  ROSE_INITIALIZE;

  SgProject *project = frontend (argc, argv);
  visitorTraversal exampleTraversal;
  exampleTraversal.traverse (project, preorder);
  return backend (project);
}

