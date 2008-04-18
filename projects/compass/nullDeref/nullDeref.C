// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Null Deref Analysis
// Author: Thomas Panas
// Date: 24-August-2007

#include "compass.h"
#include "nullDeref.h"

using namespace std;

using namespace boost;
using namespace BOOSTGraphInterface;

bool CompassAnalyses::NullDeref::Traversal::debug;
bool CompassAnalyses::NullDeref::Traversal::addressOp;

namespace CompassAnalyses
{ 
  namespace NullDeref
  { 
    
    static std::map<tps_node, tps_node> pred_map_data;
    static std::map<tps_node, int> color_map_data;
    static string functionName="";
    static string fileName="";

    const std::string checkerName      = "NullDeref";

    // Descriptions should not include the newline character "\n".
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
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["NullDeref.YourParameter"]);


}


string CompassAnalyses::NullDeref::Traversal::printTrace(const vector<tps_node>& trace) {
  string trac = "";
  SgStatement* lastStmt = 0;
  for (unsigned int i = 0; i < trace.size(); ++i) {
    tps_node n = trace[i];
    if (n.first.inEdges().size() == 1) {
      VirtualCFG::EdgeConditionKind eck = n.first.inEdges()[0].condition();
      char *conditionNames[] = {"unconditional", "true", "false", "case label", "default"};
      if (eck != VirtualCFG::eckUnconditional) {
        trac += "+++ When condition is " + ToString(conditionNames[eck]) + 
          (eck == VirtualCFG::eckCaseLabel ? n.first.inEdges()[0].caseLabel()->unparseToString() + 
           " from " + n.first.inEdges()[0].conditionBasedOn()->unparseToString() : "") + "\n";
      }
    }
    if (!n.first.isInteresting() && i != trace.size() - 1 && n.first.outEdges().size() == 1)
      continue;
    SgStatement* thisStmt = getStatement(n.first.getNode());
    if (!isSgBasicBlock(thisStmt->get_parent()))
      continue;
    if (thisStmt != lastStmt) {
      trac += ""+ thisStmt->get_file_info()->get_filenameString() + ":" + ToString(thisStmt->get_file_info()->get_line())
        + ":  " + trimIfNeeded(thisStmt->unparseToString()) +"\n";
      lastStmt = thisStmt;
    }
  }
  return trac;
}


std::string CompassAnalyses::NullDeref::Traversal::print_tps_error(string error, tps_node n, tps_node oldN) {
  //cout << "Found error: " << error << endl;
    //cout << "--- Trace:" << endl;
  vector<tps_node> trace;
  trace.push_back(n);
  n = oldN;
  for (; n != tps_node(); n = pred_map_data[n])
    trace.push_back(n);
  string retVal = printTrace(trace);
  //cout << "--- End" << endl;
  return retVal;
}

string CompassAnalyses::NullDeref::Traversal::trimIfNeeded(const string& s) {
  if (s.length() <= 44)
    return s;
  else
    return s.substr(0, 40) + " ...";
}

SgStatement* CompassAnalyses::NullDeref::Traversal::getStatement(SgNode* n) {
  while (n && !isSgStatement(n))
    n = n->get_parent();
  ROSE_ASSERT (n);
  return isSgStatement(n);
}



bool CompassAnalyses::NullDeref::Traversal::
switchForAssignment(std::vector<tps_node> &vec, tps_node n, tps_node oldN, SgExpression* rhs
                    ) const {
  if (debug) {
    std::cout << "       ------- switchForAssignment " << n << " " << (n.first.getNode() == n.second) << " " << oldN << " " << rhs->class_name() << " " << rhs->unparseToString() << std::endl;
  }
  switch (rhs->variantT()) {
  case V_SgVarRefExp:   {
    SgVarRefExp* varRefExpR = isSgVarRefExp(rhs);
    if (varRefExpR!=NULL) {
      SgVariableSymbol* varsymR = varRefExpR->get_symbol();
      SgInitializedName* initNameLocalR = varsymR->get_declaration();
      vec.push_back(tps_node(n.first, initNameLocalR));
    }
    break;
  }
  case V_SgAddressOfOp:
    addressOp=true;
    if (debug)
      std::cout << " @@*** V_SgAddressOfOp" << std::endl;
    return true;
    break;
  case V_SgCastExp:
    switchForAssignment(vec, n, oldN, isSgCastExp(rhs)->get_operand());
    break;
  case V_SgIntVal:
    if (debug)
      std::cout << " @@*** V_SgIntVal " << rhs->unparseToString() << std::endl;
    // found local variable
    if (isSgIntVal(rhs)->get_value() == 0) {
      string error = "Warning: Dereferencing known null pointer";
      string trace = print_tps_error(error, n, oldN);
      output->addOutput(new CheckerOutput(error+"\n"+trace, n.first.getNode()));
      //rb.addProperty("NULLDEREF", functionName, "1");
      //rb.addProperty("NULLDEREF", fileName, "1");
    }
    if (addressOp) {
      string error = "Warning: Pointer to stack variable";
      string trace = print_tps_error(error, n, oldN);
      output->addOutput(new CheckerOutput(error+"\n"+trace, n.first.getNode()));
      //rb.addProperty("NULLDEREF", functionName, "1");
      //rb.addProperty("NULLDEREF", fileName, "1");
    }
    break;
  case V_SgCharVal:
    if (debug)
      std::cout << " @@*** SgCharVal" << std::endl;
    if (isSgCharVal(rhs)->get_value() == 0) {
      string error = "Warning: Dereferencing known null pointer";
      string trace = print_tps_error(error, n, oldN);
      output->addOutput(new CheckerOutput(error+"\n"+trace, n.first.getNode()));
    }
    if (addressOp) {
      string error = "Warning: Pointer to stack variable";
      string trace = print_tps_error(error, n, oldN);
      output->addOutput(new CheckerOutput(error+"\n"+trace, n.first.getNode()));
    }
    break;
  case V_SgNewExp:
    if (debug)
      std::cout << " @@*** SgNewExp" << std::endl;
    break;
  case V_SgDotExp:
    if (debug)
      std::cout << " @@*** V_SgDotExp" << std::endl;
    break;
  case V_SgPntrArrRefExp:
    if (debug)
      std::cout << " @@*** SgPntrArrRefExp" << std::endl;
    break;
  case V_SgArrowExp:
    if (debug)
      std::cout << " @@*** SgArrowExp" << std::endl;
    break;
  case V_SgFunctionCallExp: {
    if (debug)
      std::cout << " @@*** SgFunctionCallExp" << std::endl;
    SgFunctionCallExp* fc = isSgFunctionCallExp(rhs);
    SgFunctionRefExp* refExp = isSgFunctionRefExp(fc->get_function());
    if (refExp) {
      ROSE_ASSERT(refExp->get_symbol());
      ROSE_ASSERT(refExp->get_symbol()->get_declaration());
      if ( refExp->get_symbol()->get_declaration()->get_name().str()==std::string("malloc")) {
        if (debug) std::cout << "  found malloc" << std::endl;
        string error = "Warning: Malloc may return NULL";
        string trace = print_tps_error(error, n, oldN);
        output->addOutput(new CheckerOutput(error+"\n"+trace, n.first.getNode()));
        
        //vec.clear();
      //  return vec;
      }
    }    
  }
    break;
  case V_SgAddOp:
    if (debug)
      std::cout << " @@*** SgAddOp" << std::endl;
    break;
  case V_SgDoubleVal:
    if (debug)
      std::cout << " @@*** V_SgDoubleVal" << std::endl;
    if (addressOp) {
      string error = "Warning: Pointer to stack variable";
      string trace = print_tps_error(error, n, oldN);
      output->addOutput(new CheckerOutput(error+"\n"+trace, n.first.getNode()));
      //rb.addProperty("NULLDEREF", functionName, "1");
      //rb.addProperty("NULLDEREF", fileName, "1");
    }
    break;
  default:
    cout << "           ____________________________ NOT HANDLED : " << rhs->class_name() << endl;
    // ROSE_ASSERT (!"Unhandled assignment RHS kind");
  }

  return false;
}

bool CompassAnalyses::NullDeref::Traversal::isNULL(SgExpression* rightSide) const {
  switch (rightSide->variantT()) {
  case V_SgCastExp : {
    return isNULL(isSgCastExp(rightSide)->get_operand());
  }
  case V_SgIntVal : {
    if (isSgIntVal(rightSide)->get_value()==0)
      return true;
    return false;
  }
	
	
  default:
    if (debug) cout << "NullDerefAnalysis::isNULL >>  unknown type : " << rightSide->class_name() <<
		 "  " << rightSide->unparseToString() << endl;
    return false;
  }
  return false;
}

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

std::vector <tps_node> CompassAnalyses::NullDeref::Traversal::tps_out_edges(tps_node node) const {
  VirtualCFG::CFGNode cfgnode = node.first;
  SgInitializedName* initName = node.second;
  std::vector<tps_node> vec;
  std::vector <VirtualCFG::CFGEdge> inEdges =  cfgnode.inEdges();
  for (unsigned int i = 0; i < inEdges.size(); ++i) {
    VirtualCFG::CFGNode source = inEdges[i].source();
    tps_node new_node_default = tps_node(source, initName);
    SgNode* sgNode = source.getNode();
    if (debug && inEdges[i].condition() != VirtualCFG::eckUnconditional) {
      char *conditionNames[] = {"unconditional", "true", "false", "case label", "default"};
      cout << "source = " << source.toString() << "  checking: " << initName->get_name().str()  << " " << sgNode->class_name() << endl;
      SgExpression* cbo = inEdges[i].conditionBasedOn();
      cout << "inEdges[i] cond " << conditionNames[inEdges[i].condition()] << " based on " << (cbo ? cbo->unparseToString() : "_") << endl;
      //       if (!source.isInteresting())
      //           goto default_case;
    }
    switch (inEdges[i].condition()) {
    case VirtualCFG::eckTrue:
      if (isLegitimateNullPointerCheck(inEdges[i].conditionBasedOn(), node.second, false))
	return std::vector<tps_node>();
      else
	goto default_case;
    case VirtualCFG::eckFalse:
      if (isLegitimateNullPointerCheck(inEdges[i].conditionBasedOn(), node.second, true))
	return std::vector<tps_node>();
      else
	goto default_case;
    case VirtualCFG::eckCaseLabel: goto default_case; // Not handled
    case VirtualCFG::eckDefault: goto default_case; // Not handled
    case VirtualCFG::eckUnconditional:
      break;
    default: ;
      // ROSE_ASSERT (!"Bad condition");
    }
    if (inEdges[i].condition() != VirtualCFG::eckUnconditional) goto default_case; // Normal loop processing does not apply
    if (isSgAssignOp(sgNode)) { //goto default_case;
      if (debug)
	std::cout << " *** SgAssignOp: " << sgNode->unparseToString() << std::endl;
      //check left hand side var against the delete variable
      SgAssignOp* assign = isSgAssignOp(sgNode);
      SgExpression* expr = assign->get_lhs_operand();
      if (debug)
	std::cout << "    *** expr: " << expr->unparseToString() << " " << expr->class_name() << std::endl;
      while (isSgPointerDerefExp(expr)) {
	expr = isSgPointerDerefExp(expr)->get_operand();
      }

      SgVarRefExp* varRefExp = isSgVarRefExp(expr);
      if (debug)
	std::cout << "    *** varRefExp: " << varRefExp->unparseToString() << std::endl;
      if (varRefExp!=NULL) {
	SgVariableSymbol* varsym = varRefExp->get_symbol();
	SgInitializedName* initNameLocal = varsym->get_declaration();
	if (debug)
	  std::cout << "    *** initNameLocal: " << initNameLocal->get_name().str() << std::endl;
	if (initNameLocal != initName)
	  goto default_case;
	if (debug)
	  std::cout << "    *** isSgAssignOp switchForAssignment: " << initName->get_name().str()  << std::endl;
	SgExpression* rhs = assign->get_rhs_operand();
	bool continueIt = switchForAssignment(vec,new_node_default, node, rhs);
	if (continueIt)
	  goto default_case;
	else
	  goto done;
      }
    } 

    else if (isSgInitializedName(sgNode)) {
      if (sgNode!=initName)
	goto default_case;
      if (debug)
	std::cout << " *** isSgInitializedName: " << isSgInitializedName(sgNode)->get_name().str()  << std::endl;
      //        if (isSgFunctionParameterList(isSgInitializedName(sgNode)->get_parent())) {
      // skip function parameters
      //          goto done;
      //        }
      SgInitializer* sgInit = isSgInitializedName(sgNode)->get_initializer();
      if (sgInit==NULL) {
        string error = "uninitialized variable (NULL)";
        string trace = print_tps_error(error, new_node_default, node);
        output->addOutput(new CheckerOutput(error+"\n"+trace, new_node_default.first.getNode()));

	//rb.addProperty("NULLDEREF", functionName, "1");
	//rb.addProperty("NULLDEREF", fileName, "1");
	goto done;
      }
      if (isSgAssignInitializer(sgInit)) {
	SgAssignInitializer* assignInit = isSgAssignInitializer(sgInit);
	SgExpression* expr = assignInit->get_operand();
	if (debug)
	  std::cout << "    *** isSgAssignInitializer switchForAssignment: " << initName->get_name().str()  << std::endl;
	bool continueIt = switchForAssignment(vec,new_node_default, node, expr);
	if (continueIt)
	  goto default_case;
	else
	  goto done;
      }
      goto done;
    } 

    else if (isSgFunctionCallExp(sgNode) && source.getIndex() == 2) {
      if (debug) std::cout << " *** isSgFunctionCallExp" << std::endl;
      SgFunctionCallExp* funcExp = isSgFunctionCallExp(sgNode);
      SgFunctionRefExp* refExp = isSgFunctionRefExp(funcExp->get_function());
      if (refExp && debug) std::cout << "  name: " << refExp->get_symbol()->get_declaration()->get_name().str() << std::endl;
      if (refExp && refExp->get_symbol()->get_declaration()->get_name().str()==std::string("__assert_fail")) {
	if (debug) std::cout << "  found __assert_fail" << std::endl;
	vec.clear();
	return vec;
      }

    }

  default_case:
  vec.push_back(new_node_default);
  done:
  ;
  }
  return vec;
}

void CompassAnalyses::NullDeref::Traversal::checkNullDeref(string analysisname, SgFunctionDeclaration* funcDecl, SgExpression* theExp, string name, int now, int max) {
  std::string lineNrDelete=ToString(theExp->get_file_info()->get_line());
  functionName = name;

  SgInitializedName* initName ;
  SgExpression* expr =  theExp;
  if (isSgFunctionCallExp(theExp)) {
    SgFunctionCallExp* f_expr = isSgFunctionCallExp(theExp);
    if (debug) 
      std::cout << " **** SgFunctionCallExp : " << //f_expr->unparseToString() << 
        " " << f_expr->class_name() << std::endl;
      SgFunctionRefExp* ff_ex = isSgFunctionRefExp(f_expr->get_function());
      //SgFunctionRefExp* refExp = isSgFunctionCallExp(theExp)->get_rhs_operand();
      if (debug) 
        std::cout << "     **** SgFunction (accept only free): " << ff_ex->unparseToString() << " " << ff_ex->class_name() << std::endl;
      if (ff_ex) { 
        if (ff_ex->unparseToString()=="free") {
          SgExprListExp* args = f_expr->get_args();
          SgExpressionPtrList& ptrList = args->get_expressions();
          SgExpressionPtrList::iterator it = ptrList.begin();
          for (; it!=ptrList.end(); ++it) {
            expr = *it;
            if (debug) 
              std::cout << "     **** free : " << expr->class_name() << std::endl;
            checkNullDeref(analysisname, funcDecl, expr, name, now, max);
          }
          return;
        }
      }
  }
  if (isSgArrowExp(theExp))
    expr = isSgArrowExp(theExp)->get_lhs_operand();
  if (isSgPointerDerefExp(theExp))
    expr = isSgPointerDerefExp(theExp)->get_operand();
  if (isSgAssignInitializer(theExp)) {
    SgAssignInitializer* assign = isSgAssignInitializer(theExp);
    SgExpression* exprAssign = assign->get_operand();
    if (assign!=NULL) {
      if (debug)
	std::cout << " **** SgAssignInitializer : " << //exprAssign->unparseToString() << 
          " " << exprAssign->class_name() << std::endl;

      if (isSgAddressOfOp(exprAssign)) {
	SgAddressOfOp* address = isSgAddressOfOp(exprAssign);
	expr = address->get_operand();
	if (debug)
	  std::cout << " **** isSgAddressOfOp : " << expr->unparseToString() << " " << expr->class_name() << std::endl;
	addressOp=true;
	//initName = isSgInitializedName(assign->get_parent());
	//        		  std::cout << " SgInitializedName* name = " << initName->get_name().str() << std::endl;
      }

      /*
        else {
        if (isSgCastExp(exprAssign)) {
          expr = (isSgCastExp(exprAssign))->get_operand();
          if (debug)
            std::cout << "     **** isSgCastExp : " <<  expr->class_name() << std::endl;
        }
        if (isSgFunctionCallExp(expr)) {
          SgFunctionRefExp* ff_ex = isSgFunctionRefExp(isSgFunctionCallExp(expr)->get_function());
          if (ff_ex->unparseToString()=="malloc") {
            if (debug)
              std::cout << "     **** isSgFunctionRefExp : " <<  ff_ex->class_name() << std::endl;
            
            
          }
        }
      }
      */

    }
  }
  if (isSgCastExp(theExp)) {
    expr = (isSgCastExp(theExp))->get_operand();
    if (debug)
      std::cout << " **** isSgCastExp : " <<  expr->class_name() << std::endl;
  }


  if (isSgVarRefExp(expr)) {
    SgVarRefExp* refExp = isSgVarRefExp(expr);
    SgVariableSymbol* sym =  refExp->get_symbol();
    std::string symbolName = sym->get_name().str();
    initName = sym->get_declaration();
    // variable name!
    std::string initNameStr = initName->get_name().str();
    std::string lineNr=ToString(initName->get_file_info()->get_line());
    std::string filename =initName->get_file_info()->get_filename();

    //      std::cout << std::endl;
    SgNode* sgexpr = theExp;
    while (!isSgExprStatement(sgexpr) && !isSgVariableDeclaration(sgexpr) && sgexpr!=NULL)
      sgexpr = sgexpr->get_parent();
    if (sgexpr==NULL || isSgAddressOfOp(sgexpr)) return;
    if (debug) {
      std::cout << "------- " << analysisname<<"---- found " << initNameStr << "  " << //sgexpr->unparseToString() <<
        "   Deref in :" << filename << "  :" <<lineNrDelete <<
	"  Declared in lineNr: " << lineNr << std::endl;
    }
    // move up in tree and find assignments to variable name
    // until the declaration is reached.

    VirtualCFG::CFGNode cfgNode = theExp->cfgForBeginning();
    tps_node node(cfgNode, initName);
    //				my_bfs vis(istCFGArrayDelete);

    // std::map<tps_node, tps_node> pred_map_data;
    pred_map_data.clear();
    predMapType pred_map = make_assoc_property_map(pred_map_data);
    // predecessor_recorder<predMapType, on_tree_edge> vis(pred_map);


    breadth_first_visit(tps_graph(this), node, color_map(colorMapType(color_map_data)).visitor(make_bfs_visitor(record_predecessors(pred_map, on_tree_edge()))));

  }
}


void
CompassAnalyses::NullDeref::Traversal::
visit(SgNode* sgNode)
{ 
  // Implement your traversal here.  
  //  int max=200; // amount of nodes?



  std::string name = sgNode->class_name();
  if (isSgFunctionDeclaration(sgNode)) {
    counter++;
    name = isSgFunctionDeclaration(sgNode)->get_name();
    //std::cout << "\n "<< ToString(counter) << "/" << ToString(max) << "  Checking function: " << name <<
    //  " ---------------------------------------- " << std::endl;

    fileName = isSgFunctionDeclaration(sgNode)->get_file_info()->get_filenameString();
    //fileName = getFileName(fileName);
    // query for delete expressions
    //  SgArrowExp, SgArrowStarOp
    std::vector<SgNode*> exprList = NodeQuery:: querySubTree (sgNode, V_SgArrowExp);
    for (std::vector<SgNode*>::iterator it = exprList.begin(); it!=exprList.end(); ++it) {
      SgNode* expr = *it;
      addressOp=false;
      SgArrowExp* delExpr = isSgArrowExp(expr);
      if (delExpr!=NULL) {
        checkNullDeref("SgArrowExp",isSgFunctionDeclaration(sgNode), delExpr, name, counter, max);
      }
    }
    exprList = NodeQuery:: querySubTree (sgNode, V_SgPointerDerefExp);
    for (std::vector<SgNode*>::iterator it = exprList.begin(); it!=exprList.end(); ++it) {
      SgNode* expr = *it;
      addressOp=false;
      SgPointerDerefExp* delExpr = isSgPointerDerefExp(expr);
      if (delExpr!=NULL) {
        checkNullDeref("SgPointerDerefExp", isSgFunctionDeclaration(sgNode), delExpr, name, counter, max);
      }
    }

    exprList = NodeQuery:: querySubTree (sgNode, V_SgAssignInitializer);
    for (std::vector<SgNode*>::iterator it = exprList.begin(); it!=exprList.end(); ++it) {
      SgNode* expr = *it;
      addressOp=false;
      SgAssignInitializer* delExpr = isSgAssignInitializer(expr);
      if (delExpr!=NULL) {
        checkNullDeref("SgAssignInitializer", isSgFunctionDeclaration(sgNode), delExpr, name, counter, max);
      }
    }

    exprList = NodeQuery:: querySubTree (sgNode, V_SgFunctionCallExp );
    for (std::vector<SgNode*>::iterator it = exprList.begin(); it!=exprList.end(); ++it) {
      SgNode* expr = *it;
      addressOp=false;
      SgFunctionCallExp* delExpr = isSgFunctionCallExp(expr);
      if (delExpr!=NULL) {
        checkNullDeref("SgFunctionCallExpr", isSgFunctionDeclaration(sgNode), delExpr, name, counter, max);
      }
    }


  }


} //End of the visit function.
   
