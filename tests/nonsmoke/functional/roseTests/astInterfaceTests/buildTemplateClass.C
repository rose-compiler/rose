// Liao, 3/21/2022
// Demonstrate how to build a tempalte class
//
// SageBuilder contains the AST nodes/subtrees builders
// SageInterface contains any other AST utility tools 
//-------------------------------------------------------------------
#include "rose.h"

using namespace SageBuilder;
using namespace SageInterface;

int main (int argc, char *argv[])
{
  // grab the scope in which AST will be added
  SgProject *project = frontend (argc, argv);
  SgGlobal *globalScope = getFirstGlobalScope (project);
  ROSE_ASSERT (globalScope);

  SgSourceFile* sfile= getEnclosingSourceFile (globalScope);
  sfile->set_unparse_template_ast(true);

  SgTemplateParameterPtrList plist;
  SgType * t_par1_type= buildNonrealType(SgName("T"), globalScope);
  SgTemplateParameter *  tpar= buildTemplateParameter (SgTemplateParameter::type_parameter, t_par1_type);
  plist.push_back(tpar);

  SgTemplateArgumentPtrList alist; 

  SgTemplateClassDeclaration* decl = buildTemplateClassDeclaration(SgName("Element"), SgClassDeclaration::e_class, globalScope, NULL, &plist, &alist );
//  setOneSourcePositionForTransformation(decl);

  appendStatement (decl, globalScope);

  // build members inside the template
  SgTemplateClassDefinition *def = isSgTemplateClassDefinition(decl->get_definition());
  SgTemplateVariableDeclaration* t_var_decl = buildTemplateVariableDeclaration(SgName("Value"), t_par1_type, NULL, def );
  appendStatement (t_var_decl, def);

  // extra stuff for testing
  SgVariableDeclaration *varDecl = buildVariableDeclaration(SgName ("i"), buildIntType(), NULL, globalScope);
  // Insert the  member variable
  appendStatement (varDecl,globalScope);


#if 0  

  // build a member function prototype of the construct
  SgInitializedName* arg1 = buildInitializedName(SgName("x"), buildIntType());
  SgFunctionParameterList * paraList = buildFunctionParameterList();
  appendArg(paraList,arg1);

  SgMemberFunctionDeclaration * funcdecl = buildNondefiningMemberFunctionDeclaration("bar",buildVoidType(), paraList);
  appendStatement(funcdecl); 

  // build a defining member function 
  SgFunctionParameterList * paraList2 = isSgFunctionParameterList(deepCopy(paraList)); 
  ROSE_ASSERT(paraList2);
  SgMemberFunctionDeclaration* funcdecl_2 = buildDefiningMemberFunctionDeclaration("bar2",buildVoidType(),paraList2);
  appendStatement(funcdecl_2);                         

  // insert the struct declaration
  appendStatement (decl);

  //Declare a struct variable
  SgVariableDeclaration * varDecl2 = SageBuilder::buildVariableDeclaration("temp", decl2->get_type(), NULL, globalScope);
  appendStatement(varDecl2, globalScope);
#endif

  AstTests::runAllTests(project);
  return backend (project);
}


