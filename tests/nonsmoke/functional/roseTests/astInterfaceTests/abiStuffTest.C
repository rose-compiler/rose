#include "rose.h"
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
  SgProject* proj = frontend(argc, argv);
  // Set up the struct layout chain
  I386PrimitiveTypeLayoutGenerator gen1_i386(NULL);
  NonpackedTypeLayoutGenerator gen_i386(&gen1_i386);

  X86_64PrimitiveTypeLayoutGenerator gen1_x86_64(NULL);
  NonpackedTypeLayoutGenerator gen_x86_64(&gen1_x86_64);
  // Process every type used in a variable or parameter declaration
  vector<SgNode*> initNames = NodeQuery::querySubTree(proj, V_SgInitializedName);
  for (size_t i = 0; i < initNames.size(); ++i) {
    SgInitializedName* in = isSgInitializedName(initNames[i]);
    SgType* t = in->get_type();
    if (isSgTypeEllipse(t)) continue;
    cout << in->get_name().getString() << " has type " << t->unparseToString() << ":\n";
    cout << "On i386:\n";
    cout << gen_i386.layoutType(t) << "\n";
    cout << "On x86-64:\n";
    cout << gen_x86_64.layoutType(t) << "\n";
    if (isSgArrayType(t)) 
      cout<<"Array element count is:"<<SageInterface::getArrayElementCount(isSgArrayType(t))<<endl;
  }
  return 0;
}
