// A translator to call many SageInterface functions to improve code coverage reported by LCOV.
//
// Initially add analysis interface functions, 
//
// Later to add transformation interface functions
//  SageInterface::initializeSwitchStatement(SgSwitchStatement* switchStatement,SgStatement *item_selector,SgStatement *body)
//  SageInterface::appendStatement(SgStatement *stmt, SgForInitStatement* for_init_stmt)
//   insertStatementListBeforeFirstNonDeclaration()
//   moveToSubdirectory()
//   attachComment()
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

    // local function, commented out
    //generateUniqueDeclaration (decl);

  }

  if (SgSwitchStatement* sw = isSgSwitchStatement(node))
  {
    cout<<"calling whereAmI() "<<endl;
    whereAmI (sw); // we don't want touch this func for all nodes

    cout<<"calling SageInterface::outputLocalSymbolTables ( SgNode* node ) "<<endl;
    outputLocalSymbolTables (sw);

    // calling some functions within a smaller narrow scope
    setOneSourcePositionForTransformation(sw);
    removeAllOriginalExpressionTrees (sw);
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

    // TODO: this function causes assertion failure
    // SgTemplateInstantiationMemberFunctionDecl* copy = buildForwardFunctionDeclaration (temp_decl);
    // prependStatement(copy, temp_decl->get_scope());
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

    SgFunctionDeclaration* nondef_decl = isSgFunctionDeclaration(func->get_firstNondefiningDeclaration ());
    if (nondef_decl != NULL && nondef_decl != func)
    {
      if (declarationPreceedsDefinition (nondef_decl, func))
        cout<<"calling declarationPreceedsDefinition() returns true."<<endl;
    }
  }

  if (SgMemberFunctionDeclaration * memfunc = isSgMemberFunctionDeclaration (node))
  {
    // this function only accepts member functions
    if (isOverloaded (memfunc))
      cout<<"calling isOverloaded() returns true"<<endl;
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

    getDeclarationOfNamedFunction(exp); 

    //TODO: assert failure for this function
    //if (getInitializerOfExpression (exp)!= NULL)
    //   cout<<"calling SageInterface::getInitializerOfExpression(SgExpression* n) returns something "<<endl;

    if (SgIntVal* iv = isSgIntVal(exp))
    {
      if(isEqualToIntConst (iv, 12345))
      {
        cout<<"calling  SageInterface::isEqualToIntConst(SgExpression* e, int value) return true, IntVal == 12345 "<<endl;
        setOperand (isSgExpression(iv->get_parent()), SageBuilder::buildIntVal(9));
      }
    }

    isConstantTrue (exp);
    isConstantFalse (exp);

#if 0
    if (SgBinaryOp* bop = isSgBinaryOp (exp))
    {
      //TODO: assertion failure
      splitExpressionIntoBasicBlock(bop);
    }
#endif

  }


  if (SgFunctionCallExp* node2= isSgFunctionCallExp(node))
  {
    // TODO: this function has many assertions for unsupported cases. 
    // functionCallExpressionPreceedsDeclarationWhichAssociatesScope(node2);
    isCallToParticularFunction ("test_splitVariableDeclaration",0, node2);

    if (SgFunctionSymbol* fsym = node2->getAssociatedFunctionSymbol ())
    {
      if ((fsym->get_name().getString()) =="test_splitVariableDeclaration" ) 
      {
        SgFunctionDeclaration* fdecl = findFunctionDeclaration (getGlobalScope(node2),"test_splitVariableDeclaration", NULL, true);
        isCallToParticularFunction (fdecl, node2);
      }
    }
  }

  // Extracted from projects/SMTPathFeasibility/utils/replaceExpressionsAndSimplifyExpressions.cpp
  if (SgIfStmt* fixIf= isSgIfStmt(node))
  {
    SgStatement* conditional = fixIf->get_conditional();
    if (isSgExprStatement(conditional)) {
      SgExpression* expr = isSgExprStatement(conditional)->get_expression();
      std::pair<SgVariableDeclaration*, SgExpression*> pr = SageInterface::createTempVariableForExpression(expr,isSgScopeStatement(fixIf),true);
      SgInitializedNamePtrList lptr = pr.first->get_variables();
      //std::cout << "lprt size: " << lptr.size() << std::endl;
      ROSE_ASSERT(lptr.size() <= 1);
      SgVarRefExp* varRef = SageBuilder::buildVarRefExp(pr.first);
      SgIntVal* iv = SageBuilder::buildIntVal(0);
      SgNotEqualOp* nop = SageBuilder::buildNotEqualOp(isSgExpression(varRef),isSgExpression(iv));
      SgExprStatement* ses = SageBuilder::buildExprStatement(isSgExpression(nop));
      SageInterface::replaceStatement(conditional,ses);

      SageInterface::insertStatementBefore(fixIf,pr.first); 
    }
  }

  // scan various input functions to trigger testing
  if (SgFunctionDefinition* node2= isSgFunctionDefinition(node))
  {
    clearScopeNumbers (node2);

    removeConsecutiveLabels (node2);

    removeLabeledGotos (node2);

    splitVariableDeclaration (node2);

    Rose_STL_Container<SgNode*>  currentVarRefList; 
    collectVariableReferencesInArrayTypes (node2, currentVarRefList);
  }

  if (SgScopeStatement* node2= isSgScopeStatement(node))
  {
    //TODO: this function as error 
    // calling SageInterface::getEnclosingScope(SgNode* n, const bool includingSelf/* =false*/)
    //getEnclosingScope (node2);

    hasSimpleChildrenList(node2);
  }

}

int
main ( int argc, char* argv[])
{
  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT (project != NULL);

  cout<<"begin test output .."<<endl;
  //1. Call some functions in the beginning 
  // the order may matter.
  // calling SageInterface::computeUniqueNameForUseAsIdentifier( SgNode* astNode )
  computeUniqueNameForUseAsIdentifier (project);

  is_OpenMP_language();
  is_CAF_language();
  is_UPC_dynamic_threads();
  is_X10_language();
  is_mixed_Fortran_and_C_language();

  //2. Call some functions during a memory traversal
  // ROSE memory traversal to catch all sorts of nodes, not just those on visible AST
  RoseVisitor visitor;
  visitor.traverseMemoryPool();

  //3. Call some functions in the end
  reset_name_collision_map();
  outputGlobalFunctionTypeSymbolTable();

  
  return backend(project);
}

