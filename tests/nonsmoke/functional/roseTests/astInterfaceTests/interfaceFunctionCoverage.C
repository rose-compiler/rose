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

  // cover string getVariantName ( VariantT v )
  getVariantName (node->variantT());


  if (SgDeclarationStatement* decl = isSgDeclarationStatement(node))
  {
    enclosingNamespaceScope( decl);

    //TODO: not working SageInterface::DeclarationSets::getDeclarations ()    
    //      decl_set  = buildDeclarationSets(decl);
    //      decl_set->getDeclarations(decl);

  }

  if (SgSwitchStatement* sw = isSgSwitchStatement(node))
  {
    whereAmI (sw); // we don't want touch this func for all nodes
  }

  if (isSgLocatedNode(node))
  {
  }

  if (SgTemplateInstantiationMemberFunctionDecl * temp_decl = isSgTemplateInstantiationMemberFunctionDecl (node))
  {
    cout<<"Found a SgTemplateInstantiationMemberFunctionDecl "<<endl;
    //TODO: this function triggers assertion failure.
    // getNonInstantiatonDeclarationForClass (temp_decl);
  }

  if (SgFunctionDeclaration* func = isSgFunctionDeclaration (node))
  {
    SgScopeStatement* scope = func->get_scope();
    if (scope->containsOnlyDeclarations() == true)
       isPrototypeInScope (scope, func, func);
  }

}

int
main ( int argc, char* argv[] )
{
  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT (project != NULL);


  // ROSE visit traversal
  RoseVisitor visitor;
  visitor.traverseMemoryPool();

  // 

  return backend(project);
}

