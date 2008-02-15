#include "rose.h"
#include "MyVisitor.h"

int main (int argc, char* argv[]) {
  SgProject* astNode=frontend(argc,argv);
  MyVisitor v;
  v.traverseInputFiles(astNode,preorder);
}



