#include "rose.h"

int main(argc, argv) {
  SgProject* astNode=frontend(argc,argv);
  MyVisitor v;
  v.traverseInputFiles(0);
}



