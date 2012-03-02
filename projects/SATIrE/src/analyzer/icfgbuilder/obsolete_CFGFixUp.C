#include <iostream>
#include "CFGFixUp.h"

#define FILEINFO Sg_File_Info::generateDefaultFileInfoForTransformationNode()

class ExprCheckTraversal : public AstSimpleProcessing {
public:
  void visit(SgNode* node) {
    std::cout << "checking " << node;
    if(SgExpression* e=isSgExpression(node)) {
      if(isSgVarRefExp(e)) {
	std::cout << "SgVarRefExp found:";
	std::cout << e->unparseToString() << std::endl;
      }
      if(e->get_file_info()==0) {
	std::cout << "NODE:" << e->sage_class_name() << " adding file info." << std::endl;
	e->set_file_info(FILEINFO);
	if(e->get_endOfConstruct()==0) {
	  std::cout << "setting missing endOfConstruct" << std::endl;
	  e->set_endOfConstruct(FILEINFO);
	}
      }
    }
    if(SgStatement* e=isSgStatement(node)) {
      if(e->get_file_info()==0) {
	std::cout << "NODE:" << e->sage_class_name() << " adding file info." << std::endl;
	e->set_file_info(FILEINFO);
      }
    }
  }
  ExprCheckTraversal() {}
  virtual ~ExprCheckTraversal() {}
};
CFGCheck::CFGCheck() {
  displayResultsOn();
}
void CFGCheck::displayResultsOn() {
  _displayResults=true;
}
void CFGCheck::displayResultsOff() {
  _displayResults=false;
}
bool CFGCheck::displayResults() {
  return _displayResults;
}
void CFGCheck::print(std::string s) {
  if(displayResults()) {
    std::cout << s;
  }
}
void CFGCheck::printnl(std::string s) {
  if(displayResults()) {
    std::cout << s;
    printnl();
  }
}
void CFGCheck::printnl() {
  if(displayResults()) {
    std::cout << std::endl;
  }
}

void CFGCheck::checkExpressions(CFG* cfg) {
  assert(cfg);
  return;

  printnl("CHECKING CFG: START");
  std::deque<BasicBlock *>::const_iterator block;
  std::deque<SgStatement *>::const_iterator stmt;
  SgExpression* exp;
  for (block = cfg->nodes.begin(); block != cfg->nodes.end(); ++block) {
    for (stmt = (*block)->statements.begin();
	 stmt != (*block)->statements.end(); ++stmt) {
      assert(*stmt);
      ExprCheckTraversal est;
      if (isSgCaseOptionStmt(*stmt) 
	  || isSgExprStatement(*stmt)
	  || isSgScopeStatement(*stmt)) {
	print("Checking Statements Group:");
	
	est.traverse(*stmt, preorder);
	printnl();
      }
      else if (dynamic_cast<ArgumentAssignment *>(*stmt))
	{
	  print("Checking ArgumentAssignment:");
	  est.traverse(dynamic_cast<ArgumentAssignment *>(*stmt)->get_lhs(),
		       preorder);
	  est.traverse(dynamic_cast<ArgumentAssignment *>(*stmt)->get_rhs(),
		       preorder);
	  printnl();
	}
      else if (dynamic_cast<ReturnAssignment *>(*stmt))
	{
	  print( "Checking ReturnAssignment:");
	  est.traverse(dynamic_cast<ReturnAssignment *>(*stmt)->get_lhs(),
		       preorder);
	  printnl();
	}
      else if (dynamic_cast<ParamAssignment *>(*stmt)) {
	print( "Checking ParamAssignment:");
	est.traverse(dynamic_cast<ParamAssignment *>(*stmt)->get_lhs(),
		     preorder);
	printnl();
	}
      else if (dynamic_cast<LogicalIf *>(*stmt)) {
	print( "Checking LogicalIf:");
	est.traverse(dynamic_cast<LogicalIf *>(*stmt)->get_condition(),
		     preorder);
	printnl();
      }
    }
  }
  printnl("CHECKING CFG: END");

#if 0
  // check expressions (called by number_exprs() )
  std::cout << "CHECK EXPRESSIONS: START"<<std::endl;
  std::cout << expr_set->size() << " expressions" << std::endl;
  std::set<SgExpression *, ExprPtrComparator>::iterator en;
  for (en = expr_set->begin(); en != expr_set->end(); ++en) {
    std::cout << "'" << expr_to_string(*en) << "'" << std::endl;
  }
  std::cout << "CHECK EXPRESSIONS: DONE."<<std::endl;
#endif
  
}
