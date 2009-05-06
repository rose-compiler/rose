/*
Example code to generate abstract handles for language constructs

by Liao, 10/6/2008
*/
#include "rose.h"
#include <iostream>
#include "abstract_handle.h"
#include "roseAdapter.h"
#include <string.h>

using namespace std;
using namespace AbstractHandle;

// a global handle for the current file
static abstract_handle* file_handle = NULL;

class visitorTraversal : public AstSimpleProcessing
{
  protected: 
    virtual void visit(SgNode* n);
};

void visitorTraversal::visit(SgNode* n)
{
  SgForStatement* forloop = isSgForStatement(n);

  if (forloop)
  { 
    cout<<"Creating handles for a loop construct..."<<endl;
    //Create an abstract node
    abstract_node* anode= buildroseNode(forloop);

    //Create an abstract handle from the abstract node
    //Using source position specifiers by default
    abstract_handle * ahandle = new abstract_handle(anode);
    cout<<ahandle->toString()<<endl;

    // Create handles based on numbering specifiers within the file
    abstract_handle * bhandle = new abstract_handle(anode,e_numbering,file_handle);
    cout<<bhandle->toString()<<endl<<endl;
  }  
}

int main(int argc, char * argv[])
{
  SgProject *project = frontend (argc, argv);

  //Generate a file handle
  abstract_node * file_node = buildroseNode((project->get_fileList())[0]);
  file_handle = new abstract_handle(file_node);

  //Generate handles for language constructs
  visitorTraversal myvisitor;
  myvisitor.traverseInputFiles(project,preorder);

// Generate source code from AST and call the vendor's compiler
  return backend(project);
}
