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
    else
        return enclosing_function(node->get_parent());
}

template<typename DFI_STORE_TYPE>
void DfiCommentAnnotator<DFI_STORE_TYPE>::addComment(std::string comment, PreprocessingInfo::RelativePositionType posSpecifier, SgStatement* node) {
  PreprocessingInfo* commentInfo = 
    new PreprocessingInfo(PreprocessingInfo::CplusplusStyleComment, 
			  comment,
			  "user-generated",0, 0, 0, 
			  posSpecifier, // e.g. PreprocessingInfo::before
			  false, 
			  true);
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
  return ss1.str();
}

template<typename DFI_STORE_TYPE>
std::string StatementAttributeTraversal<DFI_STORE_TYPE>::getPostInfo(SgStatement* stmt) {
  std::stringstream ss2;

  StatementAttribute* end= (StatementAttribute *) stmt->getAttribute("PAG statement end");

  BasicBlock *endb = end->get_bb();
  if (endb != NULL) {
    ss2 << statementPostInfoString(store,stmt);
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
	  if (isSgBasicBlock(stmt->get_parent())) {

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
  std::string stmt_str = stmt->unparseToString();
  std::cout << currentFunction() << ": " << "// pre info : " << preInfo << std::endl;
  std::cout << currentFunction() << ": " << stmt_str << std::endl;
  std::cout << currentFunction() << ": " << "// post info: " << postInfo << std::endl << std::endl;
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
      std::string prevPostInfo=getPostInfo(prev);
      std::string currentPreInfo=getPreInfo(current);
      if(prevPostInfo!=currentPreInfo) {
	std::cout << "\"" << prevPostInfo << "\" -> " << "\"" << currentPreInfo << "\"" << addEdgeLabel(prev->unparseToString()) << ";" << std::endl;
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
    std::cout << "\"" << getPreInfo(ifstmt) << "\" -> \"" << getPreInfo(trueBranchFirstStmt) << "\" " << addEdgeLabel(ifstmt->get_conditional()->unparseToString()+":T") << ";" << std::endl;

    SgBasicBlock* falseBranch=ifstmt->get_false_body();
    SgStatementPtrList& falseBranchStmtList=falseBranch->get_statements();
    SgStatement* falseBranchFirstStmt=*(falseBranchStmtList.begin());
    std::cout << "\"" << getPreInfo(ifstmt) << "\" -> \"" << getPreInfo(falseBranchFirstStmt) << "\" " << addEdgeLabel(ifstmt->get_conditional()->unparseToString()+":F") << ";" << std::endl;

    /* find last statement of true and false branch and connect its postinfo with if's postinfo */
    SgStatement* trueBranchLastStmt=lastStmtofStmtList(trueBranchStmtList);
    std::cout << "\"" << getPostInfo(trueBranchLastStmt) << "\"" 
	      << " -> "
	      << "\"" << getPostInfo(ifstmt) << "\" " << addEdgeLabel(trueBranchLastStmt->unparseToString()) << ";" << std::endl;
    SgStatement* falseBranchLastStmt=lastStmtofStmtList(falseBranchStmtList);
    std::cout << "\"" << getPostInfo(lastStmtofStmtList(falseBranchStmtList)) << "\"" 
	      << " -> "
	      << "\"" << getPostInfo(ifstmt) << "\"" << addEdgeLabel(falseBranchLastStmt->unparseToString()) << ";" << std::endl;
    break;
  }

  case V_SgWhileStmt: {
    SgWhileStmt* whlstmt=isSgWhileStmt(stmt);
    SgBasicBlock* trueBranch=whlstmt->get_body();
    SgStatementPtrList& trueBranchStmtList=trueBranch->get_statements();
    SgStatement* trueBranchFirstStmt=*(trueBranchStmtList.begin());
    std::string whlCondString=whlstmt->get_condition()->unparseToString();
    std::cout << "\"" << getPreInfo(stmt) << "\" -> \"" << getPreInfo(trueBranchFirstStmt) << "\"" << addEdgeLabel(whlCondString+":T") << ";" << std::endl;

    /* loop back edge */
    SgStatement* lastStmt=lastStmtofStmtList(trueBranchStmtList);
    std::cout << "\"" << getPostInfo(lastStmt) << "\"" 
	      << " -> "
	      << "\"" << getPreInfo(whlstmt) << "\"" << addEdgeLabel(lastStmt->unparseToString()) << ";" << std::endl;
       
    /* finding next statement on false-branch is non-trivial; we use while-stmt post info instead  */
    std::cout << "\"" << getPreInfo(whlstmt) << "\" -> " << "\"" << getPostInfo(whlstmt) << "\"" << addEdgeLabel(whlCondString+":F") << ";" << std::endl;
    break;
  }

  default:
    std::cout << "\"" << preInfo << "\" -> " << "\"" << postInfo << "\";" << std::endl;
  }
}

