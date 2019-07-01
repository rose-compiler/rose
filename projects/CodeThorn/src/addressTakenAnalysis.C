/*****************************************
 * Author: Sriram Aananthakrishnan, 2013 *
 *****************************************/

#include "sage3basic.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include "addressTakenAnalysis.h"
#include "CodeThornException.h"

using namespace CodeThorn;
using namespace CodeThorn;

/*************************************************
 ***************** ProcessQuery  *****************
 *************************************************/

MatchResult& CodeThorn::ProcessQuery::getMatchResult()
{
  return match_result;
}

void CodeThorn::ProcessQuery::printMatchResult()
{
  // MatchResult is list of maps
  // each map corresponds to one particular match instance
  for(MatchResult::iterator it = match_result.begin(); it != match_result.end(); it++)
  {
    for(SingleMatchVarBindings::iterator smbIt = (*it).begin(); smbIt != (*it).end(); smbIt++)
    {
      std::cout << "MATCH=";
      SgNode* matchedTerm = (*smbIt).second;
      ROSE_ASSERT(matchedTerm != NULL);
      std::cout << "  VAR: " << (*smbIt).first << "=" 
                << AstTerm::astTermWithNullValuesToString(matchedTerm) << " @" << matchedTerm << std::endl;
    }
  }
}

void CodeThorn::ProcessQuery::clearMatchResult()
{
  match_result.clear();
}

MatchResult& CodeThorn::ProcessQuery::operator()(std::string query, SgNode* root)
{
  AstMatching m;
  match_result = m.performMatching(query, root);
  return match_result;
}

/*************************************************
 ************* ComputeAddressTakenInfo  **********
 *************************************************/

CodeThorn::ComputeAddressTakenInfo::VariableAddressTakenInfo CodeThorn::ComputeAddressTakenInfo::getVariableAddressTakenInfo()
{
  return variableAddressTakenInfo;
}

CodeThorn::ComputeAddressTakenInfo::FunctionAddressTakenInfo CodeThorn::ComputeAddressTakenInfo::getFunctionAddressTakenInfo()
{
  return functionAddressTakenInfo;
}

bool CodeThorn::ComputeAddressTakenInfo::getAddAddressTakingsInsideTemplateDecls() {
  return addAddressTakingsInsideTemplateDecls;
}

void CodeThorn::ComputeAddressTakenInfo::setAddAddressTakingsInsideTemplateDecls(bool addAddressTakingsInsideTemplateDecls){
  this->addAddressTakingsInsideTemplateDecls = addAddressTakingsInsideTemplateDecls;
}

void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::debugPrint(SgNode* sgn)
{
  std::cerr << sgn->class_name() << ": " 
            << AstTerm::astTermWithNullValuesToString(sgn) << ", " \
            << sgn->unparseToString() << ", "                           \
            << sgn->get_file_info()->get_filenameString() << ", "       \
            << sgn->get_file_info()->get_line() << ", " \
            << std::endl;
}

// base case for the recursion
void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgVarRefExp *sgn)
{ 
  if(debuglevel > 0) debugPrint(sgn);

  VariableId id = cati.vidm.variableId(sgn);
  ROSE_ASSERT(id.isValid());
  // schroder3 (Jun 2016): Check for bitfield:
  if(SgVariableDeclaration* varDecl = cati.vidm.getVariableDeclaration(id)) {
    if(varDecl->get_bitfield ()) {
      // This variable is a bitfield. An address can not be taken from a bitfield.
      //  ==> This should be unreachable code (nothing to do):
      ROSE_ASSERT(false);
      return;
    }
  }
  // insert the id into VariableIdSet
  insertVariableId(id);
}

void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgVariableDeclaration* sgn) {
  if(debuglevel > 0) debugPrint(sgn);

  SgInitializedName* varDeclInitName = SgNodeHelper::getInitializedNameOfVariableDeclaration(sgn);
  ROSE_ASSERT(varDeclInitName);
  SgType* varDeclType = varDeclInitName->get_type();
  ROSE_ASSERT(varDeclType);

  if(debuglevel > 0) {
    std::cout << "init name: " << varDeclInitName->unparseToString() << std::endl;
    std::cout << "type: " << varDeclType->unparseToString() << std::endl;
    std::cout << "class name: " << varDeclType->class_name() << std::endl;
  }

  SgExpression* varDeclInitializer = SgNodeHelper::getInitializerExpressionOfVariableDeclaration(sgn);

  // schroder3 (2016-08-01): Check whether this is a declaration inside a catch (e.g. "try { } catch(int& i) { }"):
  if(isSgCatchOptionStmt(sgn->get_parent())) {
    // A "catch variable" has no initializer:
    ROSE_ASSERT(!varDeclInitializer);
    // Check whether this is a "declaration" of an ellipsis in a catch ("try { } catch(... /* <- */) { }"):
    if(isSgTypeEllipse(varDeclType)) {
      ROSE_ASSERT(varDeclInitName->unparseToString() == "");
      // Because this is not a real variable declaration (no name, no real type, no symbol) there is nothing to do.
    }
    // Check whether this is a declaration of an reference:
    else if(SgNodeHelper::isReferenceType(varDeclType)) {
      // A catch statement might catch a rethrown exception (If this is a catch of a rethrown exception then the variable in
      //  this catch statement is an alias of the rethrown exception and therefore likely an alias of another "catch variable"):
      //  Add the variable to the address-taken-set though there is no initializer expression:
      insertVariableId(cati.vidm.variableId(sgn));
    }
    else {
      // no initializer and no declaration of reference type: nothing to do
    }
  }
  else {
    // "normal" variable declaration:

    // schroder3 (2016-07-21):
    //  The declared variable is the target of the initialization:
    std::vector<VariableId> possibleTargets;
    possibleTargets.push_back(cati.vidm.variableId(sgn));

    // schroder3 (2016-07-21):
    //  Add the variables/ functions in the init expression. Two cases:
    //
    //  Initialization of a reference:
    //   Currently we treat every variable in the init expression of the reference as if its address was taken, because
    //   we currently have no mapping between the reference and the variables in the init expression. It is therefore
    //   currently not possible to add the variable to the address taken set only if the address of the reference is taken.
    //
    //  Initialization of a variable with underlying function pointer type:
    //   There might be an implicit address-taking of a function if the initializer contains a function:
    //   Traverse the initializer and only add functions to the address-taken set (and do not add variables of function type):
    //
    //  Handle both cases:
    handleAssociation(AK_Initialization, possibleTargets, varDeclType, varDeclInitializer);
  }
}

// only the rhs_op of SgDotExp is modified
// recurse on rhs_op
void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgDotExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  SgNode* rhs_op = sgn->get_rhs_operand();
  rhs_op->accept(*this);
}

// only the rhs_op of SgArrowExp is modified
// recurse on rhs_op
void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgArrowExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  SgNode* rhs_op = sgn->get_rhs_operand();
  rhs_op->accept(*this);
}

// schroder3 (2016-08-08): "A.*B" and "A->*Bb": The result is the member (function)
//  specified by the pointer B. This operator dereferences B (and A if it is a pointer)
//  to determine its result. If this is the operand of an address-of operator then there
//  is currently no way to determine the variable or function from which the address is
//  taken and that variable or function should already be in the address-taken-set anyway.
//  Furthermore, there is no additional address-taking inside this operator. Thus there
//  is nothing to do (calls are handled separately).
void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgDotStarOp* sgn) {
  if(debuglevel > 0) debugPrint(sgn);
}
void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgArrowStarOp* sgn) {
  if(debuglevel > 0) debugPrint(sgn);
}

// schroder3 (2016-07-26): There might be an implicit address-of operator. Apart
//  from that, the old comment is still correct:
//
// For example q = &(*p) where both q and p are pointer types
// In the example, q can potentially modify all variables pointed to by p
// same as writing q = p.
// Since we dont know anything about p, q can potentially modify all 
// the elements in addressTakenSet as result of the above expression 
// As a result, the variables whose addresses can be taken is the entire 
// set as we dont have any idea about p
// We dont need to add any new variable to addressTakenSet 
// as a consequence of the expressions similar to above.
void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgPointerDerefExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);

  // schroder3 (2016-07-26): Check for an implicit address-of operator:
  if(sgn->get_operand()->get_type()->isEquivalentType(sgn->get_type())) {
    // The type of the operand to the dereference operator and the type of the
    //  result of the dereference operator are equal. This implies that there
    //  is a implicit address-of operator that takes the address of the operand
    //  before this dereference operator dereferences the address afterwards.
    //
    // Currently this should only be possible in case of a function type:
    ROSE_ASSERT(isSgFunctionType(sgn->get_operand()->get_type()));

    // Find all variables/ functions in the operand expression because their address is
    //  implicitly taken.
    SgNode* dereferencee = sgn->get_operand();
    dereferencee->accept(*this);
  }

  // we raise a flag
  cati.variableAddressTakenInfo.first = true;
}

// For example &(A[B[C[..]]]) or &(A[x][x][x])
// any pointer can that takes this address can modify
// contents of only A. The inner index expressions are r-values
// it is sufficient to add A to addressTakenSet
// keep recursing on the lhs until we find A
void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgPntrArrRefExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  // collect the info only if its array type
  // otherwise some pointer arithmetic is going on to compute the address of the array
  SgNode* arr_op = sgn->get_lhs_operand();
  if(isSgArrayType(arr_op)) {    
    arr_op->accept(*this);
    }
  else {
    // raise the flag as we dont know whose address is taken
    cati.variableAddressTakenInfo.first = true;
  }
}

// &(a = expr)
// a's address is taken here
// process the lhs recursively
void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgAssignOp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);

  SgNode* lhs_op = sgn->get_lhs_operand();
  lhs_op->accept(*this);
}

// &(++i)
// prefix increments first and the result can be used as lvalue (in C++)
// postfix uses the operand as lvalue and increments later and therefore
// postfix increment cannot be lvalue
// both prefix/postfix are illegal in C
void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgPlusPlusOp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  SgNode* operand = sgn->get_operand();
  operand->accept(*this);
}

// same as prefix increment
// &(--i)
void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgMinusMinusOp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  SgNode* operand = sgn->get_operand();
  operand->accept(*this);
}

// &(a+1, b)
// b's address is taken
// keep recursing on the rhs_op
void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgCommaOpExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  SgNode* rhs_op = sgn->get_rhs_operand();
  rhs_op->accept(*this);
}

// if we see SgConditionalExp as operand of &
// both true and false branch are lvalues
// recurse on both of them to pick up the lvalues
void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgConditionalExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  SgNode* true_exp = sgn->get_true_exp();
  SgNode* false_exp = sgn->get_false_exp();
  true_exp->accept(*this);
  false_exp->accept(*this);
}

// cast can be lvalue
// example :  &((struct _Rep *)((this) -> _M_data()))[-1] expr from stl_list
void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgCastExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);

  SgNode* operand = sgn->get_operand();
  operand->accept(*this);
}

void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::insertAllFunctionIds() {
  FunctionIdSet allFuncIds = cati.fidm.getFunctionIdSet();
  for(FunctionIdSet::const_iterator i = allFuncIds.begin(); i != allFuncIds.end(); ++i) {
    insertFunctionId(*i);
  }
}

void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::insertAllVariableIds() {
  VariableIdSet allVarIds = cati.vidm.getVariableIdSet();
  for(VariableIdSet::const_iterator i = allVarIds.begin(); i != allVarIds.end(); ++i) {
    insertVariableId(*i);
  }
}

// schroder3 (2015-08-25): Insert the given variable into the address-taken set and add sibling
//  members if the given variable is a member of an union.
void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::insertVariableId(VariableId id) {
  class VariableIdInserter {
    ComputeAddressTakenInfo& cati;
    int debuglevel;
   public:
    VariableIdInserter(ComputeAddressTakenInfo& info, int debugLevel)
         : cati(info), debuglevel(debugLevel) { }
    void operator ()(VariableId id) {
      ROSE_ASSERT(id.isValid());

      cati.variableAddressTakenInfo.second.insert(id);

      if(debuglevel > 0) {
        std::cout << "INFO: Added variable id " << id.getIdCode() << " (Name "
                  << cati.vidm.variableName(id) << ")"
                  << std::endl;
      }
    }
  } insertVariableIdInternal(cati, debuglevel);

  // Insert the id
  insertVariableIdInternal(id);

  // Check whether this variable is a member of an union.
  if(SgVariableDeclaration* varDecl = cati.vidm.getVariableDeclaration(id)) {
    // Check whether this variable is a member by looking at the closest enclosing scope:
    SgNode* parent = varDecl;
    SgClassDefinition* varRecordDef = 0;
    while((parent = parent->get_parent())) {
      if(isSgScopeStatement(parent)) {
        varRecordDef = isSgClassDefinition(parent);
        break;
      }
    }
    if(varRecordDef) {
      // Variable is a member. Check whether the corresponding record is a union:
      SgClassDeclaration* varRecordDecl = varRecordDef->get_declaration();
      ROSE_ASSERT(varRecordDecl);
      if(varRecordDecl->get_class_type() == SgClassDeclaration::e_union) {
        // Variable is a member of an union: Add all other members of the union to the address-taken set:
        const SgDeclarationStatementPtrList& unionMembers = varRecordDef->get_members();
        for(SgDeclarationStatementPtrList::const_iterator i = unionMembers.begin(); i != unionMembers.end(); ++i) {
          // Only add members (and not member functions):
          if(SgVariableDeclaration* unionMember = isSgVariableDeclaration(*i)) {
            VariableId unionMemberVarId = cati.vidm.variableId(unionMember);
            // Do not use insertVariableId(...) to avoid that the same union member is checked multiple times.
            insertVariableIdInternal(unionMemberVarId);
            // Check for nested unions:
            if(SgClassType* memberRecordType = isSgClassType(cati.vidm.getType(unionMemberVarId))) {
              SgClassDeclaration* memberRecordDecl = isSgClassDeclaration(memberRecordType->get_declaration());
              ROSE_ASSERT(memberRecordDecl);
              if(memberRecordDecl->get_class_type() == SgClassDeclaration::e_union) {
                // Nested union found. Recursively call insertVariableId(...) for the first member of the nested union:
                if((memberRecordDecl = isSgClassDeclaration(memberRecordDecl->get_definingDeclaration()))){
                  SgClassDefinition* unionRecordDef = memberRecordDecl->get_definition();
                  ROSE_ASSERT(unionRecordDef);
                  const SgDeclarationStatementPtrList& memberUnionMembers = unionRecordDef->get_members();
                  for(SgDeclarationStatementPtrList::const_iterator j = memberUnionMembers.begin(); j != memberUnionMembers.end(); ++j) {
                    // Only add members (and not member functions):
                    if(SgVariableDeclaration* memberUnionMember = isSgVariableDeclaration(*j)) {
                      VariableId memberUnionMemberVarId = cati.vidm.variableId(memberUnionMember);
                      insertVariableId(memberUnionMemberVarId);
                      // Recursion of insertVariableId(...) will insert other members and deeper nested union members:
                      break;
                    }
                  }
                }
                else {
                  throw CodeThorn::Exception("Nested union variable " + cati.vidm.variableName(unionMemberVarId) + ": Unable to find defining declaration of union " + memberRecordDecl->get_qualified_name().getString() + ".");
                  // (We could add all variables (that are members of an union) to the address-taken set instead.)
                }
              }
            }
          }
        }
      }
    }
  }
  else {
    // If there is no SgVariableDeclaration then the variable is a function parameter or a closure variable. In both cases
    //  the variable is no member.
  }
}

void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::insertFunctionId(FunctionId id) {
  ROSE_ASSERT(id.isValid());

  cati.functionAddressTakenInfo.second.insert(id);

  if(debuglevel > 0) {
    std::cout << "INFO: Added function id " << id.getIdCode() << " (Name: "
              << cati.fidm.getFunctionNameFromFunctionId(id) << ")"
              << std::endl;
  }
}

// void f() { }
// void (*f_ptr)() = &f;
// & on SgFunctionRefExp is redundant as the functions
// are implicity converted to function pointer
//
void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgFunctionRefExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  insertFunctionId(cati.fidm.getFunctionIdFromFunctionRef(sgn));
  // raise the flag
  cati.variableAddressTakenInfo.first = true;
}

void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgMemberFunctionRefExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  insertFunctionId(cati.fidm.getFunctionIdFromFunctionRef(sgn));
  // raise the flag
  // functions can potentially modify anything
  cati.variableAddressTakenInfo.first = true;
}

void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgTemplateFunctionRefExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  insertFunctionId(cati.fidm.getFunctionIdFromFunctionRef(sgn));
  // raise the flag
  // functions can potentially modify anything
  cati.variableAddressTakenInfo.first = true;
}
void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgTemplateMemberFunctionRefExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  insertFunctionId(cati.fidm.getFunctionIdFromFunctionRef(sgn));
  // raise the flag
  // functions can potentially modify anything
  cati.variableAddressTakenInfo.first = true;
}

void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgReturnStmt* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  SgFunctionDefinition* funcDef = SgNodeHelper::getClosestParentFunctionDefinitionOfLocatedNode(sgn);
  ROSE_ASSERT(funcDef);
  SgFunctionDeclaration * funcDecl = funcDef->get_declaration();
  ROSE_ASSERT(funcDecl);
  SgType* funcReturnType = funcDecl->get_type()->get_return_type();
  SgExpression* returnExpr = sgn->get_expression();
  ROSE_ASSERT(returnExpr);
  std::vector<VariableId> associationTargets;
  // TODO: all variables on the lhs of assignments that have a call of a function with the same
  //  type as this function on the rhs.
  handleAssociation(AK_Initialization, associationTargets, funcReturnType, returnExpr);
}

// A& foo() { return A(); }
// &(foo())
// if foo() returns a reference then foo() returns a lvalue
void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgFunctionCallExp* sgn)
{
  if(debuglevel > 0) {
    std::cout << std::endl << std::endl;
    debugPrint(sgn);
  }

  // schroder3 (Jul 2016):
  // Summary: Get the type of every parameter. If the type is a reference type, then add all
  //  variables used in the corresponding argument expression to the set of address taken variables.
  //  Example:
  //   int* getAddress(int& var) { return &var; }
  //   int main() {
  //     int i = 3;
  //     int* ip = getAddress(i);
  //   }
  // In addition, if a non-static member function is called, add the object on which it is called
  //  to the set of address taken variables because the object's address is accessible inside the
  //  member function via "this".

  // Callee function type:
  const SgFunctionType* calleeType = SgNodeHelper::getCalleeFunctionType(sgn);
  if(debuglevel > 0) {
    std::cout << "Callee type: " << calleeType->unparseToString() << std::endl;
  }

  // What kind of call do we have?
  enum CalleeKind {
    CK_Function = 1,
    CK_FunctionPointer = 2,
    CK_FunctionPointerReference = 4,
    CK_FunctionReference = 8,
    CK_FunctionPointerOrFunctionPointerReferenceOrFunctionReference = 14,
    CK_FunctionLike = 15,
    CK_MemberFunction = 16, // ., ->
    CK_MemberFunctionPointer = 32, // .*, ->*
    CK_MemberFunctionLike = 48,
    CK_Any = (CK_FunctionLike | CK_MemberFunctionLike)
  } calleeKind = CK_Any;

  enum MemberFunctionKind {
    MFK_None = 1,
    MFK_NonStatic = 2,
    MFK_Static = 4,
    MFK_Any = 7
  } memberFunctionKind = MFK_Any;

  enum BaseExprKind {
    BEK_None = 1,
    BEK_Pointer = 2,
    BEK_Object = 4,
    BEK_PointerOrObject = 6,
    BEK_Any = 15
  } baseExprKind = BEK_Any;

  const SgNode* firstChildOfCallExp = sgn->get_traversalSuccessorByIndex(0);

  if(isSgDotExp(firstChildOfCallExp) || isSgArrowExp(firstChildOfCallExp)) {
    // a.b or a->b (b is a member function or a member of function pointer/ reference type):

    // Determine the callee kind:
    // There should always be a member of function type or a member function (because something like a.( b ? c : d) is not allowed):
    const SgExpression* rhs = static_cast<const SgBinaryOp*>(firstChildOfCallExp)->get_rhs_operand();
    ROSE_ASSERT(rhs);
    ROSE_ASSERT(SgNodeHelper::isCallableExpression(const_cast<SgExpression*>(rhs)));
    if(const SgMemberFunctionRefExp* memberFunctionRefExpr = isSgMemberFunctionRefExp(rhs)) {
      calleeKind = CK_MemberFunction;
      // Check whether the called member function is static or not:
      // There should be at least a forward declaration:
      SgMemberFunctionDeclaration* memberFunctionDecl = memberFunctionRefExpr->getAssociatedMemberFunctionDeclaration();
      ROSE_ASSERT(memberFunctionDecl);
      if(debuglevel > 0) {
        std::cout << "Mem func decl: " << memberFunctionDecl->unparseToString() << std::endl;
      }
      // The static modifier is attached to the (implicit) forward declaration or to the defining declaration
      //  depending on how the declaration/definition is written in the source code. We therefore have to check
      //  both declarations for the presence of a static modifier.
      // First check whether the forward declaration is static:
      if(memberFunctionDecl->get_declarationModifier().get_storageModifier().isStatic()) {
        memberFunctionKind = MFK_Static;
      }
      else {
        // The forward declaration is not static. Check whether the defining declaration is static (if it exists):
        SgMemberFunctionDeclaration* memberFuncDefiningDecl = isSgMemberFunctionDeclaration(memberFunctionDecl->get_definingDeclaration());
        if(debuglevel > 0) {
          std::cout << "Mem func def decl: " << (memberFuncDefiningDecl ? memberFuncDefiningDecl->unparseToString() : std::string("none")) << std::endl;
        }
        if(memberFuncDefiningDecl && memberFuncDefiningDecl->get_declarationModifier().get_storageModifier().isStatic()) {
          memberFunctionKind = MFK_Static;
        }
        else {
          memberFunctionKind = MFK_NonStatic;
        }
      }
    }
    else if(isSgVarRefExp(rhs)) {
      // Distinction between function reference/ pointer/ reference to pointer gives no advantage here:
      calleeKind = CK_FunctionPointerOrFunctionPointerReferenceOrFunctionReference;
      // No member function:
      memberFunctionKind = MFK_None;
    }
    else {
      // Neither member function nor member on the rhs of dot or arrow operator: Not allowed:
      ROSE_ASSERT(false);
    }

    // Determine base expression kind:
    if(isSgDotExp(firstChildOfCallExp)) {
        // a.b (a is an object):
        baseExprKind = BEK_Object;
    }
    else if(isSgArrowExp(firstChildOfCallExp)) {
      // a->b (a is a pointer to an object):
      baseExprKind = BEK_Pointer;
    }
    else {
      ROSE_ASSERT(false);
    }
  }
  else if(isSgArrowStarOp(firstChildOfCallExp)) {
    // a->*b (a is a pointer to an object, b is a member function pointer):
    calleeKind = CK_MemberFunctionPointer;
    baseExprKind = BEK_Pointer;
    // There are no member function pointers to static member functions:
    memberFunctionKind = MFK_NonStatic;
  }
  else if(isSgDotStarOp(firstChildOfCallExp)) {
    // a.*b (a is an object, b is a member function pointer):
    calleeKind = CK_MemberFunctionPointer;
    baseExprKind = BEK_Object;
    // There are no member function pointers to static member functions:
    memberFunctionKind = MFK_NonStatic;
  }
  else {
    // No base expression:
    baseExprKind = BEK_None;
    // The callee is a (static member-)function, a function pointer (== "static member-function pointer")
    //  or a function (pointer) reference but a distinction between these kinds gives no advantage here:
    calleeKind = static_cast<CalleeKind>(CK_FunctionLike | CK_MemberFunction);
    memberFunctionKind = static_cast<MemberFunctionKind>(MFK_None | MFK_Static);
  }

  if(debuglevel > 0) {
    std::cout << "Call kinds: Callee: " << calleeKind << ", Base expr: " << baseExprKind << ", Mem func: " << memberFunctionKind << std::endl;
  }

  // In the Rose AST every call of a non-static member function should
  // have an explicit object on which the member function is called
  // and there therefore should be a dot, arrow, dot-star or
  // arrow-star operator for each non-static member function call.
  if(calleeKind == CK_MemberFunction && memberFunctionKind == MFK_NonStatic) {
    ROSE_ASSERT((baseExprKind & BEK_PointerOrObject) > 0);
    ROSE_ASSERT(isSgMemberFunctionType(calleeType));
  }

  // If this is a call of a non-static member function, then add the
  // object, on which the member function is called, to the address
  // taken set. This is necessary because inside a non-static member
  // function the address of the object is accessible via "this".
  if((calleeKind & CK_MemberFunctionLike) && (memberFunctionKind & MFK_NonStatic)) {
    if(debuglevel > 0) {
      std::cout << "Member function type: " << isSgExpression(firstChildOfCallExp)->get_type()->unparseToString() << std::endl;
    }
    // Get the object/ expression on which the member-function is
    // called: Only consider dot (a.b) and dot-star (a.*b)
    // expressions. We can ignore arrow (a->b) and arrow-star (a->*b)
    // because they expect a pointer on the left hand side and they
    // dereference this pointer to get the real object on which the
    // member function is called.  Therefore only variables from which
    // the address was already taken in an other way (e.g. via
    // address-of operator) are affected.
    if((baseExprKind & BEK_Object)) {
      SgExpression* memFuncCallBaseExpr = static_cast<const SgBinaryOp*>(firstChildOfCallExp)->get_lhs_operand();
      // Add all variables that are used in the base expression to the
      // address taken set:
      memFuncCallBaseExpr->accept(*this);
    }
    else if((baseExprKind & BEK_Pointer)) {
      // Nothing to do as described above.
    }
    else {
      throw CodeThorn::Exception("Invalid internal state. firstChildOfCallExp: " + firstChildOfCallExp->unparseToString());
    }
  }

  // defines and typedefs used:
  //  #define ROSE_STL_Container std::vector
  //  typedef Rose_STL_Container<SgExpression*> SgExpressionPtrList;
  //  typedef Rose_STL_Container<SgType*> SgTypePtrList;

  // Vector of argument expressions of the current function call:
  const SgExpressionPtrList& arguments = SgNodeHelper::getFunctionCallActualParameterList(sgn);

  // Vector of pointers to the parameter types that are declared for this function:
  SgTypePtrList parameterTypes;
  parameterTypes = calleeType->get_arguments();

  // schroder3 (2016-08-17): Check for an implicitly declared function
  // (e.g. gettimeofday(...) can be used without an include and
  // without an explicit declaration). This is only allowed in C and
  // it is deprecated starting with C99 (as far as i know). The type
  // of these functions is "int()" (even if they have parameters) and
  // there is therefore no information about the parameter types.
  if(parameterTypes.size() == 0 && arguments.size() > 0) {
    if(isSgTypeInt(calleeType->get_return_type())) {
      // The type is int(): Make sure that this is compiled as C code:
      SgNode* node = sgn;
      while(!isSgFile((node = node->get_parent())));
      SgFile* enclosingFile = isSgFile(node);
      ROSE_ASSERT(enclosingFile);
      if(enclosingFile->get_C_only()) {
        // in case it is C code: because C has no references, they
        // only possible address-taking is the function to pointer
        // conversion. Make the parameter type a function pointer type
        // if the argument type is a function type:
        for(SgExpressionPtrList::const_iterator i = arguments.begin(); i != arguments.end(); ++i) {
          SgType* parameterType = (*i)->get_type();
          if(isSgFunctionType(parameterType)) {
            parameterType = SgPointerType::createType(parameterType);
          }
          parameterTypes.push_back(parameterType);
        }
      }
    }

  }
  handleCall(parameterTypes, arguments);

  // we can look at its defintion and process the return expression ?
  // function calls can modify anything
  // raise the flag more analysis required
  cati.variableAddressTakenInfo.first = true;
}

void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgThisExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  // schroder3 (Jun 2016):
  //  If we have a "this" expression then we are inside an object context. If this "this" is used for a call of
  //  a member function then we should already have the object, to which "this" points, in our address taken
  //  variables set, because a call of a member function from a non object context is not possible without
  //  specifying an object.
}

void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgExpression* sgn) {
  if(debuglevel > 0) debugPrint(sgn);
  // schroder3 (2016-07-28): Some of SgExpression's subclasses (e.g. SgConditionalExp) are handled by specific
  //  visit member functions but if they are not then there should be nothing to do. It is for example possible to
  //  get here if this is the initializer expression of a const lvalue reference declaration and we look at this
  //  initializer because there might be a variable/ function (e.g. "const int& cir = (bool_var ? 42 : int_val);").
  //
  //  If this expression has a reference type then ignoring this expression would ignore a possible address-taking.
  //  Make sure that this does not happen:
  if(SgNodeHelper::isReferenceType(sgn->get_type())) {
    throw CodeThorn::Exception("Address-Taken Analysis: Unhandled " + sgn->class_name() + " node of reference type "
                           + sgn->get_type()->unparseToString() + ".");
  }
}

void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgFunctionParameterList* sgn) {
  if(debuglevel > 0) debugPrint(sgn);

  // schroder3 (2016-07-21): Iterate over parameters and check for default arguments
  //  (=initializations) and for parameters of reference type:
  const SgInitializedNamePtrList& parameterInitNames = sgn->get_args();
  for(SgInitializedNamePtrList::const_iterator i = parameterInitNames.begin();
      i != parameterInitNames.end(); ++i
  ) {
    SgSymbol* correspondingSymbol = (*i)->search_for_symbol_from_symbol_table();
    if(!correspondingSymbol) {
      // This should be a non-defining declaration then:
      SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(sgn->get_parent());
      ROSE_ASSERT(funcDecl);
      if(!funcDecl->get_definition()) {
        // Non-defining declaration. Two cases:
        //  1) There is a defining declaration in the AST and we will see it later.
        //  2) There is no defining declaration in the AST: There is nothing we could
        //     do, because there are no symbols for parameters of function declarations
        //     in the ROSE AST. (This is okay because it is optional to give
        //     parameters names. And even if there are names in the declaration then
        //     they are never used because only the names in the definition are used.)
        //  In both cases we do nothing:
        continue;
      }
      else {
        // A defining declaration resp. a definition should always have symbols for their
        //  parameters:
        throw CodeThorn::Exception("Unable to find symbol of parameter in defining declaration.");
      }
    }
    // There is a symbol available at this point.

    // Debug output:
    if(debuglevel > 0) {
      SgSymbol* correspondingSymbol = (*i)->search_for_symbol_from_symbol_table();
      std::cout << "Parameter: " << (*i)->unparseToString()
                << "(Symbol: " << (correspondingSymbol ? correspondingSymbol->get_name().getString() +
                    ", " + correspondingSymbol->class_name() : std::string("<none>"))
                << ")" << std::endl;
    }

    VariableId parameter = cati.vidm.variableId(*i);
    SgType* parameterType = (*i)->get_type();
    ROSE_ASSERT(parameterType);

    // The target of a possible default argument is the current parameter:
    std::vector<VariableId> associationTargets;
    associationTargets.push_back(parameter);

    // Check for default argument:
    if(SgAssignInitializer* defaultArgument = isSgAssignInitializer((*i)->get_initializer())) {
      // Handle alias/ reference creation and implicit address-taking inside the default argument
      //  initialization:
      handleAssociation(AK_Initialization, associationTargets, parameterType, defaultArgument->get_operand());
    }
    else {
      // Two cases:
      //   1) Initializer other than assign initializer: Handled separately
      //   2) No initializer
      //  In both cases only add the parameter to the address taken set if it is of
      //  reference type.
      if(SgNodeHelper::isReferenceType((*i)->get_type())) {
        insertVariableId(parameter);
      }
    }
  }
}

void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgDesignatedInitializer* sgn) {
  if(debuglevel > 0) debugPrint(sgn);

  // schroder3 (2016-08-15): Designated initializer allows to initiate members by providing their
  //  name in the initialization list. C only as far as is know. Not supported yet (TODO):
  std::cout << "WARNING: Designated initializers are not supported yet. Inserting all variables and"
                 " functions into the address-taken set." << std::endl;
  insertAllVariableIds();
  insertAllFunctionIds();
}

void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgCompoundInitializer* sgn) {
  if(debuglevel > 0) debugPrint(sgn);

  // schroder3 (2016-08-15): Compound literals: Constructs a temporary object or array from initialization list:
  //  alias/ reference creation in case of initialization of an object and implicit function to pointer conversion
  //  in both cases are possible.
  // TODO: Not supported yet:
  std::cout << "WARNING: Compound literals are not supported yet. Inserting all variables and"
               " functions into the address-taken set." << std::endl;
  insertAllVariableIds();
  insertAllFunctionIds();
}

void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgCompoundLiteralExp* sgn) {
  if(debuglevel > 0) debugPrint(sgn);

  // schroder3 (2016-08-15): Compound literal expression: currently this is a node without children
  //  (and a symbol as attribute) but there is no access to the particular initializations: Nothing to do (TODO):
  std::cout << "SgCompoundLiteralExp symbol: " << SgNodeHelper::symbolToString(sgn->get_symbol()) << std::endl;
  ROSE_ASSERT(sgn->get_numberOfTraversalSuccessors() == 0);
  std::cout << "WARNING: Compound literal expressions are not supported yet. Inserting all variables and"
               " functions into the address-taken set." << std::endl;
  insertAllVariableIds();
  insertAllFunctionIds();
}

void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgAggregateInitializer* sgn) {
  if(debuglevel > 0) debugPrint(sgn);

  // schroder3 (2016-08-15): aggregate initialization of an object or an array. In case of an object it
  //  is basically the same as a constructor call.
  if(const SgExprListExp* argumentExpressionListContainer = sgn->get_initializers()) {
    // TODO: Try to get the object member types resp. the array element type! Without this information
    //  put everything that occurs in one of the aggregate init expressions into the address-taken-set:
    const SgExpressionPtrList& argumentExpressions = argumentExpressionListContainer->get_expressions();
    for(SgExpressionPtrList::const_iterator i = argumentExpressions.begin();
        i != argumentExpressions.end(); ++i
    ) {
      (*i)->accept(*this);
    }
  }
}

void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgConstructorInitializer* sgn) {
  if(debuglevel > 0) debugPrint(sgn);

  // schroder3 (2016-07-20): Call of a constructor: Basically the same as a function call expr:
  //  Get the argument expressions and the parameter types:
  if(const SgExprListExp* argumentExpressionListContainer = sgn->get_args()) {
    const SgExpressionPtrList& argumentExpressions = argumentExpressionListContainer->get_expressions();
    if(argumentExpressions.size() > 0) {
      if(argumentExpressions.size() == 1 && isSgAggregateInitializer(argumentExpressions[0])) {
        // This aggregate initialization will be handled separately.
        return;
      }

      // Arguments are available.
      // Determine the parameter types:
      const SgMemberFunctionDeclaration* constructorDecl = sgn->get_declaration();
      // If we have arguments, then there should be a declaration (calls of a default copy constructor
      //  do currently not appear as SgConstructorInitializer in the AST):
      ROSE_ASSERT(constructorDecl);
      // Iterate over the parameters and extract the parameter types:
      const SgInitializedNamePtrList& parameterInitNames = constructorDecl->get_args();
      SgTypePtrList parameterTypes;
      for(SgInitializedNamePtrList::const_iterator i = parameterInitNames.begin();
          i != parameterInitNames.end(); ++i
      ) {
        parameterTypes.push_back((*i)->get_type());
      }
      // Handle the arguments in the same way as a normal (member) function call:
      handleCall(parameterTypes, argumentExpressions);
    }
  }
}

void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgCtorInitializerList* sgn) {
  if(debuglevel > 0) debugPrint(sgn);

  // schroder3 (2016-07-20): Constructor initializer list: There is a reference creation if
  //  the initialized member has reference type and there might be an implicit address-taking
  //  if the initialized member has function pointer type. Check every initialized member:
  SgInitializedNamePtrList& constructorInitializers = sgn->get_ctors();
  for (SgInitializedNamePtrList::const_iterator i = constructorInitializers.begin();
      i != constructorInitializers.end(); ++i
  ) {
    // Member initialization or delegated constructor call:
    if(isSgConstructorInitializer((*i)->get_initializer())) {
      // The constructor initializer is handled by the corresponding visit member function.
      continue;
    }
    SgInitializedName* initializedMember = *i;

    VariableId memberVariable = cati.vidm.variableId(initializedMember);
    ROSE_ASSERT(memberVariable.isValid());

    // We know the target of the initialization: the member:
    std::vector<VariableId> possibleTargets;
    possibleTargets.push_back(memberVariable);

    // Add the variables in the init expression:
    SgAssignInitializer* memberInitializer = isSgAssignInitializer(initializedMember->get_initializer());
    if(!memberInitializer) {
      // Other initializers are handled separately
      return;
    }

    // Handle the type of the member together with the initializer expression:
    handleAssociation(AK_Initialization, possibleTargets, initializedMember->get_type(), memberInitializer->get_operand());
  }
}

void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgAddressOfOp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  // schroder3 (2016-07-26): Two cases:
  //  a) We are currently looking for an implicit address-taking:
  //     An address-of operator was found and the address-taking is therefore not implicit
  //     ==> case b
  //  b) We are currently *not* looking for an implicit address-taking:
  //     The sub-tree of this address-of operator will be traversed separately ==> Do not
  //     traverse the sub-tree.
  //
  //  ==> nothing to do in both cases.
}

void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgThrowOp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);

  // schroder3 (2016-08-02): A throw can contain an implicit address taking e.g. if the throw
  //  expression is of function type and if there is a catch for function pointer types.
  //  (Alias/ reference creation through catch is not possible because throw copy-initializes
  //  the exception object from the throw expression.)
  if(sgn->get_throwKind() != SgThrowOp::throw_expression) {
    // Nothing to do if there is no expression (e.g. in case of a rethrow):
    return;
  }
  else {
    SgExpression* throwExpression = sgn->get_operand();
    ROSE_ASSERT(throwExpression);
    // Check for implicit address-taking:
    if(SgNodeHelper::isTypeEligibleForFunctionToPointerConversion(throwExpression->get_type())) {
      // Implicit address-taking possible. Four cases:
      //  1) There is an enclosing try:
      //     1.1) There is a matching catch statement: implicit address-taking found
      //     1.1) There is an ellipsis catch statement: catched variable is not accessible but can
      //          be rethrown: implicit address-taking possible
      //     1.2) There is no matching catch statement: there might be another try somewhere:
      //          implicit address-taking possible
      //  2) There is no enclosing try: there might be another try somewhere: implicit
      //     address-taking possible
      //
      //  ==> implicit address-taking in all cases possible:
      //  TODO: Refine this (e.g. if there is only an ellipsis catch without rethrow then the function
      //        address is not accessible)
      throwExpression->accept(*this);
    }
  }
}

void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgLambdaExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  // lambda captures are handled in SgLambdaCapture, the lambda body
  // is traversed and processed like any other function body.
}

void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgLambdaCapture* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);

  // schroder3 (2016-08-22): A lambda capture by reference creates an alias between the outside/ capture
  //  variable and the inside/ closure variable. Functions can not be captured.

  // Check for capture by reference
  if(sgn->get_capture_by_reference()) {
    // This capture creates the closure variable (a reference member of the lambda object which is of
    //  anonymous class type) as an alias of the capture variable/ this.

    SgExpression* captureExpression = sgn->get_capture_variable();

    SgVarRefExp* closureVariable = isSgVarRefExp(sgn->get_closure_variable());
    ROSE_ASSERT(closureVariable);
    VariableId closureVariableId = cati.vidm.variableId(closureVariable);
    ROSE_ASSERT(closureVariableId.isValid());
    const SgType* closureVarType = cati.vidm.getType(closureVariableId);

    // The capture can be a variable or the "this" pointer:
    if(SgVarRefExp* captureVariable = isSgVarRefExp(captureExpression)) {
      VariableId captureVariableId = cati.vidm.variableId(captureVariable);
      ROSE_ASSERT(captureVariableId.isValid());
      // Check the types:
      const SgType* captureVarType = cati.vidm.getType(captureVariableId);
      const SgType* captureVarBaseType = SgNodeHelper::isReferenceType(captureVarType)
                                       ? SgNodeHelper::getReferenceBaseType(captureVarType)
                                       : captureVarType;

      // Closure variable type should be a lvalue reference type:
      ROSE_ASSERT(SgNodeHelper::isLvalueReferenceType(closureVarType));

      // Capture variable should have the same base type as the closure variable:
      ROSE_ASSERT(captureVarBaseType->isEquivalentType(SgNodeHelper::getReferenceBaseType(closureVarType)));

      // The closure variable is the newly created reference that gets initialized with the capture variable:
      std::vector<VariableId> possibleTargets;
      possibleTargets.push_back(closureVariableId);

      // Handle the alias creation:
      handleAssociation(AK_Initialization, possibleTargets, closureVarType, captureExpression);
    }
    else if(isSgThisExp(captureExpression)) {
      // schroder3 (2016-08-25): The "this" pointer is captured by value (C++11) and this should therefore be
      //  unreachable code. However an implicit "this" capture is currently marked as by-reference if the capture
      //  default is by-reference. Ignore this capture because it does not include an alias creation.

      // Some consistency checks: The captured "this" is a pointer...
      ROSE_ASSERT(SgNodeHelper::isPointerType(captureExpression->get_type()));
      //  ... and the pointer is copied to the closure variable which is not of reference type but of pointer type:
      ROSE_ASSERT(!SgNodeHelper::isReferenceType(closureVarType));
      ROSE_ASSERT(SgNodeHelper::isPointerType(closureVarType));
    }
    else {
      ROSE_ASSERT(false);
    }
  }
}

void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::visit(SgNode* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  std::cerr << "unhandled operand " << sgn->class_name() << " of SgAddressOfOp in AddressTakenAnalysis\n";
  std::cerr << sgn->unparseToString() << std::endl;
  ROSE_ASSERT(0);
}

// schroder3 (2016-07-20): Handles the arguments of a constructor or (member) function call regarding their "address-taken-ness".
void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::handleCall(const SgTypePtrList& parameterTypes, const SgExpressionPtrList& argumentExpressions) {
  // Iterators:
  SgExpressionPtrList::const_iterator argIter = argumentExpressions.begin();
  SgTypePtrList::const_iterator paramTypeIter = parameterTypes.begin();
  // Look at each argument:
  while(argIter != argumentExpressions.end()) {
    // Get the current argument;
    /*const*/ SgExpression* argument = *argIter;
    if(debuglevel > 0) {
      std::cout << "argument: " << argument->unparseToString() << std::endl;
    }
    // Get the corresponding parameter type if existing:
    SgType* parameterType = 0;
    if(paramTypeIter != parameterTypes.end()) {
      parameterType = *paramTypeIter;
    }
    else{
      // More arguments than parameters: all following arguments are variadic

      // We should not get here because we should have seen an ellipse type in the
      //  previous iteration (which yields a break).
      ROSE_ASSERT(false);

      break;
    }

    if(!parameterType) {
      throw CodeThorn::Exception("No type of parameter!");
    }

    // Output the type as string if in debug mode:
    if(debuglevel > 0) {
      std::cout << "type: " << parameterType->unparseToString() << std::endl;
    }

    // Check whether the type is an ellipse type
    if(isSgTypeEllipse(parameterType)){
      // The function is variadic. I think that passing by reference through variadic arguments
      //  is not possible ==> nothing to do for this and following arguments ==> break

      // Ellipsis should be the last "parameter":
      ROSE_ASSERT(++paramTypeIter == parameterTypes.end());

      break;
    }

    // Possible parameters:
    std::vector<VariableId> possibleParameters;
    // TODO: Try to determine the function that is currently called. If this does not work,
    //  get all functions that could be called given the current function type.

    // If the parameter's type is a reference type:
    //  Add all variables that are used in the argument expression to the address taken set.
    // If the parameter's type is a function pointer type:
    //  Add all functions that are used in the argument expression and do not have a address-of
    //  operator to the address taken set
    // Handle both cases:
    handleAssociation(AK_Initialization, possibleParameters, parameterType, argument);

    ++argIter;
    ++paramTypeIter;
  }
}

// schroder3 (2016-07-20): Handles all kinds of associations (currently initializations and assignments) regarding their "address-taken-ness".
void CodeThorn::ComputeAddressTakenInfo::OperandToVariableId::handleAssociation(const AssociationKind associationKind, const std::vector<VariableId>& possibleTargetEntities,
                                                                            const SgType* targetEntityType, /*const*/ SgExpression* associatedExpression) {
  // TODO: As soon as every caller of this function provides a list of possible target entities the targetEntityType parameter can be removed.

  ROSE_ASSERT(targetEntityType);
  if(associatedExpression) {
    // Consistency check:
    for(std::vector<VariableId>::const_iterator i = possibleTargetEntities.begin();
        i != possibleTargetEntities.end(); ++i
    ) {
      ROSE_ASSERT((*i).isValid());
      ROSE_ASSERT(cati.vidm.getType(*i)->isEquivalentType(targetEntityType));
    }

    if(debuglevel > 0) {
      std::cout << "Association kind: " << associationKind << std::endl;
      std::cout << "Target type: " << targetEntityType->unparseToString() << std::endl;
      std::cout << "Associated expression: " << associatedExpression->unparseToString() << std::endl;
      std::cout << "Associated expression type: " << associatedExpression->get_type()->unparseToString() << std::endl;
    }

    // If we have a reference to pointer to function type then we handle it as a reference type and
    //  not as a function type.
    if(const SgType* targetEntityReferenceType = SgNodeHelper::isReferenceType(targetEntityType)) {
      if(debuglevel > 0) {
        std::cout << "... is reference type." << std::endl;
      }
      if(associationKind == AK_Initialization) {
        // Initialization of a reference: A reference creates an alias of the variable that is referenced by the reference.
        //  This alias works in both directions. For example if we have the code "int b = 0; int& c = b;" then we can
        //  change the value of b by using c ("c = 1;") and we can change the "value" of c by using b ("b = 2;"). We
        //  therefore have to add the reference variable as well as the variables in the init expression to the address
        //  taken set. (But we only add the reference variable if we have a init expression.)

        // Add all possible targets of this initialization to the address taken set:
        for(std::vector<VariableId>::const_iterator i = possibleTargetEntities.begin();
            i != possibleTargetEntities.end(); ++i
        ) {
          insertVariableId(*i);
        }

        // The type of the target entity is a reference type:
        //  Treat every variable in the expression as if there is a alias/ reference creation of the variable. Currently we
        //  add every variable in the expression to the adderss-taken set because we have no mapping between the target refernce and
        //  the variables in the expression. It is therefore currently not possible to add the variable to the address taken set only
        //  if the address of the target reference is taken.
        associatedExpression->accept(*this);
      }
      else if(associationKind == AK_Assignment) {
        // Assignment of a reference: The variable that is referenced by the reference is assigned.
        //  ==> no alias/ reference creation, but there might be an implicit address-taking in the assignment.
      }
      else if(associationKind == AK_Cast) {
        // Cast of a reference:
        //  ==> no alias/ reference creation, but there might be an implicit address-taking in the expression that is casted.
      }
      else {
        ROSE_ASSERT(false);
      }

      // Remove the reference for the following implicit address-taking check:
      targetEntityType = SgNodeHelper::getReferenceBaseType(targetEntityReferenceType);
    }

    // There is an implicit address-taking in the association if the target entity is of function
    //  pointer type and if the associated expression is of function (reference) type:
    if(SgNodeHelper::isFunctionPointerType(targetEntityType)
       && SgNodeHelper::isTypeEligibleForFunctionToPointerConversion(associatedExpression->get_type())
    ) {
      if(debuglevel > 0) {
        std::cout << "... found implicit address-taking." << std::endl;
      }
      // Traverse the expression and only add functions or variables of function reference type to the
      //  address-taken set (and do not add variables of non function reference type):
      associatedExpression->accept(*this);
    }
  }
  else {
    // No expression ==> no alias/ reference creation and no implicit function address-taking. ==> nothing to do
  }
}

void CodeThorn::ComputeAddressTakenInfo::computeAddressTakenInfo(SgNode* root) {
  // schroder3 (2016-08-09): Check whether the ASTMatching should should be used to determine
  //  all address-taken-relevant nodes:
  if(useASTMatching) {
    computeAddressTakenInfoUsingASTMatching(root);
    // Do not compute the addess-taken-set twice:
    return;
  }
  // else: Do not use the ASTMatching:

  // schroder3 (2016-08-08): Create AST visitor, iterate over the AST, and handle all nodes that are relevant
  //  for the address-taken-analysis:
  OperandToVariableId optovid(*this);
  RoseAst ast(root);
  for(RoseAst::iterator i = ast.begin(); i != ast.end(); ++i) {

    // schroder3 (2016-07-29): Skip all nodes that are located in a sub-tree of a template declaration or definition
    //  because these nodes are never executed. They are never executed because there is always a specialization
    //  (SgTemplateInstatiation... node) that is used (and which is not skipped). Even in case of an implicit specialization the content
    //  of the template declaration/ definition is copied to specialization/ instantiation node (which is not skipped) and the
    //  address-takings can be found in these copies. It is is possible to deactivate the skipping of these nodes by setting
    //  addAddressTakingsInsideTemplateDecls to true.
    if(!addAddressTakingsInsideTemplateDecls
       && (isSgTemplateFunctionDeclaration(*i)
       || isSgTemplateMemberFunctionDeclaration(*i)
       || isSgTemplateVariableDeclaration(*i)
       || isSgTemplateClassDefinition(*i)
       || isSgTemplateFunctionDefinition(*i))
    ) {
      i.skipChildrenOnForward();
    }

    // schroder3 (Jun 2016): The obvious one:
    else if(SgAddressOfOp* addressOf = isSgAddressOfOp(*i)) {
      addressOf->get_operand()->accept(optovid);
    }

    // schroder3 (2016-07-19): An assignment can contain an implicit function address-taking.
    else if(SgAssignOp* assignment = isSgAssignOp(*i)) {
      // Only traverse the rhs of the assignment and only if there is an implicit address-taking:
      std::vector<VariableId> assignmentTargets;
      // TODO: All possible variables in the lhs of this assignment.
      optovid.handleAssociation(OperandToVariableId::AK_Assignment, assignmentTargets,
                                assignment->get_lhs_operand()->get_type(), assignment->get_rhs_operand());
    }

    // schroder3 (2016-07-28): A cast can contain an implicit address taking e.g. if the operand is of function type
    //  and if the cast type is a function pointer type.
    else if(SgCastExp* castExpr = isSgCastExp(*i)) {
      // Traverse the operand if there is an implicit address-taking:
      // The "target" of the association is the parent expression but there is no variable
      optovid.handleAssociation(OperandToVariableId::AK_Cast, std::vector<VariableId>(),
                                castExpr->get_type(), castExpr->get_operand());
    }
    else if(

    // schroder3 (Jun 2016): Call by reference creates an alias of the argument and if a non-static member function
    //  is called on an object then the address of that object is accessible in the member function via "this"
    // schroder3 (2016-07-19): ... and the address is implicitly taken if a function is
    //  provided as an argument for a parameter of function pointer type
            isSgFunctionCallExp(*i)
    // schroder3 (2016-07-20): The same (except the call on an object) applies to a constructor call:
         || isSgConstructorInitializer(*i)

    // schroder3 (2016-07-20): There will be an alias/ reference creation for every function parameter of reference type
    //  TODO: Replace this by a more precise method (e.g. try to add reference parameters when handling function calls)
         || isSgFunctionParameterList(*i)

    // schroder3 (Jun 2016): Creating a reference creates an alias of a variable
    // schroder3 (2016-07-19): ... and a variable declaration can contain an implicit function address-taking
         || isSgVariableDeclaration(*i)

    // schroder3 (2016-07-20): Initializing a member reference variable creates an alias
    // schroder3 (2016-07-20): ... and initializing a member of function pointer type with a function (reference)
    //  implicitly takes the address.
         || isSgCtorInitializerList(*i)

    // schroder3 (2016-07-20): Returning a variable from a function with reference return type creates an alias
    // schroder3 (2016-07-20): ... and returning a function from a function with function pointer return type
    //  can contain an implicit function address-taking.
         || isSgReturnStmt(*i)

    // schroder3 (2016-07-26): A dereference/ indirection operator can contain an implicit address taking if
    //  the operand is of function type.
         || isSgPointerDerefExp(*i)

    // schroder3 (2016-08-02): A throw can contain an implicit address taking e.g. if the throw expression is of
    //  function type and if there is a catch for function pointer types.
         || isSgThrowOp(*i)

    // schroder3 (2016-08-15): Aggregate/ "brace" initialization of an object or an array: alias/ reference creation
    //  in case of initialization of an object and implicit function to pointer conversion in both cases are possible.
         || isSgAggregateInitializer(*i)

    // schroder3 (2016-08-15): Compound literals: Constructs a temporary object or array from initialization list:
    //  alias/ reference creation in case of initialization of an object and implicit function to pointer conversion
    //  in both cases are possible.
         || isSgCompoundInitializer(*i)
         || isSgCompoundLiteralExp(*i)

    // schroder3 (2016-08-15): Designated initializers allow to initiate members by providing their
    //  name in the initialization list. C only as far as is know.
         || isSgDesignatedInitializer(*i)

   // schroder3 (2016-08-22): A lambda capture by reference creates an alias between the outside/ capture variable and
   //  the inside/ closure variable.
        || isSgLambdaCapture(*i)

    ) {
      // schroder3 (2016-08-08): Look for address-takings:
      (*i)->accept(optovid);
    }
  }
}

// schroder3 (2016-08-09): Alternative to computeAddressTakenInfo(...) that uses the ASTMatching mechanism.
//  This is currently slower than computeAddressTakenInfo(...).
void CodeThorn::ComputeAddressTakenInfo::computeAddressTakenInfoUsingASTMatching(SgNode* root) {
  // query to match all subtrees of interest
  // process query
  ProcessQuery collectAddressTakingRelatedNodes;
  std::string matchquery;

  // schroder3 (2016-07-29): Skip all nodes that are located in a sub-tree of a template declaration or definition,
  //  because these nodes are never executed. They are never executed because there is always a specialization
  //  (SgTemplateInstatiation... node) that is used (and which is not skipped). Even in case of an implicit specialization the content
  //  of the template declaration/ definition is copied to specialization/ instantiation node (which is not skipped) and the
  //  address-takings can be found in these copies. It is is possible to deactivate the skipping of these nodes by setting
  //  addAddressTakingsInsideTemplateDecls to true.
  if(!addAddressTakingsInsideTemplateDecls) {
    // schroder3 (2016-07-29): Uncommented the "#SgTemplate..." query parts because they have an effect now
    //  (Address-Taken-Analysis works with template functions now and member function definitions inside template class
    //  declarations should be available soon).
    // schroder3 (2016-07-20): Commented out the "#SgTemplate..." query parts because they do not have an effect.
    matchquery =
      "#SgTemplateClassDeclaration|"
      "#SgTemplateFunctionDeclaration|"
      "#SgTemplateMemberFunctionDeclaration|"
      "#SgTemplateVariableDeclaration|"
      "#SgTemplateClassDefinition|"
      "#SgTemplateFunctionDefinition|"
    ;
  }

  matchquery +=
    // schroder3 (Jun 2016): The obvious one:
    "SgAddressOfOp($OP)|"

    // schroder3 (Jun 2016): Call by reference creates an alias of the argument and if a non-static member function
    //  is called on an object then the address of that object is accessible in the member function via "this"
    // schroder3 (2016-07-19): ... and the address is implicitly taken if a function is
    //  provided as an argument for a parameter of function pointer type
    "$OP=SgFunctionCallExp|"
    // schroder3 (2016-07-20): The same (except the call on an object) applies to a constructor call:
    "$OP=SgConstructorInitializer|"

    // schroder3 (2016-07-20): There will be an alias/ reference creation for every function parameter of reference type
    //  TODO: Replace this by a more precise method (e.g. try to add reference parameters when handling function calls)
    "$OP=SgFunctionParameterList|"

    // schroder3 (Jun 2016): Creating a reference creates an alias of a variable
    // schroder3 (2016-07-19): ... and a variable declaration can contain an implicit function address-taking
    "$OP=SgVariableDeclaration|"

    // schroder3 (2016-07-20): Initializing a member reference variable creates an alias
    // schroder3 (2016-07-20): ... and initializing a member of function pointer type with a function (reference)
    // implicitly takes the address.
    "$OP=SgCtorInitializerList|"

    // schroder3 (2016-07-20): Returning a variable from a function with reference return type creates an alias
    // schroder3 (2016-07-20): ... and returning a function from a function with function pointer return type
    //  can contain an implicit function address-taking.
    "$OP=SgReturnStmt|"

    // schroder3 (2016-07-19): An assignment can contain an implicit function address-taking.
    "$OP=SgAssignOp|"

    // schroder3 (2016-07-26): A dereference/ indirection operator can contain an implicit address taking if
    //  the operand is of function type.
    "$OP=SgPointerDerefExp|"

    // schroder3 (2016-07-28): A cast can contain an implicit address taking e.g. if the operand is of function type
    //  and if the cast type is a function pointer type.
    "$OP=SgCastExp|"

    // schroder3 (2016-08-02): A throw can contain an implicit address taking e.g. if the throw expression is of
    //  function type and if there is a catch for function pointer types.
    "$OP=SgThrowOp";

  MatchResult& matches = collectAddressTakingRelatedNodes(matchquery, root);
  for(MatchResult::iterator it = matches.begin(); it != matches.end(); ++it) {
    SgNode* matchedNode = (*it)["$OP"];
    // SgNode* head = (*it)["$HEAD"];
    // debugPrint(head); debugPrint(matchedOperand);
    OperandToVariableId optovid(*this);
    if(const SgAssignOp* assignment = isSgAssignOp(matchedNode)) {
      // Only traverse the rhs of the assignment and only if there is an implicit address-taking:
      std::vector<VariableId> assignmentTargets;
      // TODO: All possible variables in the lhs of this assignment.
      optovid.handleAssociation(OperandToVariableId::AK_Assignment, assignmentTargets,
                                assignment->get_lhs_operand()->get_type(), assignment->get_rhs_operand());
    }
    else if(const SgCastExp* castExpr = isSgCastExp(matchedNode)) {
      // Traverse the operand if there is an implicit address-taking:
      // The "target" of the association is the parent expression but there is no variable
      optovid.handleAssociation(OperandToVariableId::AK_Cast, std::vector<VariableId>(),
                                castExpr->get_type(), castExpr->get_operand());
    }
    else {
      matchedNode->accept(optovid);
    }
  }
}

// pretty print
void CodeThorn::ComputeAddressTakenInfo::printAddressTakenInfo()
{
  std::cout << "addressTakenSet: [" << (variableAddressTakenInfo.first? "true, " : "false, ")
            << VariableIdSetPrettyPrint::str(variableAddressTakenInfo.second, vidm) << "]\n";
  std::cout << "functionAddressTakenSet: ["
            << fidm.getFunctionIdSetAsString(functionAddressTakenInfo.second) << "]\n";
}

/*************************************************
 **************** TypeAnalysis *******************
 *************************************************/

VariableIdSet CodeThorn::CollectTypeInfo::getPointerTypeSet()
{
  return pointerTypeSet;
}

VariableIdSet CodeThorn::CollectTypeInfo::getArrayTypeSet()
{
  return arrayTypeSet;
}

VariableIdSet CodeThorn::CollectTypeInfo::getReferenceTypeSet()
{
  return referenceTypeSet;
}

void CodeThorn::CollectTypeInfo::collectTypes()
{
  if(varsUsed.size() == 0) {
    // get the entire set from VariableIdMapping
    varsUsed = vidm.getVariableIdSet();
  }

  for(VariableIdSet::iterator it = varsUsed.begin(); it != varsUsed.end(); ++it)
  {
    // Note on function pointer types
    // function pointer can modify any variable
    // not just the variables in the addressTakenSet
    // answering function pointer derefence requires side
    // effect analysis to determine the list of variables
    // can modify. Currenty we ignore function pointers as
    // the goal of this analysis is supposed to be simple.
    if(vidm.hasPointerType(*it))
    {
      SgType* baseType = vidm.getType(*it)->findBaseType();
      // perhaps its worthwile to keep them in
      // a separte set and not support any dereferencing
      // queries rather than not adding them
      if(!isSgFunctionType(baseType))
      {
        pointerTypeSet.insert(*it);
      }
    }
    else if(vidm.hasArrayType(*it))
    {
      arrayTypeSet.insert(*it);
    }
    else if(vidm.hasReferenceType(*it))
    {
      referenceTypeSet.insert(*it);
    }
  }
}

void CodeThorn::CollectTypeInfo::printPointerTypeSet()
{
  std::cout << "pointerTypeSet: " << VariableIdSetPrettyPrint::str(pointerTypeSet, vidm) << "\n";
}

void CodeThorn::CollectTypeInfo::printArrayTypeSet()
{
  std::cout << "arrayTypeSet: " << VariableIdSetPrettyPrint::str(arrayTypeSet, vidm) << "\n";
}

void CodeThorn::CollectTypeInfo::printReferenceTypeSet()
{
  std::cout << "referenceTypeSet: " << VariableIdSetPrettyPrint::str(referenceTypeSet, vidm) << "\n";
}

/*************************************************
 ********** FlowInsensitivePointerInfo  **********
 *************************************************/

void CodeThorn::FlowInsensitivePointerInfo::collectInfo()
{
  compAddrTakenInfo.computeAddressTakenInfo(root);
  collTypeInfo.collectTypes();
}

void CodeThorn::FlowInsensitivePointerInfo::printInfoSets()
{
  compAddrTakenInfo.printAddressTakenInfo();
  collTypeInfo.printPointerTypeSet();
  collTypeInfo.printArrayTypeSet();
  collTypeInfo.printReferenceTypeSet();
}

VariableIdMapping& CodeThorn::FlowInsensitivePointerInfo::getVariableIdMapping()
{
  return vidm;
}

VariableIdSet CodeThorn::FlowInsensitivePointerInfo::getMemModByPointer()
{
  return getAddressTakenVariables();
}

VariableIdSet CodeThorn::FlowInsensitivePointerInfo::getAddressTakenVariables() {
  VariableIdSet unionSet;
  VariableIdSet addrTakenSet = (compAddrTakenInfo.getVariableAddressTakenInfo()).second;
  VariableIdSet arrayTypeSet = collTypeInfo.getArrayTypeSet();
  
  // std::set_union(addrTakenSet.begin(), addrTakenSet.end(),
  //                       arrayTypeSet.begin(), arrayTypeSet.end(),
  //                       unionSet.begin());

  // we can perhaps cache this for efficiency
  // to answer queries for multiple dereferencing queries
  CodeThorn::set_union(addrTakenSet, arrayTypeSet, unionSet);

  return unionSet;
}

FunctionIdSet CodeThorn::FlowInsensitivePointerInfo::getAddressTakenFunctions() {
  return compAddrTakenInfo.getFunctionAddressTakenInfo().second;
}
