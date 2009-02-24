#include "rose.h"
#include "MyVisitor.h"

MyVisitor::visit(SgNode* node) {
  cout << node->get_class_name() << endl;
}



