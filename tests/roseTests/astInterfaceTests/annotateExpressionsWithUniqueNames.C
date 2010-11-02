#include "rose.h"
using namespace std;
using namespace SageInterface;

class visitorTraversal : public AstSimpleProcessing
{
  protected:
    void virtual visit (SgNode* n)
    {
      SgExpression* exp = isSgExpression(n);
      if (exp != NULL)
      {
        AstAttribute* att = exp->getAttribute("UniqueNameAttribute"); 
        ROSE_ASSERT (att!= NULL);
        UniqueNameAttribute* u_att = dynamic_cast<UniqueNameAttribute* > (att);
        ROSE_ASSERT (u_att!= NULL);
        cout<<"Found expression of type "<< n->class_name()<< " : "<< u_att->get_name()<<endl;
      }
    }
};


int main ( int argc, char** argv )
{
  SgProject *project = frontend (argc, argv);
  SageInterface::annotateExpressionsWithUniqueNames(project);
  visitorTraversal my_traversal;
  my_traversal.traverseInputFiles(project,preorder);

  return backend (project);
}

