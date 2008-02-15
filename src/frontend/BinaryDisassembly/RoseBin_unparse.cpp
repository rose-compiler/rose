/****************************************************
 * RoseBin :: Binary Analysis for ROSE
 * Author : tps
 * Date : 5Apr07
 * Decription : unparser
 ****************************************************/

#include "rose.h"

using namespace std;

/****************************************************
 * Assign the global node and the filename
 ****************************************************/
void RoseBin_unparse::init(SgAsmNode* root, char* fileName) {
  globalNode = root;
  filename = fileName;
}

/****************************************************
 * unparse the binary AST
 ****************************************************/
void RoseBin_unparse::unparse() {
  //RoseBin_unparse_visitor* visitor = new RoseBin_unparse_visitor();
  //traverseMemoryPoolVisitorPattern(*visitor);

  visitor->init(filename);
  visitor->traverse(globalNode, preorder);
  visitor->close();
}

