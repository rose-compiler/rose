// Input program from Tim (part of a questions as to how to detect volatile types).
// The problem is that volatile can refere to different sorts of declarations.
// Since the design of the modifiers follows the C++ grammar it has some consiquences 
// that a few thigs can be stored in eithr of a few places (usually not more than two).
// But when there is more than one location to store, we have to select one, and this
// can be less than intuative.

#include<rose.h>

#include<vector>

int main(int argc, char* argv[])
{
  SgProject* sagaProject = frontend(argc, argv);

  std::vector<SgNode*> var_refs = NodeQuery::querySubTree(sagaProject, V_SgVarRefExp);
  SgVarRefExp *var_ref = isSgVarRefExp(var_refs.at(0));
  SgDeclarationStatement *decl = var_ref->get_symbol()->get_declaration()->get_declaration();

  SgDeclarationModifier &modifier = decl->get_declarationModifier();

  if(modifier.get_typeModifier().get_constVolatileModifier().isVolatile()) {
    std::cout << "Volatile" << std::endl;
  } else {
    std::cout << "Not Volatile" << std::endl;
  }

  return 0;
}
