// New Delete
// Author: Thomas Panas
// Date: 24-August-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_NEW_DELETE_H
#define COMPASS_NEW_DELETE_H

//#include "BoostGraphInterface.hxx"

namespace CompassAnalyses
{ 
  namespace NewDelete
    { 
      /*! \brief New Delete: Add your description here 
       */
      extern const std::string checkerName;
      extern const std::string shortDescription;
      extern const std::string longDescription;



      // Specification of Checker Output Implementation
      class CheckerOutput: public Compass::OutputViolationBase
	{ 
	public:
	  CheckerOutput(std::string problem, SgNode* node);
	};

      // Specification of Checker Traversal Implementation
      class Traversal 
	: public Compass::AstSimpleProcessingWithRunFunction
	{
	  // Checker specific parameters should be allocated here.
               Compass::OutputObject* output;

	  typedef std::pair<bool, std::vector<SgExpression*> > BoolWithTrace;
	  std::map<SgExpression*, BoolWithTrace> traces;
	  BoolWithTrace expressionIsNewExpr(SgExpression* expr);
	  void checkNewDelForFunction(SgDeleteExp* delExpr, std::string name);

	  template<typename T>    
	    static std::string ToString(T t){
	    std::ostringstream myStream; //creates an ostringstream object               
	    myStream << t << std::flush;       
	    return myStream.str(); //returns the string form of the stringstream object
	  }                        

	public:
	  Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);


	  // The implementation of the run function has to match the traversal being called.
	  void run(SgNode* n){  this->traverse(n, preorder); };

	  void visit(SgNode* n);
	};
    }
}

// COMPASS_NEW_DELETE_H
#endif
// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// New Delete Analysis
// Author: tps
// Date: 24-August-2007
// refactored July 2008

#include "compass.h"
// #include "newDelete.h"


using namespace std;


namespace CompassAnalyses
{ 
  namespace NewDelete
  { 
    static bool isCFGArrayDelete = false;
    static string functionName="";
    static string fileName="";

    const std::string checkerName      = "NewDelete";
    // Descriptions should not include the newline character "\n".
    const std::string shortDescription = "Check new-delete correlation.";
    const std::string longDescription  = "This analysis checks whether each delete statement is valid (no NULL pointer and array-delete on array create).\n";
  } //End of namespace NewDelete.
} //End of namespace CompassAnalyses.

CompassAnalyses::NewDelete::
CheckerOutput::CheckerOutput ( std::string problem, SgNode* node )
  : OutputViolationBase(node,checkerName,problem)
{}

CompassAnalyses::NewDelete::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
  : output(output)
{
}


/*---------------------------------------------------------------
 * This code checks whether an expression is NULL
 * the code is recursive
 ****************************************************************/
CompassAnalyses::NewDelete::Traversal::BoolWithTrace 
CompassAnalyses::NewDelete::Traversal::expressionIsNewExpr(SgExpression* expr) {
  if (traces.find(expr) != traces.end()) {
    return traces[expr];
  }
  // cerr << "Working on " << expr->unparseToString() << endl;
  traces[expr].first = false;
  BoolWithTrace result(false, vector<SgExpression*>());
  switch (expr->variantT()) {
  case V_SgVarRefExp: {
    SgVarRefExp* vr = isSgVarRefExp(expr);
    SgInitializedName* var = vr->get_symbol()->get_declaration();
    ROSE_ASSERT (var);
    vector<SgNode*> defs = Compass::sourceDefUsePrerequisite.getSourceDefUse()->getDefFor(vr, var);
    // cerr << "Have " << defs.size() << " def(s)" << endl;
    for (size_t i = 0; i < defs.size(); ++i) {
      SgExpression* def = isSgExpression(defs[i]);
      if (!def) {
        // cerr << "Def is a " << defs[i]->class_name() << endl;
        result.first = true;
        break;
      }
      BoolWithTrace tr = expressionIsNewExpr(def);
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
    result = expressionIsNewExpr(lhs);
    break;
  }
  case V_SgCastExp: {
    SgExpression* op = isSgUnaryOp(expr)->get_operand();
    result = expressionIsNewExpr(op);
    break;
  }
  case V_SgAssignInitializer: {
    SgExpression* op = isSgAssignInitializer(expr)->get_operand();
    result = expressionIsNewExpr(op);
    break;
  }
  case V_SgFunctionCallExp: {
    SgFunctionCallExp* fc = isSgFunctionCallExp(expr);
    SgFunctionRefExp* fr = isSgFunctionRefExp(fc->get_function());
    if (!fr) {
      result.first = true;
    } else {
      string name = fr->get_symbol()->get_declaration()->get_name().getString();
      result.first = true;
    }
    break;
  }
  case V_SgNewExp: {
    SgNewExp* newExp = isSgNewExp(expr);
    SgType* type = newExp->get_type();
    SgPointerType* ptype = isSgPointerType(type);
    if (ptype!=NULL) {
      type = ptype->get_base_type();
    }
    if (isSgArrayType(type)) {
      if (isCFGArrayDelete) {
        //std::cout << "    deleting array type correctly."  <<std::endl;
        result.first = false;
      } else {
        result.first = true;
	}
    }
    break;
  }
  case V_SgThisExp: {
    result.first = false;
    break;
  }
  case V_SgAddressOfOp: {
    result.first = false;
    break;
  }
  case V_SgIntVal: {
    result.first = (isSgIntVal(expr)->get_value() == 0);
    break;
  }
  default: {
    cout << "Compass::newdelete::Unhandled expression kind " << expr->class_name() << endl;
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
void CompassAnalyses::NewDelete::Traversal::
checkNewDelForFunction(SgDeleteExp* expr, string name) {
#if ROSE_GCC_OMP
#pragma omp critical (runDefUseAnalysisCompass)
#endif
  { 
  std::string lineNrDelete=ToString(expr->get_file_info()->get_line());

  BoolWithTrace tr;

  ROSE_ASSERT(isSgDeleteExp(expr));
  isCFGArrayDelete = expr->get_is_array();
  SgExpression* exprV = expr->get_variable();
  if (isSgVarRefExp(exprV)) 
    tr = expressionIsNewExpr(exprV);

  if (tr.first) {

    //      cout << "Expression " << expr->unparseToString() << " at line " << 
    //    expr->get_startOfConstruct()->get_line() << "  in file : " << expr->get_file_info()->get_filename() << " " ;
    string trace = "delete is dangerous: stack is:\n";
    for (size_t i = tr.second.size(); i > 0; --i) {
      string classname = (tr.second[i - 1]->class_name());
      string unparsed = (tr.second[i - 1]->unparseToString());
      int line = (tr.second[i - 1]->get_startOfConstruct()->get_line());
      trace.append(ToString(i)); trace.append(": "); trace.append(classname);
        trace.append(" "); trace.append(unparsed); trace.append(" (line ");
        trace.append(ToString(line)); trace.append(")\n");
    }
    trace += "End of stack\n";
    cout << trace ;
    output->addOutput(new CheckerOutput(trace ,exprV));
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
CompassAnalyses::NewDelete::Traversal::
visit(SgNode* sgNode)
{ 
  if (isSgDeleteExp(sgNode)) {
    std::string name = sgNode->class_name();
    SgDeleteExp* delExpr = isSgDeleteExp(sgNode);
    if (delExpr!=NULL) {
      //      if (debug) 
      //  cerr << "\n >>>  Found SgDeleteExpr : " << delExpr->unparseToString() << endl;
      checkNewDelForFunction(delExpr, name);
    }
  }
} //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::NewDelete::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::NewDelete::Traversal(params, output);
}

static Compass::PrerequisiteList getPrerequisites() {
  Compass::PrerequisiteList defusePre;
  defusePre.push_back(&Compass::projectPrerequisite);
  defusePre.push_back(&Compass::sourceDefUsePrerequisite);
  return defusePre;
}

extern const Compass::Checker* const newDeleteChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::NewDelete::checkerName,
        CompassAnalyses::NewDelete::shortDescription,
        CompassAnalyses::NewDelete::longDescription,
        Compass::C | Compass::Cpp,
	getPrerequisites(),
        run,
        createTraversal);
