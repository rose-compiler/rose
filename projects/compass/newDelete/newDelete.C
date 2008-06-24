// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// New Delete Analysis
// Author: Thomas Panas
// Date: 24-August-2007

#include "compass.h"
#include "newDelete.h"

using namespace std;

using namespace boost;
using namespace BOOSTGraphInterface;

namespace CompassAnalyses
{ 
  namespace NewDelete
  { 


    static std::map<tps_node, tps_node> pred_map_data;
    static std::map<tps_node, int> color_map_data;
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
  : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
{
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["NewDelete.YourParameter"]);


}


string CompassAnalyses::NewDelete::Traversal::printTrace(const vector<tps_node>& trace) {
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


std::string CompassAnalyses::NewDelete::Traversal::print_tps_error(string error, tps_node n, tps_node oldN) {
  //  cout << "Found error: " << error << endl;
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



string CompassAnalyses::NewDelete::Traversal::trimIfNeeded(const string& s) {
  if (s.length() <= 44)
    return s;
  else
    return s.substr(0, 40) + " ...";
}

SgStatement* CompassAnalyses::NewDelete::Traversal::getStatement(SgNode* n) {
  while (n && !isSgStatement(n))
    n = n->get_parent();
  ROSE_ASSERT (n);
  return isSgStatement(n);
}



void CompassAnalyses::NewDelete::Traversal::
switchForAssignment(std::vector<tps_node> &vec, 
                    tps_node n, 
                    tps_node oldN, 
                    SgExpression* rhs
                    ) const {

  switch (rhs->variantT()) {
  case V_SgNewExp: {
    //           cout << "Got new" << endl;
    SgNewExp* newExp = isSgNewExp(rhs);
    if (newExp!=NULL) {
      SgType* type = newExp->get_type();
      SgPointerType* ptype = isSgPointerType(type);
      if (ptype!=NULL) {
	type = ptype->get_base_type();
      }
      if (isSgArrayType(type)) {
	if (isCFGArrayDelete) {
	  //std::cout << "    deleting array type correctly."  <<std::endl;
	} else {
          string error = "Trying to delete array as pointer ";
	  string trace = print_tps_error(error, n, oldN);
          output->addOutput(new CheckerOutput(error+"\n"+trace ,newExp));
	}
      }
    }
    break;
  }
  case V_SgVarRefExp:   {
    SgVarRefExp* varRefExpR = isSgVarRefExp(rhs);
    if (varRefExpR!=NULL) {
      SgVariableSymbol* varsymR = varRefExpR->get_symbol();
      SgInitializedName* initNameLocalR = varsymR->get_declaration();
      vec.push_back(tps_node(n.first, initNameLocalR));
    }
    break;
  }
  case V_SgAddressOfOp: {
    string error = "Trying to delete non-heap variable ";
    string trace = print_tps_error(error, n, oldN);
    output->addOutput(new CheckerOutput(error+"\n"+trace ,n.first.getNode()));
    break;
  }
  case V_SgCastExp:
    switchForAssignment(vec, n, oldN, isSgCastExp(rhs)->get_operand());
    break;
  case V_SgIntVal: {
    string error = "Trying to delete NULL pointer ";
    string trace = print_tps_error(error, n, oldN);
    output->addOutput(new CheckerOutput(error+"\n"+trace ,n.first.getNode()));
    break;
  }
  default:
    cout << "Unhandled assignment RHS kind : " << rhs->class_name() << endl;
    // ROSE_ASSERT (!"Unhandled assignment RHS kind");
  }

}


std::vector <tps_node> CompassAnalyses::NewDelete::Traversal::
tps_out_edges(tps_node node) const{

  VirtualCFG::CFGNode cfgnode = node.first;
  SgInitializedName* initName = node.second;
  std::vector<tps_node> vec;
  std::vector <VirtualCFG::CFGEdge> inEdges =  cfgnode.inEdges();

  //cerr << " node ::: " << cfgnode.toString() << "   " << inEdges.size() << endl;

  for (unsigned int i = 0; i < inEdges.size(); ++i) {
    VirtualCFG::CFGNode source = inEdges[i].source();
    tps_node new_node_default = tps_node(source, initName);
    //cout << "source = " << source.toString() << "  interesting : " << source.isInteresting() << endl;
    SgNode* sgNode = source.getNode();

    

    if (!source.isInteresting())
      goto default_case;
    if (isSgAssignOp(sgNode)) { //goto default_case;
      //        std::cout << " *** SgAssignOp: " << sgNode->unparseToString() << std::endl;
      //check left hand side var against the delete variable
      SgAssignOp* assign = isSgAssignOp(sgNode);
      SgExpression* expr = assign->get_lhs_operand();
      SgVarRefExp* varRefExp = isSgVarRefExp(expr);
      if (varRefExp!=NULL) {
	SgVariableSymbol* varsym = varRefExp->get_symbol();
	SgInitializedName* initNameLocal = varsym->get_declaration();
	if (initNameLocal != initName)
	  goto default_case;
	SgExpression* rhs = assign->get_rhs_operand();
	switchForAssignment(vec,new_node_default, node, rhs);
	goto done;
      }
    } else if (isSgInitializedName(sgNode)) {
      if (sgNode!=initName)
	goto default_case;
      //     std::cout << " *** isSgInitializedName: " << sgNode->unparseToString() << std::endl;
      SgInitializer* sgInit = isSgInitializedName(sgNode)->get_initializer();
      if (sgInit==NULL) {
        string error = "Trying to delete uninitialized pointer ";
        string trace = print_tps_error(error, new_node_default, node);
        output->addOutput(new CheckerOutput(error+"\n"+trace , new_node_default.first.getNode()));

	goto done;
      }
      SgAssignInitializer* assignInit = isSgAssignInitializer(sgInit);
      SgExpression* expr = assignInit->get_operand();
      switchForAssignment(vec,new_node_default, node, expr);
      goto done;
    }

  default_case:


  vec.push_back(new_node_default);
  done:
  ;
  }
  return vec;
}





void CompassAnalyses::NewDelete::Traversal::checkNewDelForFunction(SgDeleteExp* delExp, string name) {
  std::string lineNrDelete=ToString(delExp->get_file_info()->get_line());

  isCFGArrayDelete = delExp->get_is_array();
  SgExpression* expr = delExp->get_variable();
  if (isSgVarRefExp(expr)) {
    SgVarRefExp* refExp = isSgVarRefExp(expr);
    SgVariableSymbol* sym =  refExp->get_symbol();
    std::string symbolName = sym->get_name().str();
    SgInitializedName* initName = sym->get_declaration();
    // variable name!
    std::string initNameStr = initName->get_name().str();
    std::string lineNr=ToString(initName->get_file_info()->get_line());
    //    std::string filename = getFileName(initName->get_file_info()->get_filename());
    std::string filename = initName->get_file_info()->get_filename();    
    
    //    std::cout << std::endl;
    //std::cout << "------------- found DeleteExp in :" << filename << ":" <<lineNrDelete <<
    //  "  Declared in lineNr: " << lineNr <<
    //  " array-Delete: " << (isCFGArrayDelete ? "True": "False") << std::endl;
    // move up in tree and find assignments to variable name
    // until the declaration is reached.

    VirtualCFG::CFGNode cfgNode = delExp->cfgForBeginning();
    tps_node node(cfgNode, initName);
    //				my_bfs vis(istCFGArrayDelete);

    // std::map<tps_node, tps_node> pred_map_data;
    pred_map_data.clear();
    predMapType pred_map = make_assoc_property_map(pred_map_data);
    // predecessor_recorder<predMapType, on_tree_edge> vis(pred_map);

#pragma omp critical (boost_visit)
    breadth_first_visit(tps_graph(this), node, color_map(colorMapType(color_map_data)).visitor(make_bfs_visitor(record_predecessors(pred_map, on_tree_edge()))));


  }
}


void
CompassAnalyses::NewDelete::Traversal::
visit(SgNode* sgNode)
{ 
#if 0
  if (isSgFunctionDeclaration(sgNode)) {
    string fileName = isSgFunctionDeclaration(sgNode)->get_file_info()->get_filenameString();
    string name = sgNode->class_name();
    //fileName = getFileName(fileName);
    // query for delete expressions
    std::vector<SgNode*> deleteExprList = NodeQuery:: querySubTree (sgNode, V_SgDeleteExp);
    for (std::vector<SgNode*>::iterator it = deleteExprList.begin(); it!=deleteExprList.end(); ++it) {
      SgNode* expr = *it;
      SgDeleteExp* delExpr = isSgDeleteExp(expr);
      if (delExpr!=NULL) {
        checkNewDelForFunction(isSgFunctionDeclaration(sgNode), delExpr, name);
      }
    }
  }
#endif 

  if (isSgDeleteExp(sgNode)) {
    std::string name = sgNode->class_name();
    SgDeleteExp* delExpr = isSgDeleteExp(sgNode);
    if (delExpr!=NULL) {
      checkNewDelForFunction(delExpr, name);
      }
  }


} //End of the visit function.
   
