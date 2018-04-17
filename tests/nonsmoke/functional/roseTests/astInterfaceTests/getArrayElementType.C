// Test SageInterface::getArrayElementType()
// Liao, 7/14/2016
#include "rose.h"
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
  SgProject* proj = frontend(argc, argv);
  vector<SgNode*> initNames = NodeQuery::querySubTree(proj, V_SgInitializedName);
  for (size_t i = 0; i < initNames.size(); ++i) {
    SgInitializedName* in = isSgInitializedName(initNames[i]);
    SgType* t = in->get_type();
   if (isSgArrayType(t)) 
   {
      SgType* etype = SageInterface::getArrayElementType (t);
      cout<<"Array element type:"<<etype->class_name()<<endl;
   }   
  }
  return 0;
}
