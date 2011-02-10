
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "rewrite.h"
#include <iostream>
#include <iomanip>
#include <list>

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (8/1/2005): test use of new static function to create 
// Sg_File_Info object that are marked as transformations
#undef SgNULL_FILE
#define SgNULL_FILE Sg_File_Info::generateDefaultFileInfoForTransformationNode()


bool containsNonConst(SgNode* n, SgExpression* lhs) {
  bool result = false;
  if (isSgAssignInitializer(n))
    n = isSgAssignInitializer(n)->get_operand();
  if (isSgAssignInitializer(lhs))
    lhs = isSgAssignInitializer(lhs)->get_operand();
  // cout << n->unparseToString() << " <--> " << lhs->unparseToString() << endl;
  // cout << "containsNonConst: n is a " << n->sage_class_name() << ", lhs is a " << lhs->sage_class_name() << endl;
  if (n == lhs) result = true;
  if (isSgVarRefExp(lhs) && isSgVarRefExp(n)) {
    // cout << "n@" << hex << (int)(isSgVarRefExp(n)->get_symbol()->get_declaration()) << " = " << isSgVarRefExp(n)->get_symbol()->get_name().str() << ", lhs@" << hex << (int)(isSgVarRefExp(lhs)->get_symbol()->get_declaration()) << " = " << isSgVarRefExp(lhs)->get_symbol()->get_name().str() << endl;
    if (isSgVarRefExp(lhs)->get_symbol()->get_declaration() == 
        isSgVarRefExp(n)->get_symbol()->get_declaration())
      result = true;
  }
  if (isSgDotExp(n) && 
      containsNonConst(isSgDotExp(n)->get_lhs_operand(), lhs))
    result = true;
  // cout << " ==> " << result << endl;
  return result;
}

bool isInitializerForReferenceVar(SgAssignInitializer* n, SgExpression* expr) {
  // cout << "In isInitializerForReferenceVar, n is " << n->unparseToString() << endl;
  string expr_str;
  if (isSgVarRefExp(expr)) {
    SgName n = isSgVarRefExp(expr)->get_symbol()->get_name();
    expr_str = n.str();
  } else {
    expr_str = expr->unparseToString();
  }
#if 0
  cout << "Expr is " << expr->sage_class_name() << ": " << expr_str << endl;
  printf ("n = %p = %s n->parent() = %p = %s \n",n,n->class_name().c_str(),n->get_parent(),n->get_parent()->class_name().c_str());
#endif
  SgInitializedName* in = isSgInitializedName(n->get_parent());
  if (!in) return false;
  if (!in->get_type())
    in = isSgInitializedName(in->get_parent());
  assert (in);
  assert (in->get_type());
  // cout << "Type is " << in->get_type()->unparseToString() << ", var is " << in->get_name().str() << endl;
  bool isNonconstRef = SageInterface::isNonconstReference(in->get_type());
  bool containsNonConstUse = containsNonConst(n, expr);
  bool result = isNonconstRef && containsNonConstUse;
  // cout << "isNonconstReference is " << isNonconstRef << ", containsNonConst is " << containsNonConstUse << endl;
  // cout << "Result is " << (result ? "true" : "false") << endl;
  return result;
}

class IsPotentiallyModifiedVisitor: public AstSimpleProcessing {
  SgExpression* expr;
  bool& result;

  public:
  IsPotentiallyModifiedVisitor(SgExpression* expr, bool& result):
    expr(expr), result(result) {}

  virtual void visit(SgNode* n) {
    // cout << "Checking for modifications to " << expr->unparseToString() << " in " << (isSgInitializedName(n) ? "initname" : n->unparseToString()) << ", result is " << result << endl;
    if (result) return;
    switch (n->variantT()) {
      case V_SgAndAssignOp:
      case V_SgAssignOp:
      case V_SgDivAssignOp:
      case V_SgIorAssignOp:
      case V_SgLshiftAssignOp:
      case V_SgMinusAssignOp:
      case V_SgModAssignOp:
      case V_SgMultAssignOp:
      case V_SgPlusAssignOp:
      case V_SgRshiftAssignOp:
      case V_SgXorAssignOp:
      case V_SgDotStarOp:
      case V_SgPntrArrRefExp: // FIXME should handle all references
      result |= containsNonConst(isSgBinaryOp(n)->get_lhs_operand(), expr);
      break;

      case V_SgAssignInitializer:
      result |= isInitializerForReferenceVar(isSgAssignInitializer(n), expr);
      break;

      case V_SgAddressOfOp:
      case V_SgMinusMinusOp:
      case V_SgPlusPlusOp:
      result |= containsNonConst(isSgUnaryOp(n)->get_operand(), expr);
      break;

      case V_SgFunctionCallExp:
      {
        SgFunctionCallExp* fc = isSgFunctionCallExp(n);
        SgExprListExp* args1 = fc->get_args();
        SgFunctionType* ft = isSgFunctionType(fc->get_function()->get_type());

// DQ (8/13/2004): Working with Jeremiah, we can now assert this 
// and we don't have to handle the cases where this is false!
        ROSE_ASSERT (ft != NULL);
        SgTypePtrList& params = ft->get_arguments();
        SgExpressionPtrList& args = args1->get_expressions();
        SgTypePtrList::iterator pi = params.begin();
        SgExpressionPtrList::iterator ai = args.begin();
        for (; ai != args.end(); ++ai, ++pi) {
          if (SageInterface::isNonconstReference(*pi))
            result |= containsNonConst(*ai, expr);
        }
        break;
      }

      case V_SgInitializedName:
      {
#if 0
        SgVarRefExp* vr = new SgVarRefExp(SgNULL_FILE, 
                            new SgVariableSymbol(isSgInitializedName(n)));
        result |= containsNonConst(expr, vr); // Note flipped arguments
        delete vr;
#endif
      }

      default: // Do nothing
      break;
    }
  }
};

// Is expression expr potentially modified (assigned to, address taken,
// passed by non-const reference) somewhere in the tree pointed to by
// root?
bool isPotentiallyModified(SgExpression* expr, SgNode* root) {
  bool result = false;
  // cout << "Checking for modifications to " << expr->unparseToString() << " in " << (isSgInitializedName(root) ? "initname" : root ? root->unparseToString() : "NULL") << endl;
  if (root)
    IsPotentiallyModifiedVisitor(expr, result).traverse(root, preorder);
  // cout << "Result is " << result << endl;
  return result;
}

class HasAddressTakenVisitor: public AstSimpleProcessing {
  SgExpression* expr;
  bool& result;

  public:
  HasAddressTakenVisitor(SgExpression* expr, bool& result):
    expr(expr), result(result) {}

  virtual void visit(SgNode* n) {
    switch (n->variantT()) {
      case V_SgAddressOfOp:
      result |= containsNonConst(isSgUnaryOp(n)->get_operand(), expr);
      break;

      case V_SgFunctionCallExp:
      result |= isPotentiallyModified(expr, n);
      break;

      case V_SgAssignInitializer:
      result |= isInitializerForReferenceVar(isSgAssignInitializer(n), expr);
      break;

      default: // Do nothing
      break;
    }
  }
};

// Does expression expr potentially have its address taken somewhere in the
// tree pointed to by root?
bool hasAddressTaken(SgExpression* expr, SgNode* root) {
  bool result = false;
  if (isSgPntrArrRefExp(expr) || isSgPointerDerefExp(expr))
    return true;
  HasAddressTakenVisitor(expr, result).traverse(root, preorder);
  return result;
}
