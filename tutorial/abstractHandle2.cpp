/*
Example code to generate language handles from input strings about
* source position information
* numbering information

by Liao, 10/9/2008
*/
#include "rose.h"
#include <iostream>
#include <string.h>
#include "abstract_handle.h"
#include "roseAdapter.h"

using namespace std;
using namespace AbstractHandle;

int main(int argc, char * argv[])
{
  SgProject *project = frontend (argc, argv);

  // Generate a file handle from the first file of the project
  abstract_node* file_node= new roseNode((project->get_fileList())[0]);
  abstract_handle* handle1 = new abstract_handle(file_node);
  cout<<"Created a file handle:\n"<<handle1->toString()<<endl<<endl;;

  //Create a handle to a namespace given its name and parent handle 
  string input1("SgNamespaceDeclarationStatement<name,space1>");
  abstract_handle* handle0 = new abstract_handle(handle1,input1);
  cout<<"Created a handle:\n"<<handle0->toString()<<endl<<endl;
  cout<<"It points to:\n"<<handle0->getNode()->toString()<<endl<<endl;

  // Create a handle within the file, given a string specifying  
  // its construct type (class declaration) and source position 
  string input("SgClassDeclaration<position,4.3-9.2>");
  abstract_handle* handle2 = new abstract_handle(handle1,input);

  cout<<"Created a handle:\n"<<handle2->toString()<<endl<<endl;
  cout<<"It points to:\n"<<handle2->getNode()->toString()<<endl<<endl;;

  // find the second function declaration within handle2
  abstract_handle handle3(handle2,"SgFunctionDeclaration<numbering,2>");

  cout<<"Created a handle:\n"<<handle3.toString()<<endl<<endl;
  cout<<"It points to:\n"<<handle3.getNode()->toString()<<endl;

// Generate source code from AST and call the vendor's compiler
  return backend(project);
}
