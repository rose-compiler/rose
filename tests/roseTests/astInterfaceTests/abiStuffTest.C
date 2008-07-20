#include "rose.h"
#include <iostream>

using namespace std;

int main(int argc, char** argv) {
  SgProject* proj = frontend(argc, argv);
  // Set up the struct layout chain
  I386PrimitiveTypeLayoutGenerator gen1(NULL);
  NonpackedTypeLayoutGenerator gen(&gen1);
  // Process every type used in a variable or parameter declaration
  vector<SgNode*> initNames = NodeQuery::querySubTree(proj, V_SgInitializedName);
  for (size_t i = 0; i < initNames.size(); ++i) {
    SgInitializedName* in = isSgInitializedName(initNames[i]);
    SgType* t = in->get_type();
    if (isSgTypeEllipse(t)) continue;
    cout << in->get_name().getString() << " has type " << t->unparseToString() << ":\n";
    cout << gen.layoutType(t) << "\n";
  }
  return 0;
}
