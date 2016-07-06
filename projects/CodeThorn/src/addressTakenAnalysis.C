/*****************************************
 * Author: Sriram Aananthakrishnan, 2013 *
 *****************************************/

#include "sage3basic.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include "addressTakenAnalysis.h"

using namespace CodeThorn;
using namespace SPRAY;

/*************************************************
 ***************** ProcessQuery  *****************
 *************************************************/

MatchResult& SPRAY::ProcessQuery::getMatchResult()
{
  return match_result;
}

void SPRAY::ProcessQuery::printMatchResult()
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
      std::cout << "  VAR: " << (*smbIt).first << "=" << 
        SPRAY::AstTerm::astTermWithNullValuesToString(matchedTerm) << " @" << matchedTerm << std::endl;
    }
  }
}

void SPRAY::ProcessQuery::clearMatchResult()
{
  match_result.clear();
}

MatchResult& SPRAY::ProcessQuery::operator()(std::string query, SgNode* root)
{
  AstMatching m;
  match_result = m.performMatching(query, root);
  return match_result;
}

/*************************************************
 ************* ComputeAddressTakenInfo  **********
 *************************************************/

SPRAY::ComputeAddressTakenInfo::VariableAddressTakenInfo SPRAY::ComputeAddressTakenInfo::getVariableAddressTakenInfo()
{
  return variableAddressTakenInfo;
}

SPRAY::ComputeAddressTakenInfo::FunctionAddressTakenInfo SPRAY::ComputeAddressTakenInfo::getFunctionAddressTakenInfo()
{
  return functionAddressTakenInfo;
}

void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::debugPrint(SgNode* sgn)
{
  std::cerr << sgn->class_name() << ": " 
            << SPRAY::AstTerm::astTermWithNullValuesToString(sgn) << ", " \
            << sgn->unparseToString() << ", "                           \
            << sgn->get_file_info()->get_filenameString() << ", "       \
            << sgn->get_file_info()->get_line() << ", " \
            << std::endl;
}

// base case for the recursion
void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgVarRefExp *sgn)
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
  cati.variableAddressTakenInfo.second.insert(id);

  if(debuglevel > 0) {
    std::cout << "INFO: Added id " << id.getIdCode() << " (Symbol "
        << cati.vidm.getSymbol(id)->unparseToString()
        << " (" << SgNodeHelper::symbolToString(cati.vidm.getSymbol(id)) << "))"
        << std::endl;
  }
}


void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgVariableDeclaration* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  // Check if this is a declaration of a reference:
  SgInitializedName* varDeclInitName = SgNodeHelper::getInitializedNameOfVariableDeclaration(sgn);
  ROSE_ASSERT(varDeclInitName);
  SgType* varDeclType = varDeclInitName->get_type();
  ROSE_ASSERT(varDeclType);
  if(SgReferenceType* varDeclReferenceType = isSgReferenceType(varDeclType)) {
    // schroder3 (Jul 2016):
    //  Declaration of a reference: A reference creates an alias of the variable that is referenced by the reference.
    //  This alias works in both directions. For example if we have the code "int b = 0; int& c = b;" then we can
    //  change the value of b by using c ("c = 1;") and we can change the "value" of c by using b ("b = 2;"). We
    //  therefore have to add the reference variable as well as the variables in the init expression to the address
    //  taken set.

    //  Reference variable:
    VariableId refVarid = cati.vidm.variableId(sgn);
    cati.variableAddressTakenInfo.second.insert(refVarid);

    //  Init expression:
    //  Currently we treat every variable in the init expression of the reference as if it's address was taken, because
    //  we currently have no mapping between the reference and the variables in the init expression. It is therefore
    //  currently not possible to add the variable to the address taken set only if the address of the reference is taken.
    SgExpression* varDeclInitExpr = SgNodeHelper::getInitializerExpressionOfVariableDeclaration(sgn);
    if(debuglevel > 0) {
      std::cout << "INFO: Reference declaration of type " << varDeclReferenceType->unparseToString() << std::endl;
      std::cout << "INFO: Type of init expression: " << varDeclInitExpr->get_type()->unparseToString() << std::endl;
    }
    varDeclInitExpr->accept(*this);
  }
}

// only the rhs_op of SgDotExp is modified
// recurse on rhs_op
void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgDotExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  SgNode* rhs_op = sgn->get_rhs_operand();
  rhs_op->accept(*this);
}

// only the rhs_op of SgDotExp is modified
// recurse on rhs_op
void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgArrowExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  SgNode* rhs_op = sgn->get_rhs_operand();
  rhs_op->accept(*this);
}

// For example q = &(*p) where both q and p are pointer types
// In the example, q can potentially modify all variables pointed to by p
// same as writing q = p.
// Since we dont know anything about p, q can potentially modify all 
// the elements in addressTakenSet as result of the above expression 
// As a result, the variables whose addresses can be taken is the entire 
// set as we dont have any idea about p
// We dont need to add any new variable to addressTakenSet 
// as a consequence of the expressions similar to above.
void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgPointerDerefExp* sgn)
{
 if(debuglevel > 0) debugPrint(sgn);
  // we raise a flag
  cati.variableAddressTakenInfo.first = true;
}

// For example &(A[B[C[..]]]) or &(A[x][x][x])
// any pointer can that takes this address can modify
// contents of only A. The inner index expressions are r-values
// it is sufficient to add A to addressTakenSet
// keep recursing on the lhs until we find A
void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgPntrArrRefExp* sgn)
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
void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgAssignOp* sgn)
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
void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgPlusPlusOp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  SgNode* operand = sgn->get_operand();
  operand->accept(*this);
}

// same as prefix increment
// &(--i)
void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgMinusMinusOp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  SgNode* operand = sgn->get_operand();
  operand->accept(*this);
}

// &(a+1, b)
// b's address is taken
// keep recursing on the rhs_op
void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgCommaOpExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  SgNode* rhs_op = sgn->get_rhs_operand();
  rhs_op->accept(*this);
}

// if we see SgConditionalExp as operand of &
// both true and false branch are lvalues
// recurse on both of them to pick up the lvalues
void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgConditionalExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  SgNode* true_exp = sgn->get_true_exp();
  SgNode* false_exp = sgn->get_false_exp();
  true_exp->accept(*this);
  false_exp->accept(*this);
}

// cast can be lvalue
// example :  &((struct _Rep *)((this) -> _M_data()))[-1] expr from stl_list
void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgCastExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  SgNode* operand = sgn->get_operand();
  operand->accept(*this);
}

void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::insertFunctionId(FunctionId id) {
  ROSE_ASSERT(id.isValid());
  if(debuglevel > 0) {
    std::cout << "function symbol " << id.toString(cati.fidm) << std::endl;
    std::cout << "count" << cati.functionAddressTakenInfo.second.count(id) << std::endl;
  }
  cati.functionAddressTakenInfo.second.insert(id);
}

// void f() { }
// void (*f_ptr)() = &f;
// & on SgFunctionRefExp is redundant as the functions
// are implicity converted to function pointer
//
void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgFunctionRefExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  insertFunctionId(cati.fidm.getFunctionIdFromFunctionRef(sgn));
  // raise the flag
  cati.variableAddressTakenInfo.first = true;
}

void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgMemberFunctionRefExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  insertFunctionId(cati.fidm.getFunctionIdFromFunctionRef(sgn));
  // raise the flag
  // functions can potentially modify anything
  cati.variableAddressTakenInfo.first = true;
}

void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgTemplateFunctionRefExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  insertFunctionId(cati.fidm.getFunctionIdFromFunctionRef(sgn));
  // raise the flag
  // functions can potentially modify anything
  cati.variableAddressTakenInfo.first = true;
}
void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgTemplateMemberFunctionRefExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  insertFunctionId(cati.fidm.getFunctionIdFromFunctionRef(sgn));
  // raise the flag
  // functions can potentially modify anything
  cati.variableAddressTakenInfo.first = true;
}

// A& foo() { return A(); }
// &(foo())
// if foo() returns a reference then foo() returns a lvalue
void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgFunctionCallExp* sgn)
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
    CK_Unknown = 0,
    CK_Function = 1,
    CK_FunctionPointer= 2,
    CK_FunctionOrFunctionPointer = 3,
    CK_MemberFunction = 4, // ., ->
    CK_MemberFunctionPointer = 8, // .*, ->*
    CK_MemberFunctionOrMemberFunctionPointer = 12
  } calleeKind = CK_Unknown;

  enum MemberFunctionKind {
    MFK_Unknown = 0,
    MFK_None = 1,
    MFK_NonStatic= 2,
    MFK_Static= 4
  } memberFunctionKind = MFK_Unknown;

  enum BaseExprKind {
    BEK_Unknown = 0,
    BEK_None = 1,
    BEK_Pointer = 2,
    BEK_Object = 4,
    BEK_PointerOrObject = 6,
  } baseExprKind = BEK_Unknown;

  const SgNode* firstChildOfCallExp = sgn->get_traversalSuccessorByIndex(0);

  if(isSgDotExp(firstChildOfCallExp)) {
    // a.b (a is an object, b is a member function):
    calleeKind = CK_MemberFunction;
    baseExprKind = BEK_Object;
    // static/non-static will be determined later.
  }
  else if(isSgArrowExp(firstChildOfCallExp)) {
    // a->b (a is a pointer to an object, b is a member function):
    calleeKind = CK_MemberFunction;
    baseExprKind = BEK_Pointer;
    // static/non-static will be determined later.
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
    // The callee is a (static member-)function or a function pointer (== "static member-function pointer").
    //  but a distinction between these kinds gives not advantage here:
    calleeKind = CK_Unknown;
    memberFunctionKind = MFK_Unknown;

  }

  if(calleeKind == CK_MemberFunction) {
    // Check whether the called member function is static or not:

    // We should always have a member function ref expr because something like a.( b ? c : d) is not allowed:
    SgMemberFunctionRefExp* memberFunctionRefExpr = isSgMemberFunctionRefExp(static_cast<const SgBinaryOp*>(firstChildOfCallExp)->get_rhs_operand());
    ROSE_ASSERT(memberFunctionRefExpr);
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

  if(debuglevel > 0) {
    std::cout << "Call kinds: Callee: " << calleeKind << ", Base expr: " << baseExprKind << ", Mem func: " << memberFunctionKind << std::endl;
  }

  // In the Rose AST every call of a non-static member function should have an explicit object on which the member function is called and
  //  there therefore should be a dot, arrow, dot-star or arrow-star operator for each non-static member function call.
  if(calleeKind == CK_MemberFunction && memberFunctionKind == MFK_NonStatic) {
    ROSE_ASSERT((baseExprKind & BEK_PointerOrObject) > 0);
    ROSE_ASSERT(isSgMemberFunctionType(calleeType));
  }

  // If this is a call of a non-static member function, then add the object, on which the member function
  //  is called, to the address taken set. This is necessary because inside a non-static member function
  //  the address of the object is accessible via "this".
  if((calleeKind & CK_MemberFunctionOrMemberFunctionPointer)
      && (memberFunctionKind == MFK_NonStatic || memberFunctionKind == MFK_Unknown)
  ) {
    if(debuglevel > 0) {
      std::cout << "Member function type: " << isSgExpression(firstChildOfCallExp)->get_type()->unparseToString() << std::endl;
    }
    // Get the object/ expression on which the member-function is called:
    //  Only consider dot (a.b) and dot-star (a.*b) expressions. We can ignore arrow (a->b)
    //  and arrow-star (a->*b) because they expect a pointer on the left hand side and they
    //  dereference this pointer to get the real object on which the member function is called.
    //  Therefore only variables from which the address was already taken in an other way (e.g. via
    //  address-of operator) are affected.
    if(baseExprKind == BEK_Object) {
      SgExpression* memFuncCallBaseExpr = static_cast<const SgBinaryOp*>(firstChildOfCallExp)->get_lhs_operand();
      // Add all variables that are used in the base expression to the address taken set:
      memFuncCallBaseExpr->accept(*this);
    }
    else if(baseExprKind == BEK_Pointer) {
      // Nothing to do as described above.
    }
    else {
      std::cout << "ERROR: Invalid internal state. firstChildOfCallExp:" << firstChildOfCallExp->unparseToString() << std::endl;
      exit(1);
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
  // schroder3 (2016-06-28): TODO: SgFunctionCallExp::getAssociatedFunctionSymbol does not return correct symbol
  //  in case of call of result of call ("get_func()()"). Therefore determineFunctionDefinition does not return
  //  the correct definition. We will use the parameter types of the callee function type instead
//  if(SgFunctionDefinition* functionDefinition = SgNodeHelper::determineFunctionDefinition(sgn)) {
//    SgInitializedNamePtrList& parameterInitNames = SgNodeHelper::getFunctionDefinitionFormalParameterList(functionDefinition);
//    for(SgInitializedNamePtrList::const_iterator paramInitNamesIter = parameterInitNames.begin();
//        paramInitNamesIter != parameterInitNames.end();
//      ++paramInitNamesIter
//    ) {
//      parameterTypes.push_back((*paramInitNamesIter)->get_type());
//    }
//  }
//  else {
//    std::cout << "INFO: no function definition found for call " << sgn->unparseToString()
//            << ". Will use parameter types of callee type instead." << std::endl;
    parameterTypes = calleeType->get_arguments();
//  }

  // Iterators:
  SgExpressionPtrList::const_iterator argIter = arguments.begin();
  SgTypePtrList::const_iterator paramTypeIter = parameterTypes.begin();
  // Look at each argument:
  while(argIter != arguments.end()) {
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
      std::cout << "ERROR: No type of parameter!" << std::endl;
      exit(1);
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

    // Check whether the type is a reference type:
    if(isSgReferenceType(parameterType)) {
      // Add all variables that are used in the argument expression to the address taken set:
      argument->accept(*this);
    }

    ++argIter;
    ++paramTypeIter;
  }

  // we can look at its defintion and process the return expression ?
  // function calls can modify anything
  // raise the flag more analysis required
  cati.variableAddressTakenInfo.first = true;
}

void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgThisExp* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  // schroder3 (Jun 2016):
  //  If we have a "this" expression then we are inside an object context. If this "this" is used for a call of
  //  a member function then we should already have the object, to which "this" points, in our address taken
  //  variables set, because a call of a member function from a non object context is not possible without
  //  specifying an object.
}

void SPRAY::ComputeAddressTakenInfo::OperandToVariableId::visit(SgNode* sgn)
{
  if(debuglevel > 0) debugPrint(sgn);
  std::cerr << "unhandled operand " << sgn->class_name() << " of SgAddressOfOp in AddressTakenAnalysis\n";
  std::cerr << sgn->unparseToString() << std::endl;
  ROSE_ASSERT(0);
}

void SPRAY::ComputeAddressTakenInfo::computeAddressTakenInfo(SgNode* root)
{
  // query to match all SgAddressOfOp subtrees
  // process query
  ProcessQuery collectSgAddressOfOp;
  // TODO: not sufficient to pick up address taken by function pointers
  std::string matchquery;

// "#SgTemplateArgument|"
// "#SgTemplateArgumentList|"
// "#SgTemplateParameter|"
// "#SgTemplateParameterVal|"
// "#SgTemplateParamterList|"
  
  // skipping all template declaration specific nodes as they dont have any symbols
  // we still traverse SgTemplateInstatiation*
  matchquery = \
    "#SgTemplateClassDeclaration|"
    "#SgTemplateFunctionDeclaration|"
    "#SgTemplateMemberFunctionDeclaration|"
    "#SgTemplateVariableDeclaration|"
    "#SgTemplateClassDefinition|"
    "#SgTemplateFunctionDefinition|"
    "$HEAD=SgAddressOfOp($OP)|"
    // schroder3 (Jun 2016): Call by reference creates an alias of the argument
    "$OP=SgFunctionCallExp|"
    // schroder3 (Jun 2016): Creating a reference creates an alias of a variable
    "$OP=SgVariableDeclaration";

  // schroder3: TODO: There is also implicit address taking of functions (==> normalization)
  //  (e.g. "void func() {} int main() { void (*fp)() = func; /*fp contains address of func without using address of operator */}")

//  std::cout << std::endl << "Variable Id Mapping:" << std::endl;
//  this->vidm.toStream(std::cout);
//  std::cout << std::endl << "Function Id Mapping:" << std::endl;
//  this->fidm.toStream(std::cout);

  MatchResult& matches = collectSgAddressOfOp(matchquery, root);
  for(MatchResult::iterator it = matches.begin(); it != matches.end(); ++it) {
    SgNode* matchedOperand = (*it)["$OP"];
    // SgNode* head = (*it)["$HEAD"];
    // debugPrint(head); debugPrint(matchedOperand);
    OperandToVariableId optovid(*this);
    matchedOperand->accept(optovid);
  }              
}

// pretty print
void SPRAY::ComputeAddressTakenInfo::printAddressTakenInfo()
{
  std::cout << "addressTakenSet: [" << (variableAddressTakenInfo.first? "true, " : "false, ")
            << VariableIdSetPrettyPrint::str(variableAddressTakenInfo.second, vidm) << "]\n";
  std::cout << "functionAddressTakenSet: ["
            << fidm.getFunctionIdSetAsString(functionAddressTakenInfo.second) << "]\n";
}

/*************************************************
 **************** TypeAnalysis *******************
 *************************************************/

VariableIdSet SPRAY::CollectTypeInfo::getPointerTypeSet()
{
  return pointerTypeSet;
}

VariableIdSet SPRAY::CollectTypeInfo::getArrayTypeSet()
{
  return arrayTypeSet;
}

VariableIdSet SPRAY::CollectTypeInfo::getReferenceTypeSet()
{
  return referenceTypeSet;
}


void SPRAY::CollectTypeInfo::collectTypes()
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

void SPRAY::CollectTypeInfo::printPointerTypeSet()
{
  std::cout << "pointerTypeSet: " << VariableIdSetPrettyPrint::str(pointerTypeSet, vidm) << "\n";
}

void SPRAY::CollectTypeInfo::printArrayTypeSet()
{
  std::cout << "arrayTypeSet: " << VariableIdSetPrettyPrint::str(arrayTypeSet, vidm) << "\n";
}

void SPRAY::CollectTypeInfo::printReferenceTypeSet()
{
  std::cout << "referenceTypeSet: " << VariableIdSetPrettyPrint::str(referenceTypeSet, vidm) << "\n";
}

/*************************************************
 ********** FlowInsensitivePointerInfo  **********
 *************************************************/

void SPRAY::FlowInsensitivePointerInfo::collectInfo()
{
  compAddrTakenInfo.computeAddressTakenInfo(root);
  collTypeInfo.collectTypes();
}

void SPRAY::FlowInsensitivePointerInfo::printInfoSets()
{
  compAddrTakenInfo.printAddressTakenInfo();
  collTypeInfo.printPointerTypeSet();
  collTypeInfo.printArrayTypeSet();
  collTypeInfo.printReferenceTypeSet();
}

VariableIdMapping& SPRAY::FlowInsensitivePointerInfo::getVariableIdMapping()
{
  return vidm;
}

VariableIdSet SPRAY::FlowInsensitivePointerInfo::getMemModByPointer()
{
  return getAddressTakenVariables();
}

VariableIdSet SPRAY::FlowInsensitivePointerInfo::getAddressTakenVariables() {
  VariableIdSet unionSet;
  VariableIdSet addrTakenSet = (compAddrTakenInfo.getVariableAddressTakenInfo()).second;
  VariableIdSet arrayTypeSet = collTypeInfo.getArrayTypeSet();
  
  // std::set_union(addrTakenSet.begin(), addrTakenSet.end(),
  //                       arrayTypeSet.begin(), arrayTypeSet.end(),
  //                       unionSet.begin());

  // we can perhaps cache this for efficiency
  // to answer queries for multiple dereferencing queries
  SPRAY::set_union(addrTakenSet, arrayTypeSet, unionSet);

  return unionSet;
}

FunctionIdSet SPRAY::FlowInsensitivePointerInfo::getAddressTakenFunctions() {
  return compAddrTakenInfo.getFunctionAddressTakenInfo().second;
}
