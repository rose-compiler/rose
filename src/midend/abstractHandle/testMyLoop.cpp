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
  // declare and init a list of loops using MyLoop  
  vector <MyLoop* > loops;

  MyLoop loop1, loop2;

  loop1.sourceFileName="file1.c";
  loop1.line_number = 7;
  loop1.loop_code = "for (i=0;i<100;i++) \n a[i]=0;";

  loop2.sourceFileName="file1.c";
  loop2.line_number = 12;
  loop2.loop_code = "for (i=0;i<100;i++) \n b[i]=0;";

  loops.push_back(&loop1);
  loops.push_back(&loop2);

  //Generate the abstract handle  for the source file
  fileNode* filenode = new fileNode("file1.c");
  filenode->setMLoops(&loops);
 
  abstract_handle* file_handle = new abstract_handle(filenode);
  cout<<"Created a file handle:"<<endl<<file_handle->toString()<<endl;

  //Create a loop handle within the file
  abstract_node* loop_node1= new loopNode(&loop1);
  abstract_handle* loop_handle1= new abstract_handle(loop_node1);
  cout<<"Created a loop handle:"<<endl<<loop_handle1->toString()<<endl;

  //Create another loop handle within a file using its source position information
  string input1("ForStatement<position,12>");
  abstract_handle* loop_handle2= new abstract_handle(file_handle,input1); 
  cout<<"Created a loop handle:"<<endl<<loop_handle2->toString()<<endl;

  return 0;
}
