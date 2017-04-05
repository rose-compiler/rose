// A translator to call many SageInterface functions to improve code coverage reported by LCOV.
//
// Initially add analysis interface functions, 
//
// Later to add transformation interface functions
//  SageInterface::initializeSwitchStatement(SgSwitchStatement* switchStatement,SgStatement *item_selector,SgStatement *body)
//
//
// by traversing memory pools
//
// Liao, 4/4/2017
#include "rose.h"
#include <iostream>
using namespace std;
using namespace SageInterface;

// used to cover SageInterface::DeclarationSets:: * 
SageInterface::DeclarationSets* decl_set = NULL; 

class RoseVisitor : public ROSE_VisitTraversal
{
  public:
    void visit ( SgNode* node);
};


void RoseVisitor::visit ( SgNode* node)
{
  if (SgDeclarationStatement* decl = isSgDeclarationStatement(node))
  {
    cout<<"calling enclosingNamespaceScope() "<<endl;
    enclosingNamespaceScope( decl);

    //TODO: not working SageInterface::DeclarationSets::getDeclarations ()    
    //      decl_set  = buildDeclarationSets(decl);
    //      decl_set->getDeclarations(decl);

    
    cout<<"calling SageInterface::generateUniqueNameForUseAsIdentifier_support ( SgDeclarationStatement* declaration ) "<<endl;
    cout<<generateUniqueNameForUseAsIdentifier_support (decl)<<endl;

    // TODO: this fails for some nodes, moved to function declarations 
   // cout<<"calling SageInterface::generateUniqueNameForUseAsIdentifier( SgDeclarationStatement* declaration ) "<<endl;
   // cout<<generateUniqueNameForUseAsIdentifier(decl)<<endl;
  }

  if (SgSwitchStatement* sw = isSgSwitchStatement(node))
  {
    cout<<"calling whereAmI() "<<endl;
    whereAmI (sw); // we don't want touch this func for all nodes

    cout<<"calling SageInterface::outputLocalSymbolTables ( SgNode* node ) "<<endl;
    outputLocalSymbolTables (sw);
  }

   // TODO: not sure when SgToken show up in AST
  if (SgToken* stk = isSgToken(node) )
  {
    cout<<"calling SageInterface::get_name ( const SgToken* token ) "<<endl;
    cout<<get_name (stk)<<endl;
  }

  if (isSgLocatedNode(node))
  {
    // cover string getVariantName ( VariantT v )
    // the test harness in Makefile.am will redirect output to rose_inputinterfaceFunctionCoverage.C.passed
    cout<<"calling getVariantName() "<<endl;
    cout<<getVariantName (node->variantT())<<endl;
  }


  if (SgTemplateInstantiationMemberFunctionDecl * temp_decl = isSgTemplateInstantiationMemberFunctionDecl (node))
  {
    cout<<"calling functions operating on SgTemplateInstantiationMemberFunctionDecl()"<<endl;
    //TODO: this function triggers assertion failure.
    // getNonInstantiatonDeclarationForClass (temp_decl);

    templateDefinitionIsInClass (temp_decl);
  }

  if (SgFunctionDeclaration* func = isSgFunctionDeclaration (node))
  {
    SgScopeStatement* scope = func->get_scope();
    if (scope->containsOnlyDeclarations() == true)
    {
      cout<<"calling isPrototypeInScope() "<<endl;
      isPrototypeInScope (scope, func, func);
    }

    cout<<"calling SageInterface::generateUniqueNameForUseAsIdentifier( SgDeclarationStatement* declaration ) "<<endl;
    cout<<generateUniqueNameForUseAsIdentifier(func)<<endl;
  }

  //TODO: this never should work since we don't use SgC_PreprocessorDirectiveStatement now. 
  if (SgC_PreprocessorDirectiveStatement * pdecl= isSgC_PreprocessorDirectiveStatement(node))
  {
    cout<<"calling get_name(SgC_PreprocessorDirectiveStatement*) "<<endl;
    cout<< get_name(pdecl)<<endl;    
  }

  if (SgExpression* exp = isSgExpression(node))
  {
    cout<<"calling SageInterface::get_name ( const SgType* type ) "<<endl;
    cout<<get_name(exp->get_type()); // we don't want touch this func for all nodes
  }

}

int
main ( int argc, char* argv[] )
{
  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT (project != NULL);

  cout<<"begin test output .."<<endl;
  //1. Call some functions in the beginning 
  // the order matters.
  // calling SageInterface::computeUniqueNameForUseAsIdentifier( SgNode* astNode )
  computeUniqueNameForUseAsIdentifier (project);


  //2. Call some functions during a memory traversal
  // ROSE memory traversal to catch all sorts of nodes, not just those on visible AST
  RoseVisitor visitor;
  visitor.traverseMemoryPool();

  //3. Call some functions in the end
  reset_name_collision_map();
  outputGlobalFunctionTypeSymbolTable();

  return backend(project);
}

