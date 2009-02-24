#include "rose.h"
#include "MyIndenting.h"

MyIndenting::MyIndenting():tabSize(4) {
}
MyIndenting::MyIndenting(unsigned int ts):tabSize(ts) {
}
  
MyIndentLevel
MyIndenting::evaluateInheritedAttribute(SgNode* node, MyIndentLevel inh) {
  if(dynamic_cast<SgBasicBlock*>(node)) {
    inh.level=inh.level+1;
  }
  //printspaces(inh.level*tabSize);
  cout << node->get_class_name() << endl;
  return inh;
}



