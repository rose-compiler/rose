/*
test code for creating abstract handles with qualified name style
This is useful to uniquely identify AST nodes during transformations.

by Liao, 3/6/2014
*/
#include "rose.h"
#include <string>
#include <iostream>

using namespace std;

// a global handle for the current file
//static abstract_handle* file_handle = NULL;

class visitorTraversal : public AstSimpleProcessing
{
  protected:
    virtual void visit(SgNode* n);
};

void visitorTraversal::visit(SgNode* n)
{
  SgLocatedNode* lnode= isSgLocatedNode(n);

  if (lnode)
  {
#if 0    
    cout<<"Creating handles for a loop construct..."<<endl;
    //Create an abstract node
    abstract_node* anode= buildroseNode(forloop);

    //Create an abstract handle from the abstract node
    //Using source position specifiers by default
    abstract_handle * ahandle = new abstract_handle(anode);
    cout<<ahandle->toString()<<endl;

    // Create handles based on numbering specifiers within the file
    abstract_handle * bhandle = new abstract_handle(anode,e_numbering,file_handle);
#endif
    AbstractHandle::abstract_handle * bhandle = SageInterface::buildAbstractHandle(lnode);
    cout<<bhandle->toString()<<endl<<endl;
  }
}

int main(int argc, char * argv[])
{
  SgProject *project = frontend (argc, argv);

  //Generate a file handle
#if 0
  abstract_node * file_node = buildroseNode((project->get_fileList())[0]);
  file_handle = new abstract_handle(file_node);
#endif
  //Generate handles for language constructs
  visitorTraversal myvisitor;
  myvisitor.traverseInputFiles(project,preorder);

// Generate source code from AST and call the vendor's compiler
  return backend(project);
}

