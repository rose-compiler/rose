#include "RuntimeInstrumentation.h"

void RuntimeInstrumentation::run(SgNode* project) {
  traverse(project, preorder);
}

/****************************************************
 * visit each node
 ****************************************************/
void RuntimeInstrumentation::visit(SgNode* n) {
  // check for different types of variable access
  // if access is found, then we assert the variable before the current statement
}
