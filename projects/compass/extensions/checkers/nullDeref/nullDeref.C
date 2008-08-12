// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Null Deref Analysis
// Author: tps
// Date: 24-August-2007
// Altered in July 2008 by tps and JW to use defuse instead of boost

#include "compass.h"
#include "nullDeref.h"

using namespace std;

using namespace boost;
using namespace BOOSTGraphInterface;

bool CompassAnalyses::NullDeref::Traversal::debug;

namespace CompassAnalyses
{ 
  namespace NullDeref
  { 
    const std::string checkerName      = "NullDeref";
    const std::string shortDescription = "Looks for Null Pointer Dereferences";
    const std::string longDescription  = "This analysis looks for common possible null pointer dereferences. It is based on BOOST.";
  } //End of namespace NullDeref.
} //End of namespace CompassAnalyses.

CompassAnalyses::NullDeref::
CheckerOutput::CheckerOutput ( string problem, SgNode* node )
  : OutputViolationBase(node,checkerName,problem)
{}

CompassAnalyses::NullDeref::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
  : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
{
}


#if 0
// This part needs to stay in case that the DEFUSE Analysis can handle ASSERTIONS, e.g. assert (m!=NULL);
bool CompassAnalyses::NullDeref::Traversal::isLegitimateNullPointerCheck(SgExpression* expr, SgInitializedName* pointerVar, bool invertCheck) const {
  if (debug) {
    std::cout << "Null pointer test: " << (invertCheck ? "negative" : "positive") << " " << expr->unparseToString() << " pointer: " << pointerVar->get_name().str() << std::endl;
    std::cout << "class_name (expr) : " << expr->class_name() << endl;
  }
  switch (expr->variantT()) {
  case V_SgCastExp : {
    SgVarRefExp* varRef = isSgVarRefExp(isSgCastExp(expr)->get_operand());
    if (varRef==NULL)
      return false;
    if (invertCheck==true)
      return false;
    if (varRef->get_symbol()->get_declaration()==pointerVar)
      return true;
    return false;
  }
  case V_SgNotOp : {
    return isLegitimateNullPointerCheck(isSgNotOp(expr)->get_operand(), pointerVar, !invertCheck);
  }
  case V_SgNotEqualOp : {
    SgVarRefExp* varRef = isSgVarRefExp(isSgNotEqualOp(expr)->get_lhs_operand());
    SgExpression* rightSide = (isSgNotEqualOp(expr)->get_rhs_operand());
    if (varRef==NULL)
      return false;
    if (invertCheck==true)
      return false;
    if (varRef->get_symbol()->get_declaration()!=pointerVar)
      return false;
    if (!isNULL(rightSide))
      return false;
    return true;
  }
  case V_SgEqualityOp : {
    SgVarRefExp* varRef = isSgVarRefExp(isSgEqualityOp(expr)->get_lhs_operand());
    SgExpression* rightSide = (isSgEqualityOp(expr)->get_rhs_operand());
    if (varRef==NULL)
      return false;
    if (invertCheck==false)
      return false;
    if (varRef->get_symbol()->get_declaration()!=pointerVar)
      return false;
    if (!isNULL(rightSide))
      return false;
    return true;
  }
      
  default:
    return false;
  }

  return false;
}
#endif

/*---------------------------------------------------------------
 * This code checks whether an expression is NULL
 * the code is recursive
 ****************************************************************/
CompassAnalyses::NullDeref::Traversal::BoolWithTrace 
CompassAnalyses::NullDeref::Traversal::expressionIsNull(SgExpression* expr) {
  if (traces.find(expr) != traces.end()) {
    return traces[expr];
  }
  // cerr << "Working on " << expr->unparseToString() << endl;
  traces[expr].first = false;
  CompassAnalyses::NullDeref::Traversal::BoolWithTrace result(false, vector<SgExpression*>());
  switch (expr->variantT()) {
  case V_SgVarRefExp: {
    SgVarRefExp* vr = isSgVarRefExp(expr);
    SgInitializedName* var = vr->get_symbol()->get_declaration();
    ROSE_ASSERT (var);
    vector<SgNode*> defs = Compass::defuse->getDefFor(vr, var);
    // cerr << "Have " << defs.size() << " def(s)" << endl;
    for (size_t i = 0; i < defs.size(); ++i) {
      SgExpression* def = isSgExpression(defs[i]);
      if (!def) {
        // cerr << "Def is a " << defs[i]->class_name() << endl;
        result.first = true;
        break;
      }
      BoolWithTrace tr = expressionIsNull(def);
      if (tr.first) {
        result = tr;
        break;
      }
    }
    break;
  }
  case V_SgAddOp: {
    SgExpression* lhs = isSgAddOp(expr)->get_lhs_operand();
    //SgExpression* rhs = isSgAddOp(expr)->get_rhs_operand();
    // Assumes lhs is a pointer
    result = expressionIsNull(lhs);
    break;
  }
  case V_SgCastExp: {
    SgExpression* op = isSgUnaryOp(expr)->get_operand();
    result = expressionIsNull(op);
    break;
  }
  case V_SgAssignInitializer: {
    SgExpression* op = isSgAssignInitializer(expr)->get_operand();
    result = expressionIsNull(op);
    break;
  }
  case V_SgFunctionCallExp: {
    SgFunctionCallExp* fc = isSgFunctionCallExp(expr);
    SgFunctionRefExp* fr = isSgFunctionRefExp(fc->get_function());
    if (!fr) {
      result.first = true;
    } else {
      string name = fr->get_symbol()->get_declaration()->get_name().getString();
      if (name == "malloc") {
        result.first = true; // Check the result of malloc
      } else if (name == "xmalloc") {
        result.first = false; // For testing
      } else {
        result.first = true;
      }
    }
    break;
  }
  case V_SgNewExp: {
    result.first = false;
    break;
  }
  case V_SgThisExp: {
    result.first = false;
    break;
  }
  case V_SgAddressOfOp: {
    result.first = false; // FIXME
    break;
  }
  case V_SgIntVal: {
    result.first = (isSgIntVal(expr)->get_value() == 0);
    break;
  }
  default: {
    cout << "Unhandled expression kind " << expr->class_name() << endl;
    result.first = true;
    break;
  }
  }
  if (result.first) {
    result.second.push_back(expr);
  }
  traces[expr] = result;
  return result;
}


/*********************************************************
 *  Check an expression for being NULL
 *  and print the results (locally and to output)
 *********************************************************/
void CompassAnalyses::NullDeref::Traversal::
checkNullDeref(string analysisname, SgExpression* expr,  string name) {
  // get the line number of the expression

#if ROSE_GCC_OMP
#pragma omp critical (runDefUseAnalysisCompass)
#endif
  { 
    std::string lineNrDelete=ToString(expr->get_file_info()->get_line());
    BoolWithTrace tr;
    tr = expressionIsNull(expr);

    if (tr.first) {
      if (debug) 
        cout << "Expression " << expr->unparseToString() << " at line " << 
          expr->get_startOfConstruct()->get_line() << "  in file : " << expr->get_file_info()->get_filename() << " " ;
      string trace = "could be NULL: stack is:\n";
      for (size_t i = tr.second.size(); i > 0; --i) {
        string classname = (tr.second[i - 1]->class_name());
        string unparsed = (tr.second[i - 1]->unparseToString());
        int line = (tr.second[i - 1]->get_startOfConstruct()->get_line());
        trace.append(ToString(i)); trace.append(": "); trace.append(classname);
        trace.append(" "); trace.append(unparsed); trace.append(" (line ");
        trace.append(ToString(line)); trace.append(")\n");
      }
      trace += "End of stack\n";
      if (debug) 
        cout << trace ;
      output->addOutput(new CheckerOutput(trace ,expr));
    } else {
      //cout << "can not be NULL *************************************************************\n";
    }
  }
}


/*********************************************************
 *  main entry function. Only certain nodes are of interest 
 *  to this analysis.
 *********************************************************/
void
CompassAnalyses::NullDeref::Traversal::
visit(SgNode* sgNode)
{ 

  std::string name = sgNode->class_name();
  if (isSgArrowExp(sgNode)) {
    SgArrowExp* delExpr = isSgArrowExp(sgNode);
    if (delExpr!=NULL) {
      if (debug) 
        cerr << "\n >>>  Found SgArrowExp : " << delExpr->unparseToString() << endl;
      checkNullDeref("SgArrowExp", delExpr->get_lhs_operand(), name);
    }
  } 
  else if (isSgPointerDerefExp(sgNode)) {
    SgPointerDerefExp* delExpr = isSgPointerDerefExp(sgNode);
    if (delExpr!=NULL) {
      if (debug) 
        cerr << "\n >>>  Found SgPointerDerefExp : " << delExpr->unparseToString() << endl;
      checkNullDeref("SgPointerDerefExp",  delExpr->get_operand(), name);
    }
  }
  else if (isSgPntrArrRefExp(sgNode)) {
    SgPntrArrRefExp* delExpr = isSgPntrArrRefExp(sgNode);
    if (delExpr!=NULL) {
      if (debug) 
        cerr << "\n >>>  Found SgPntrArrRefExp : " << delExpr->unparseToString() << endl;
      checkNullDeref("SgPntrArrRefExp",  delExpr->get_lhs_operand(), name);
    }
  }
  else if (isSgFunctionCallExp(sgNode)) {
    SgFunctionCallExp* delExpr = isSgFunctionCallExp(sgNode);
    const SgExpressionPtrList& args = delExpr->get_args()->get_expressions();
    if (delExpr!=NULL) {
      if (args.size() >= 1) {
        SgFunctionRefExp* fr = isSgFunctionRefExp(delExpr->get_function());
        if (fr) {
          string name = fr->get_symbol()->get_declaration()->get_name().getString();
          if (name == "free") {
            if (debug) 
              cerr << "\n >>>  Found SgFunctionCallExpr : " << delExpr->unparseToString() << endl;
            checkNullDeref("SgFunctionCallExpr",  args[0], name);
          }
        }
      }
    }
  }
  
} //End of the visit function.
   
