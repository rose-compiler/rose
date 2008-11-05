#include <iostream>
#include <string>
#include <vector>
#include "abstract_handle.h"
#include "myloop.h"
#include "loopAdapter.h"

using namespace std;
using namespace AbstractHandle;

int main()
{
  //-------------Preparing the internal loop representation---------
  // declare and initialize a list of loops using MyLoop  
  // The loop tool should be able to generate its representation from 
  // source code somehow. We fill it up manually here. 
  vector <MyLoop* > loops;

  MyLoop loop1, loop2, loop3;
  loop1.sourceFileName="file1.c";
  loop1.line_number = 7;
  loop1.parent = NULL;
  loop2.sourceFileName="file1.c";
  loop2.line_number = 8;
  loop2.parent=&loop1;
  loop1.children.push_back(&loop2);
  loop3.sourceFileName="file1.c";
  loop3.line_number = 12;
  loop3.parent=NULL;
  loops.push_back(&loop1);
  loops.push_back(&loop3);

  //------------------ using abstract handles-------------
  //Generate the abstract handle  for the source file
  fileNode* filenode = new fileNode("file1.c");
  filenode->setMLoops(loops);
  abstract_handle* file_handle = new abstract_handle(filenode);
  cout<<"Created a file handle:"<<endl<<file_handle->toString()<<endl;

  //Create a loop handle within the file using numbering info.
  abstract_node* loop_node1= new loopNode(&loop1);
  abstract_handle* loop_handle1= new abstract_handle(loop_node1,e_numbering,file_handle);
  cout<<"Created a loop handle:"<<endl<<loop_handle1->toString()<<endl;

  //Create another loop handle within a file using its source position information
  string input1("ForStatement<position,12>");
  abstract_handle* loop_handle2= new abstract_handle(file_handle,input1); 
  cout<<"Created a loop handle:"<<endl<<loop_handle2->toString()<<endl;

  //Create yet another loop handle within a loop using its relative numbering information
  string input2("ForStatement<numbering,1>");
  abstract_handle* loop_handle3= new abstract_handle(loop_handle1,input2); 
  cout<<"Created a loop handle:"<<endl<<loop_handle3->toString()<<endl;

  return 0;
}
