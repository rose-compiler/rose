// -*- C++ -*-
// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: StatementAttributeTraversal.h,v 1.6 2008-09-09 14:21:17 gergo Exp $

#ifndef STATEMENTATTRIBUTETRAVERSAL_H
#define STATEMENTATTRIBUTETRAVERSAL_H

// #include <config.h>
#include "CFGTraversal.h"
#include "ProcTraversal.h"
// GB (2008-03-03): Changed all unparseToString calls to Ir::fragmentToString.
// This function is declared in IrCreation.h.
#include "IrCreation.h"

template<typename DFI_STORE_TYPE>
class StatementAttributeTraversal : public AstSimpleProcessing
{
public:
  StatementAttributeTraversal(DFI_STORE_TYPE store_) : store(store_) {}
  virtual ~StatementAttributeTraversal();

  std::string getPreInfo(SgStatement* stmt);
  std::string getPostInfo(SgStatement* stmt);
protected:
  void visit(SgNode *);
  std::string currentFunction();

  virtual void handleStmtDfi(SgStatement* stmt,std::string preInfo, std::string postInfo) {}
  virtual void visitBasicBlock(SgBasicBlock* stmt) {}

  DFI_STORE_TYPE store;
  std::string _currentFunction;

  virtual std::string  statementPreInfoString(DFI_STORE_TYPE store, SgStatement *stmt) { return ""; }
  virtual std::string statementPostInfoString(DFI_STORE_TYPE store, SgStatement *stmt) { return ""; }
private:
  SgFunctionDeclaration* enclosing_function(SgNode *node);
};

template<typename DFI_STORE_TYPE>
class DfiCommentAnnotator : public StatementAttributeTraversal<DFI_STORE_TYPE>
{
public:
  DfiCommentAnnotator(DFI_STORE_TYPE store_):StatementAttributeTraversal<DFI_STORE_TYPE>(store_) {}
  virtual ~DfiCommentAnnotator();

protected:
  virtual void handleStmtDfi(SgStatement* stmt,std::string preInfo, std::string postInfo);

  void addComment(std::string comment, PreprocessingInfo::RelativePositionType posSpecifier, SgStatement* node);
  void addCommentBeforeNode(std::string comment, SgStatement* node);
  void addCommentAfterNode(std::string comment, SgStatement* node);

private:
};

template<typename DFI_STORE_TYPE>
class DfiTextPrinter : public StatementAttributeTraversal<DFI_STORE_TYPE>
{
public:
  DfiTextPrinter(DFI_STORE_TYPE store_) : StatementAttributeTraversal<DFI_STORE_TYPE>(store_) {}
  virtual ~DfiTextPrinter();

protected:
  void handleStmtDfi(SgStatement* stmt,std::string preInfo, std::string postInfo);

private:
};

template<typename DFI_STORE_TYPE>
class DfiDotGenerator : public StatementAttributeTraversal<DFI_STORE_TYPE>
{
public:
  DfiDotGenerator(DFI_STORE_TYPE store_);
  virtual ~DfiDotGenerator();
  void edgeInfoOn();
  void edgeInfoOff();
  bool edgeInfo();

protected:
  virtual void handleStmtDfi(SgStatement* stmt,std::string preInfo, std::string postInfo);
  virtual void visitBasicBlock(SgBasicBlock* stmt);
  std::string addEdgeLabel(std::string s);
  SgStatement* lastStmtofStmtList(SgStatementPtrList& l);
private:
  bool _edgeInfo;
};


////////////////////////////////////////////////////////////////////////
// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: StatementAttributeTraversal.h,v 1.6 2008-09-09 14:21:17 gergo Exp $

/* this file is inlcuded by StatementAttributeTraversal.h for template instantiation */

#include "cfg_support.h"
#include "analysis_info.h"

template<typename DFI_STORE_TYPE>
SgFunctionDeclaration* 
StatementAttributeTraversal<DFI_STORE_TYPE>::enclosing_function(SgNode *node) {
    if (isSgFile(node))
        return NULL;
    else if (isSgFunctionDeclaration(node))
        return isSgFunctionDeclaration(node);
    else {
      // MS: in ROSE 0.8.10e parent pointers of unreachable nodes in case-stmts are NULL.
      //     therefore we check that pointer (consequence: --textoutput does not print such statements)
      if(SgNode* p=node->get_parent()) { 
        return enclosing_function(p);
      } else {
	return NULL; // MS: we cannot find the function declaration node if the parent-pointer is not properly set
      }
    }
}

template<typename DFI_STORE_TYPE>
void DfiCommentAnnotator<DFI_STORE_TYPE>::addComment(std::string comment, PreprocessingInfo::RelativePositionType posSpecifier, SgStatement* node) {
  PreprocessingInfo* commentInfo = 
    new PreprocessingInfo(PreprocessingInfo::CplusplusStyleComment, 
			  comment,
			  "user-generated",0, 0, 0, 
			  posSpecifier // e.g. PreprocessingInfo::before
           // GB (2008-09-09): This function used to have two parameters
           // more; they are gone since ROSE 0.9.3a (at least).
              );
  node->addToAttachedPreprocessingInfo(commentInfo);
}

template<typename DFI_STORE_TYPE>
void DfiCommentAnnotator<DFI_STORE_TYPE>::addCommentBeforeNode(std::string comment, SgStatement* node) {
  addComment(comment,PreprocessingInfo::before,node);
}

template<typename DFI_STORE_TYPE>
void DfiCommentAnnotator<DFI_STORE_TYPE>::addCommentAfterNode(std::string comment, SgStatement* node) {
  addComment(comment,PreprocessingInfo::after,node);
}


template<typename DFI_STORE_TYPE>
std::string StatementAttributeTraversal<DFI_STORE_TYPE>::getPreInfo(SgStatement* stmt) {
  std::stringstream ss1;

  if (stmt->attributeExists("PAG statement start")) {
    StatementAttribute *start = (StatementAttribute *) stmt->getAttribute("PAG statement start");

    BasicBlock *startb = start->get_bb();
    if (startb != NULL) {
      //ss1 << (carrier_printfunc(CARRIER_TYPE)(
      //				   (carrier_type_o(CARRIER_TYPE))
      //					   get_statement_pre_info(store, stmt)
      //					   ));
      ss1 << statementPreInfoString(store,stmt);
    } else
      ss1 << "<undefined dfi>";
  } else
    ss1 << "<undefined dfi>";

  return ss1.str();
}

template<typename DFI_STORE_TYPE>
std::string StatementAttributeTraversal<DFI_STORE_TYPE>::getPostInfo(SgStatement* stmt) {
  std::stringstream ss2;

  if (stmt->attributeExists("PAG statement end")) {
    StatementAttribute* end= (StatementAttribute *) stmt->getAttribute("PAG statement end");

    BasicBlock *endb = end->get_bb();
    if (endb != NULL) {
      ss2 << statementPostInfoString(store,stmt);
    } else
      ss2 << "<undefined dfi>";
  } else
    ss2 << "<undefined dfi>";

  return ss2.str();
}

template<typename DFI_STORE_TYPE>
StatementAttributeTraversal<DFI_STORE_TYPE>::~StatementAttributeTraversal() {
}

template<typename DFI_STORE_TYPE>
std::string StatementAttributeTraversal<DFI_STORE_TYPE>::currentFunction() {
  return _currentFunction;
}

template<typename DFI_STORE_TYPE>
void StatementAttributeTraversal<DFI_STORE_TYPE>::visit(SgNode *node)
{
    if (isSgStatement(node))
    {
        SgStatement *stmt = isSgStatement(node);
        SgFunctionDeclaration *func = enclosing_function(stmt);
        if(func!=NULL) {
            bool isBBStatement = isSgBasicBlock(stmt->get_parent());
            bool isIfPart = isSgIfStmt(stmt->get_parent())
                && stmt != isSgIfStmt(stmt->get_parent())->get_conditional();
            bool isWhileBody = isSgWhileStmt(stmt->get_parent())
                && stmt == isSgWhileStmt(stmt->get_parent())->get_body();
            bool isDoWhileBody = isSgDoWhileStmt(stmt->get_parent())
                && stmt == isSgDoWhileStmt(stmt->get_parent())->get_body();
            bool isForBody = isSgForStatement(stmt->get_parent())
                && stmt==isSgForStatement(stmt->get_parent())->get_loop_body();
            bool isInterestingStatement = isBBStatement
                                       || isIfPart
                                       || isWhileBody
                                       || isDoWhileBody
                                       || isForBody;
            if (!isSgBasicBlock(stmt) && isInterestingStatement) {
                std::string funcname = func->get_name().str();
                _currentFunction=funcname;
                handleStmtDfi(stmt,getPreInfo(stmt),getPostInfo(stmt));
            }
            if(SgBasicBlock* bb=isSgBasicBlock(stmt)) {
                visitBasicBlock(bb); // Basic Block has no annotation but we need it sometimes
            }
        }
    }
}

template<typename DFI_STORE_TYPE>
DfiCommentAnnotator<DFI_STORE_TYPE>::~DfiCommentAnnotator() {
}

template<typename DFI_STORE_TYPE>
void DfiCommentAnnotator<DFI_STORE_TYPE>::handleStmtDfi(SgStatement* stmt,std::string preInfo, std::string postInfo) {
  addCommentBeforeNode("// pre info : "+preInfo,stmt);
  addCommentAfterNode("// post info: "+postInfo,stmt);
}

template<typename DFI_STORE_TYPE>
DfiTextPrinter<DFI_STORE_TYPE>::~DfiTextPrinter() {
}

template<typename DFI_STORE_TYPE>
void DfiTextPrinter<DFI_STORE_TYPE>::handleStmtDfi(SgStatement* stmt,std::string preInfo, std::string postInfo) {
  
  //std::cout << "Unparsing:" << std::endl;
// std::string stmt_str = stmt->unparseToString(0);
  std::string stmt_str = Ir::fragmentToString(stmt);
  //std::cout << "done. Printing info now ... " << std::endl;
  std::cout << DfiTextPrinter<DFI_STORE_TYPE>::currentFunction() << ": " << "// pre info : " << preInfo << std::endl;
  std::cout << DfiTextPrinter<DFI_STORE_TYPE>::currentFunction() << ": " << stmt_str << std::endl;
  std::cout << DfiTextPrinter<DFI_STORE_TYPE>::currentFunction() << ": " << "// post info: " << postInfo << std::endl << std::endl;
}

template<typename DFI_STORE_TYPE>
DfiDotGenerator<DFI_STORE_TYPE>::DfiDotGenerator(DFI_STORE_TYPE store_):StatementAttributeTraversal<DFI_STORE_TYPE>(store_) {
  edgeInfoOn();
}

template<typename DFI_STORE_TYPE>
DfiDotGenerator<DFI_STORE_TYPE>::~DfiDotGenerator() {
}

template<typename DFI_STORE_TYPE>
SgStatement* DfiDotGenerator<DFI_STORE_TYPE>::lastStmtofStmtList(SgStatementPtrList& l) {
  SgStatement* stmt=0;
  for(SgStatementPtrList::iterator i=l.begin(); i!=l.end(); ++i) {
    stmt=*i;
  }
  return stmt;
}

template<typename DFI_STORE_TYPE>
void DfiDotGenerator<DFI_STORE_TYPE>::edgeInfoOn() { _edgeInfo=true; }
template<typename DFI_STORE_TYPE>
void DfiDotGenerator<DFI_STORE_TYPE>::edgeInfoOff() { _edgeInfo=false; }
template<typename DFI_STORE_TYPE>
bool DfiDotGenerator<DFI_STORE_TYPE>::edgeInfo() { return _edgeInfo; }

template<typename DFI_STORE_TYPE>
std::string 
DfiDotGenerator<DFI_STORE_TYPE>::addEdgeLabel(std::string s) {
  if(edgeInfo()) return " [label=\""+s+"\"]";
  return "";
}

template<typename DFI_STORE_TYPE>
void DfiDotGenerator<DFI_STORE_TYPE>::visitBasicBlock(SgBasicBlock* stmt) {
  switch(stmt->variantT()) {
  case V_SgBasicBlock: {
    SgBasicBlock* bb=isSgBasicBlock(stmt);
    SgStatementPtrList& bblist=bb->get_statements();
    SgStatement* prev=0;
    SgStatement* current=0;
    for(SgStatementPtrList::iterator i=bblist.begin(); i!=bblist.end(); ++i) {
      if(prev==0) {
	prev=*i;
	continue;
      }
      current=*i;
      std::string prevPostInfo=DfiDotGenerator<DFI_STORE_TYPE>::getPostInfo(prev);
      std::string currentPreInfo=DfiDotGenerator<DFI_STORE_TYPE>::getPreInfo(current);
      if(prevPostInfo!=currentPreInfo) {
 // std::cout << "\"" << prevPostInfo << "\" -> " << "\"" << currentPreInfo << "\"" << addEdgeLabel(prev->unparseToString()) << ";" << std::endl;
	std::cout << "\"" << prevPostInfo << "\" -> " << "\"" << currentPreInfo << "\"" << addEdgeLabel(Ir::fragmentToString(prev)) << ";" << std::endl;
      }
    }
    break;
  }
  default:{}
  }
}

template<typename DFI_STORE_TYPE>
void DfiDotGenerator<DFI_STORE_TYPE>::handleStmtDfi(SgStatement* stmt,std::string preInfo, std::string postInfo) {
  switch(stmt->variantT()) {
  case V_SgIfStmt: {
    SgIfStmt* ifstmt=isSgIfStmt(stmt);

    SgBasicBlock* trueBranch=ifstmt->get_true_body();
    SgStatementPtrList& trueBranchStmtList=trueBranch->get_statements();
    SgStatement* trueBranchFirstStmt=*(trueBranchStmtList.begin());
 // std::cout << "\"" << DfiDotGenerator<DFI_STORE_TYPE>::getPreInfo(ifstmt) << "\" -> \"" << DfiDotGenerator<DFI_STORE_TYPE>::getPreInfo(trueBranchFirstStmt) << "\" " << addEdgeLabel(ifstmt->get_conditional()->unparseToString()+":T") << ";" << std::endl;
    std::cout << "\"" << DfiDotGenerator<DFI_STORE_TYPE>::getPreInfo(ifstmt) << "\" -> \"" << DfiDotGenerator<DFI_STORE_TYPE>::getPreInfo(trueBranchFirstStmt) << "\" " << addEdgeLabel(Ir::fragmentToString(ifstmt->get_conditional())+":T") << ";" << std::endl;

    SgBasicBlock* falseBranch=ifstmt->get_false_body();
    SgStatementPtrList& falseBranchStmtList=falseBranch->get_statements();
    SgStatement* falseBranchFirstStmt=*(falseBranchStmtList.begin());
 // std::cout << "\"" << DfiDotGenerator<DFI_STORE_TYPE>::getPreInfo(ifstmt) << "\" -> \"" << DfiDotGenerator<DFI_STORE_TYPE>::getPreInfo(falseBranchFirstStmt) << "\" " << addEdgeLabel(ifstmt->get_conditional()->unparseToString()+":F") << ";" << std::endl;
    std::cout << "\"" << DfiDotGenerator<DFI_STORE_TYPE>::getPreInfo(ifstmt) << "\" -> \"" << DfiDotGenerator<DFI_STORE_TYPE>::getPreInfo(falseBranchFirstStmt) << "\" " << addEdgeLabel(Ir::fragmentToString(ifstmt->get_conditional())+":F") << ";" << std::endl;

    /* find last statement of true and false branch and connect its postinfo with if's postinfo */
    SgStatement* trueBranchLastStmt=lastStmtofStmtList(trueBranchStmtList);
    std::cout << "\"" << DfiDotGenerator<DFI_STORE_TYPE>::getPostInfo(trueBranchLastStmt) << "\"" 
	      << " -> "
	   // << "\"" << DfiDotGenerator<DFI_STORE_TYPE>::getPostInfo(ifstmt) << "\" " << addEdgeLabel(trueBranchLastStmt->unparseToString()) << ";" << std::endl;
	      << "\"" << DfiDotGenerator<DFI_STORE_TYPE>::getPostInfo(ifstmt) << "\" " << addEdgeLabel(Ir::fragmentToString(trueBranchLastStmt)) << ";" << std::endl;
    SgStatement* falseBranchLastStmt=lastStmtofStmtList(falseBranchStmtList);
    std::cout << "\"" << DfiDotGenerator<DFI_STORE_TYPE>::getPostInfo(lastStmtofStmtList(falseBranchStmtList)) << "\"" 
	      << " -> "
	   // << "\"" << DfiDotGenerator<DFI_STORE_TYPE>::getPostInfo(ifstmt) << "\"" << addEdgeLabel(falseBranchLastStmt->unparseToString()) << ";" << std::endl;
	      << "\"" << DfiDotGenerator<DFI_STORE_TYPE>::getPostInfo(ifstmt) << "\"" << addEdgeLabel(Ir::fragmentToString(falseBranchLastStmt)) << ";" << std::endl;
    break;
  }

  case V_SgWhileStmt: {
    SgWhileStmt* whlstmt=isSgWhileStmt(stmt);
    SgBasicBlock* trueBranch=whlstmt->get_body();
    SgStatementPtrList& trueBranchStmtList=trueBranch->get_statements();
    SgStatement* trueBranchFirstStmt=*(trueBranchStmtList.begin());
 // std::string whlCondString=whlstmt->get_condition()->unparseToString();
    std::string whlCondString=Ir::fragmentToString(whlstmt->get_condition());
    std::cout << "\"" << DfiDotGenerator<DFI_STORE_TYPE>::getPreInfo(stmt) << "\" -> \"" << DfiDotGenerator<DFI_STORE_TYPE>::getPreInfo(trueBranchFirstStmt) << "\"" << addEdgeLabel(whlCondString+":T") << ";" << std::endl;

    /* loop back edge */
    SgStatement* lastStmt=lastStmtofStmtList(trueBranchStmtList);
    std::cout << "\"" << DfiDotGenerator<DFI_STORE_TYPE>::getPostInfo(lastStmt) << "\"" 
	      << " -> "
	   // << "\"" << DfiDotGenerator<DFI_STORE_TYPE>::getPreInfo(whlstmt) << "\"" << addEdgeLabel(lastStmt->unparseToString()) << ";" << std::endl;
	      << "\"" << DfiDotGenerator<DFI_STORE_TYPE>::getPreInfo(whlstmt) << "\"" << addEdgeLabel(Ir::fragmentToString(lastStmt)) << ";" << std::endl;
       
    /* finding next statement on false-branch is non-trivial; we use while-stmt post info instead  */
    std::cout << "\"" << DfiDotGenerator<DFI_STORE_TYPE>::getPreInfo(whlstmt) << "\" -> " << "\"" << DfiDotGenerator<DFI_STORE_TYPE>::getPostInfo(whlstmt) << "\"" << addEdgeLabel(whlCondString+":F") << ";" << std::endl;
    break;
  }

  default:
    std::cout << "\"" << preInfo << "\" -> " << "\"" << postInfo << "\";" << std::endl;
  }
}

#endif
