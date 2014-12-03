/*
 * Program to test the type equivalence thing on variable types.
 */

#include "rose.h"
#include "RoseAst.h"
//#include "typeEquivalenceChecker.hpp"


class FunctionTypeAccu: public AstSimpleProcessing {

 public:
  FunctionTypeAccu();
  void visit(SgNode *node);

  std::vector < SgFunctionDeclaration * >funcs_;
  bool profile_;
};

FunctionTypeAccu::FunctionTypeAccu() {
}

void
FunctionTypeAccu::visit(SgNode *node) {
  if (isSgFunctionDeclaration(node)) {
    SgFunctionDeclaration *func = isSgFunctionDeclaration(node);

    funcs_.push_back(func);
  }
  return;
}


int
main(int argc, char **argv) {

  SgProject *proj = frontend(argc, argv);

  FunctionTypeAccu t;
  t.traverse(proj, preorder);

  bool checkEqual = false;

  std::vector < SgFunctionDeclaration * >::iterator i, j;
  for (i = t.funcs_.begin(); i != t.funcs_.end(); ++i) {
    for (j = t.funcs_.begin(); j != t.funcs_.end(); ++j) {
      if ((*i)->get_name().getString() != "a"
          || (*j)->get_name().getString() != "b") {
        continue;
      }
      // We use the type of the initialized names here to check
      checkEqual = SageInterface::checkTypesAreEqual((*i)->get_type(), (*j)->get_type());
      if (i == j) {
        continue;
      }
    }
  }
  if (checkEqual) {
    return 0;
  } else {
    return -1;
  }
}
