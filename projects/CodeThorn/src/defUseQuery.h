#ifndef _DEFUSEQUERY_H
#define _DEFUSEQUERY_H

/*************************************************************
 * Author   : Sriram Aananthakrishnan                        *
 *************************************************************/


#include "AstTerm.h"
#include "AstMatching.h"
#include "VariableIdMapping.h"
#include "Miscellaneous.h"
#include <set>
#include <utility>

#include "VariableIdUtils.h"

using namespace CodeThorn;
using namespace CodeThorn;

// VariableId is short hand for named memory
// locations

// basic type info to describe some type
// information about the variable
// type info is associated with the respective VariableId
enum VariableIdTypeInfo {
  variableType,
  arrayType,
  pointerType,
  referenceType,
  classType
};

// determines VariableIdTypeInfo given a VariableId and the VariableIdMapping
VariableIdTypeInfo getVariableIdTypeInfo(VariableId vid, VariableIdMapping& vidm);

// stringify the type info
std::string variableIdTypeInfoToString(VariableIdTypeInfo vidTypeInfo);

typedef std::pair<VariableId, VariableIdTypeInfo> VariableIdInfo;

// map with VariableId as the key
//
typedef std::map<VariableId, VariableIdTypeInfo> VariableIdInfoMap;

// VarsInfo is used to represent the result
// of def/use query on expressions. 
// VarsInfo is a pair <VariableIdInfoMap, bool>
// @VariableIdInfoMap: set of named memory locations 
// (variables) represented by their VariableId and their
// associated VariableIdTypeInfo
// @bool: flag is set if the expression modifies/uses
// memory locations that are different from 
// the named memory locations appearing on the
// expression itself (example: *p or array + 5)
// flag is an indicator for requiring more
// sophisticated pointer analysis to answer the
// def/use query on this expression
typedef std::pair<VariableIdInfoMap, bool> VarsInfo;

// it is expensive to answer def/use queries on demand
// for expressions that involve function calls
// as it would mean walking all the expressions
// in the function and the functions that it may call
// we simply collect all the function calls in the expression
// @FunctionCallExpSet: consitsts of all the functions
// that this expression is calling
typedef std::set<SgFunctionCallExp*> FunctionCallExpSet;

/*************************************************
 *************** DefUseVarsInfo ****************
 *************************************************/

// determined completely based on syntactic information
// def_vars_info consists of VariableIds which are written by the expression
// use_vars_info consists of VariableIds which are read but not modified by this expression
// func_set consists of all SgFunctionCallExps that are invoked by this expression
class DefUseVarsInfo
{
  VarsInfo def_vars_info;
  VarsInfo use_vars_info;
  FunctionCallExpSet func_set;
  // flag is set if the variables in def_vars_info are
  // used after their definition in the same expression
  // or in other words has side-effects in the expression
  // example: a = b + (c = 0) or a = i++
  bool useAfterDef;
   
public:
  DefUseVarsInfo() : useAfterDef(false) { }
DefUseVarsInfo(const VarsInfo& _def_info, const VarsInfo& _use_info, const FunctionCallExpSet& _fset, bool _useAfterDef)
  : def_vars_info(_def_info),
    use_vars_info(_use_info),
    func_set(_fset),
    useAfterDef(_useAfterDef) { }
    
  // returns the corresponding info about the memory locations
  VarsInfo getDefVarsInfo() {
    return def_vars_info;
  }
  VarsInfo getUseVarsInfo() {
    return use_vars_info;
  }
  VarsInfo& getDefVarsInfoMod() {
    return def_vars_info;
  }
  VarsInfo& getUseVarsInfoMod() {
    return use_vars_info;
  }
  const VarsInfo& getDefVarsInfoRef() const {
    return def_vars_info;
  }
  const VarsInfo& getUseVarsInfoRef() const {
    return use_vars_info;
  }
  FunctionCallExpSet getFunctionCallExpSet() {
    return func_set;
  }
  FunctionCallExpSet& getFunctionCallExpSetMod() {
    return func_set;
  }
  const FunctionCallExpSet& getFunctionCallExpSetRef() const {
    return func_set;
  }
  bool isDefSetEmpty() const {
    return def_vars_info.first.size() == 0;
  }
  bool isUseSetEmpty() const {
    return use_vars_info.first.size() == 0;
  }
  bool isFunctionCallExpSetEmpty() const {
    return func_set.size() == 0;
  }
  // returns the flag of VarsInfo
  bool isDefSetModByPointer() const {
    return def_vars_info.second;
  }
  bool isUseSetModByPointer() const {
    return use_vars_info.second;
  }
  bool getUseAfterDef() const {
    return useAfterDef;
  }
  // raise the flag to handle side-effects
  void setUseAfterDef() {
    useAfterDef = true;
  }
  // access method to know if the expr has side-effect
  bool isUseAfterDef() const {
    return useAfterDef;
  }
  // helpful to determine if the useAfterDef flag needs to be raised
  // called on object of expr which is not expected to have side-effetcs
  // function inspects all possible defs info of the object
  // should only be called on expression with non-modifying semantics
  bool isUseAfterDefCandidate() const {
    return (!isDefSetEmpty() ||
            isDefSetModByPointer() ||
            !isFunctionCallExpSetEmpty());
  }
  void copyUseToDef();

  friend DefUseVarsInfo operator+(const DefUseVarsInfo& duvi1, const DefUseVarsInfo& duvi2);

  static std::string varsInfoPrettyPrint(VarsInfo& vars_info, VariableIdMapping& vidm);
  static std::string functionCallExpSetPrettyPrint(FunctionCallExpSet& func_calls_info);

  // for more readability
  std::string str(VariableIdMapping& vidm);  

  // Add VariableIds corresponding to all elements of *array_name* to
  // *dev_vars_info* or *use_vars_info* depending on *def*.
  void addAllArrayElements(SgInitializedName* array_name, VariableIdMapping& vidm, bool def);
};

// used by the getDefUseVarsInfo_rec to traverse the 
// structure of the expression and call the appropriate
// getDefUseLHS/RHS functions
class ExprWalker : public ROSE_VisitorPatternDefaultBase
{
  VariableIdMapping& vidm;
  // if set then we are processing an expression on lhs of assignment
  bool isModExpr;
  DefUseVarsInfo duvi;

public:
  ExprWalker(VariableIdMapping& _vidm, bool isModExpr);
   
  // unary op with modifying semantics
  void visit(SgMinusMinusOp* sgn);
  void visit(SgPlusPlusOp* sgn);

  // unary op with non-modifying semantics
  void visit(SgCastExp* sgn);
  void visit(SgAddressOfOp* sgn); 
  void visit(SgMinusOp* sgn);
  void visit(SgUnaryAddOp* sgn);
  void visit(SgNotOp* sgn);
  void visit(SgBitComplementOp* sgn);
  void visit(SgThrowOp* sgn);

 
  // binary op with modifying semantics
  void visit(SgAssignOp* sgn);
  void visit(SgCompoundAssignOp* sgn);

  // binary op that can be lvalues
  void visit(SgCommaOpExp* sgn);
  void visit(SgPntrArrRefExp* sgn);
  void visit(SgPointerDerefExp* sgn);
  void visit(SgDotExp* sgn);
  void visit(SgArrowExp* sgn);

  // binary op that can only be rvalues
  void visit(SgAddOp* sgn);
  void visit(SgAndOp* sgn);
  void visit(SgArrowStarOp* sgn);
  void visit(SgBitAndOp* sgn);
  void visit(SgBitOrOp* sgn);
  void visit(SgBitXorOp* sgn);
  void visit(SgDivideOp* sgn);
  void visit(SgDotStarOp* sgn);
  void visit(SgEqualityOp* sgn);
  void visit(SgGreaterOrEqualOp* sgn);
  void visit(SgGreaterThanOp* sgn);
  void visit(SgLessOrEqualOp* sgn);
  void visit(SgLessThanOp* sgn);
  void visit(SgLshiftOp* sgn);
  void visit(SgModOp* sgn);
  void visit(SgMultiplyOp* sgn);
  void visit(SgNotEqualOp* sgn);
  void visit(SgOrOp* sgn);
  void visit(SgRshiftOp* sgn);
  void visit(SgSubtractOp* sgn);

  // expr that can be lvalues
  void visit(SgFunctionCallExp* sgn);
  void visit(SgConditionalExp* sgn);

  // expr that can only be rvalues
  void visit(SgExprListExp* sgn);
  void visit(SgSizeOfOp* sgn);
  void visit(SgDeleteExp* sgn);
  void visit(SgNewExp* sgn);
  void visit(SgTypeIdOp* sgn);
  void visit(SgVarArgOp* sgn);
  void visit(SgVarArgStartOp* sgn);
  void visit(SgVarArgStartOneOperandOp* sgn);
  void visit(SgVarArgEndOp* sgn);
  void visit(SgVarArgCopyOp* sgn);

  
  // different intializers
  void visit(SgAssignInitializer* sgn);
  void visit(SgConstructorInitializer* sgn);
  void visit(SgAggregateInitializer* sgn);
  void visit(SgCompoundInitializer* sgn);
  void visit(SgDesignatedInitializer* sgn);

  // basic cases for the recursive function
  // that represent variables
  void visit(SgVarRefExp* sgn);
  void visit(SgInitializedName* sgn);

  // no processing required
  void visit(SgValueExp* sgn);
  void visit(SgNullExpression* sgn);
  void visit(SgFunctionRefExp* sgn);
  void visit(SgMemberFunctionRefExp* sgn);
  void visit(SgThisExp* sgn);
  void visit(SgClassNameRefExp* sgn);
  void visit(SgLabelRefExp* sgn);
  void visit(SgTemplateFunctionRefExp* sgn);
  void visit(SgTemplateMemberFunctionRefExp* sgn);
  void visit(SgTypeTraitBuiltinOperator* sgn);
  void visit(SgPseudoDestructorRefExp* sgn);
  void visit(SgStatementExpression* sgn);
  void visit(SgAsmOp* sgn);
  
  void visit(SgExpression* sgn);

  // helper methods
  void visitSgUnaryOpNoMod(SgUnaryOp* sgn);
  void visitSgBinaryOpNoMod(SgBinaryOp* sgn);

  DefUseVarsInfo getDefUseVarsInfo();
};

// interface function to
// collect defined and used variables based on the semantics of the expressions
// flag is raised for pointer dereferencing as we dont know what is defined/used
// function calls in expressions are treated as black boxes and are simply collected
// main interface function
DefUseVarsInfo getDefUseVarsInfo(SgNode* sgn, VariableIdMapping& vidm);
DefUseVarsInfo getDefUseVarsInfoExpr(SgExpression* sgn, VariableIdMapping& vidm);
DefUseVarsInfo getDefUseVarsInfoInitializedName(SgInitializedName* sgn, VariableIdMapping& vidm);
DefUseVarsInfo getDefUseVarsInfoVariableDeclaration(SgVariableDeclaration* sgn, VariableIdMapping& vidm);

// internal implementation
// @sgn: root node
// @duvi: the defuse object that will be built
// @isModExpr: set to true if the expression is modifying a memory location
DefUseVarsInfo getDefUseVarsInfo_rec(SgNode* sgn, VariableIdMapping& vidm, bool isModExpr);

#endif

