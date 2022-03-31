// Liao, 3/21/2022
// Demonstrate how to build a tempalte class
//
// SageBuilder contains the AST nodes/subtrees builders
// SageInterface contains any other AST utility tools 
//-------------------------------------------------------------------
#include "rose.h"

using namespace SageBuilder;
using namespace SageInterface;

SgProject * tester_init(int argc, char *argv[], SgGlobal * & global_scope) {
  SgProject * project = frontend (argc, argv);

  global_scope = SageInterface::getFirstGlobalScope (project);
  ROSE_ASSERT(global_scope);

  SgSourceFile* sfile = SageInterface::getEnclosingSourceFile(global_scope);
  sfile->set_unparse_template_ast(true);

  return project;
}

SgTemplateClassDeclaration * build_template_class(
  SgScopeStatement * scope,
  SgType * & t_par1_type,
  SgTemplateClassDefinition * & t_class_defn
) {
  // 1 - create the nonreal scope of the template declaration

  SgDeclarationScope * nrscope = SageBuilder::buildDeclarationScope();
  
  // 2 - the scope of the template parameters (type, non-type, and template) is that nonreal scope

  t_par1_type = SageBuilder::buildNonrealType(SgName("T"), nrscope);
  SgTemplateParameterPtrList tpl_param_list{
    SageBuilder::buildTemplateParameter(SgTemplateParameter::type_parameter, t_par1_type)
  };

  // 3 - build the template declaration
  SgTemplateArgumentPtrList alist; 
  SgTemplateClassDeclaration * t_class_decl = SageBuilder::buildTemplateClassDeclaration(
      SgName("Element"), SgClassDeclaration::e_class,
      scope, nullptr, &tpl_param_list, &alist
  );

  // 4 - Attach template 

  t_class_decl->set_nonreal_decl_scope(nrscope);
  nrscope->set_parent(t_class_decl);

  SageInterface::appendStatement(t_class_decl, scope);
  
  t_class_defn = isSgTemplateClassDefinition(t_class_decl->get_definition());

  return t_class_decl;
}

#if defined(CODE_TO_GENERATE)
template <class T>
class Element {
  private:
    static T Value;
    static bool Valid;
  public:
    static void Set(T E) {
      Value = E;
      Valid = true;
    }
    static void Reset();
};

template <class T>
bool Element<T>::Valid = false;

template <class T>
T Element<T>::Value;

template <class T>
void Element<T>::Reset() {
  Valid = false;
}
#endif

int main (int argc, char *argv[]) {
  SgGlobal * global_scope;
  SgProject * project = tester_init (argc, argv, global_scope);

  SgScopeStatement * scope = global_scope;

  // Notes:
  //   - I am not using SageInterface::appendStatement because it reset the scope to be the parent that is a problem for out-of-class definitions
  //     Instead: stmt->set_parent(parent); parent->append_statement(stmt);

  // 1 - Build the (defining) template class declaration

  SgType * t_par1_type = nullptr;
  SgTemplateClassDefinition * t_class_defn = nullptr;
  SgTemplateClassDeclaration * t_class_decl = build_template_class(scope, t_par1_type, t_class_defn);

  // 2 - Build member variables with initializers outside the class

  SgTemplateVariableDeclaration * var0_decl = SageBuilder::buildTemplateVariableDeclaration(
      SgName("Value"), t_par1_type, nullptr, t_class_defn
  );
  var0_decl->get_declarationModifier().get_storageModifier().setStatic();
  var0_decl->set_parent(t_class_defn);
  t_class_defn->append_statement(var0_decl);

  SgTemplateVariableDeclaration * var1_decl = SageBuilder::buildTemplateVariableDeclaration(
      SgName("Value"), t_par1_type, nullptr, t_class_defn
  );
  scope->append_statement(var1_decl);
  var1_decl->set_parent(scope);
  var1_decl->get_variables()[0]->set_scope(t_class_defn);

  SgTemplateVariableDeclaration * var2_decl = SageBuilder::buildTemplateVariableDeclaration(
      SgName("Valid"), SageBuilder::buildBoolType(), nullptr, t_class_defn
  );
  var2_decl->get_declarationModifier().get_storageModifier().setStatic();
  var2_decl->set_parent(t_class_defn);
  t_class_defn->append_statement(var2_decl);

  SgTemplateVariableDeclaration * var3_decl = SageBuilder::buildTemplateVariableDeclaration(
      SgName("Valid"), SageBuilder::buildBoolType(),
      SageBuilder::buildAssignInitializer(SageBuilder::buildBoolValExp(false)),
      t_class_defn
  );
  var3_decl->set_parent(scope);
  scope->append_statement(var3_decl);

  // 3 - Build a member functions defined inside the class (need to add a hidden 1st non-defining)

  SgTemplateParameterPtrList mfnc0_tpl_params;
  SgTemplateMemberFunctionDeclaration * mfnc0_decl = SageBuilder::buildNondefiningTemplateMemberFunctionDeclaration(
      "Set", buildVoidType(),
      SageBuilder::buildFunctionParameterList( SageBuilder::buildInitializedName(SgName("E"), t_par1_type) ),
      t_class_defn, nullptr, 0, &mfnc0_tpl_params
  );

  mfnc0_decl->get_declarationModifier().get_storageModifier().setStatic();
  mfnc0_decl->set_parent(t_class_defn);
//  t_class_defn->append_statement(mfnc0_decl);
  mfnc0_decl->setCompilerGenerated(); // FIXME that is not sufficent so I also commented out the call to append_statement() above

  SgTemplateMemberFunctionDeclaration * mfnc1_decl = SageBuilder::buildDefiningTemplateMemberFunctionDeclaration(
      "Set", buildVoidType(),
      SageBuilder::buildFunctionParameterList( SageBuilder::buildInitializedName(SgName("E"), t_par1_type) ),
      t_class_defn, nullptr, 0, mfnc0_decl
  );

  mfnc1_decl->get_declarationModifier().get_storageModifier().setStatic();
  mfnc1_decl->set_parent(t_class_defn);
  t_class_defn->append_statement(mfnc1_decl);

  // TODO mfnc1_decl->get_definition() ...

  // 4 - Build a member functions defined outside the class

  SgTemplateParameterPtrList mfnc2_tpl_params;
  SgTemplateMemberFunctionDeclaration * mfnc2_decl = SageBuilder::buildNondefiningTemplateMemberFunctionDeclaration(
      "Reset", buildVoidType(), SageBuilder::buildFunctionParameterList(), t_class_defn, nullptr, 0, &mfnc2_tpl_params
  );
  mfnc2_decl->get_declarationModifier().get_storageModifier().setStatic();
  mfnc2_decl->set_parent(t_class_defn);
  t_class_defn->append_statement(mfnc2_decl);
  
  SgTemplateMemberFunctionDeclaration * mfnc3_decl = SageBuilder::buildDefiningTemplateMemberFunctionDeclaration(
      "Reset", buildVoidType(), SageBuilder::buildFunctionParameterList(), t_class_defn,
      nullptr, 0, mfnc2_decl
  );
  mfnc3_decl->set_parent(scope);
  scope->append_statement(mfnc3_decl);

  // TODO mfnc3_decl->get_definition() ...

  AstTests::runAllTests(project);

  return backend (project);
}


