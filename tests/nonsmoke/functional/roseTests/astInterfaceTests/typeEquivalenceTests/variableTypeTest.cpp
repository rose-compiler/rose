/*
 * Program to test the type equivalence thing on variable types.
 */

#include "rose.h"
#include "RoseAst.h"
//#include "typeEquivalenceChecker.hpp"


class TypeTraversal: public AstSimpleProcessing {

 public:
  TypeTraversal();
  void visit(SgNode *node);

  SgInitializedName *n1;
  SgInitializedName *n2;
  std::vector < SgInitializedName * >names_;
  bool profile_;
};

TypeTraversal::TypeTraversal(): n1(NULL), n2(NULL) {
}

void
TypeTraversal::visit(SgNode *node) {
  if (isSgInitializedName(node)) {
    SgInitializedName *name = isSgInitializedName(node);

    names_.push_back(name);
  }
  return;
}


int
main(int argc, char **argv) {

  SgProject *proj = frontend(argc, argv);

  TypeTraversal t;
  t.traverse(proj, preorder);

  bool checkEqual = false;
#if 0
if(argc < 4){
    return -2;
  }

  int expected = atoi(argv[2]);
#endif
  std::vector < SgInitializedName * >::iterator i, j;
  for (i = t.names_.begin(); i != t.names_.end(); ++i) {
    for (j = t.names_.begin(); j != t.names_.end(); ++j) {
      if ((*i)->get_name().getString() != "a"
          || (*j)->get_name().getString() != "b") {
        continue;
      }
     
      // We use the type of the initialized names here to check
      checkEqual = SageInterface::checkTypesAreEqual((*i)->get_type(), (*j)->get_type());
      if (i == j) {
        if (!checkEqual){
          return 1;
        }
//        continue;
      }
    }
  }
//  int res = checkEqual?1:0;
//  if (res == expected) {
  if(checkEqual){
    return 0;
  } else {
    return -1;
  }
}
