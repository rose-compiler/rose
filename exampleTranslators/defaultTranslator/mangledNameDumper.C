#include "rose.h"
using namespace std;
// Must use memory pool traversal here
// AstSimple traversal will skip types
class visitorTraversal:public ROSE_VisitTraversal
{ 
  public:
    virtual void visit (SgNode * n);
};

void
visitorTraversal::visit (SgNode * n)
{
  SgType*  t_node = isSgType(n);
  if (t_node != NULL)
  {
    cout<<"Type "<< t_node->class_name()<<endl;
    cout<<"\tunparsed string is "<< t_node->unparseToString()<<endl;
    cout<<"\tmangled name is "<< t_node->get_mangled().getString()  <<endl;
  }
}

int main ( int argc, char** argv )
{
  SgProject *project = frontend (argc, argv);

  // Dump mangled map
  cout<<"----------- mangled name map -------------"<<endl;
  std::map< SgNode *, std::string > & m_map = SgNode::get_globalMangledNameMap ();
  std::map< SgNode *, std::string >::iterator iter = m_map.begin();
  for (; iter != m_map.end(); iter++)
  {
    cout<<"SgNode is "<< (*iter).first->class_name()<<"    ";
    cout<<"Mangled name is "<< (*iter).second <<endl;
  }

  // Dump mangled types
  cout<<"----------- mangled types-------------"<<endl;
  visitorTraversal exampleTraversal;
  exampleTraversal.traverseMemoryPool();

  return backend (project);
}













