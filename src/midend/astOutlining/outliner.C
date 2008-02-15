/*
A simplified OpenMP outliner to demonstrate the key idea of an OpenMP translator
and also to show how to accomplish a complex transformation in ROSE.

It traverses the AST tree using bottomup order and do the following work: 
  find all pragmas named "omp parallel",
  generate an outlined function based on the code immediately after the pragma
  insert the outlined function right before the enclosing function of the pragma
  replace the pragma and its followup code with a runtime call, 
  the runtime call has a parameter, which is a pointer to the outliend function.
The bottomup processing also enables correct translating of nested 'omp parallel'.

By C. Liao
Aug. 15, 2006
*/
#include "rose.h"
#include <iostream>
#include <string>
#include <sstream>

// DQ (8/16/2006): Include the code moved to the header file: outliner.h
#include "outliner.h"

using namespace std;

#define TRANS_FILE Sg_File_Info::generateDefaultFileInfoForTransformationNode()

static SgTreeCopy treeCopy;	// We need deep copy most of the time

class ASTtools
{
public:
  static SgNode *get_globalscope (SgNode * astNode);
  static SgNode *getEnclosingFunctionDeclaration (SgNode * astNode);
  static int moveUpPreprocessingInfo (SgStatement *, SgStatement *);
};

#if 0
// DQ (8/16/2006): This is moved to outliner.h
class OmpMidend
{
public:
  static int counter;		//unique number for outlined functions
  static int transParallelRegion (SgPragmaDeclaration * decl);
  static int bottomupProcessing (SgProject *);
  static int insertHeaders (SgProject *);	// header for runtime call prototypes
private:
  static SgFunctionDeclaration *generateOutlinedFunction (SgPragmaDeclaration
							  * decl);
  static SgStatement *generateParallelRTLcall (SgPragmaDeclaration * pragDecl,
					       SgFunctionDeclaration *
					       outlinedFunc);
};
#endif

int OmpMidend::counter = 0;

//----------------------------
//tasks for transforming a parallel region
// *. generate a unique outlined function
// *. replace parallel region with the runtime call to the outlined function
int
OmpMidend::transParallelRegion (SgPragmaDeclaration * decl)
{
  // generate outlined function
  SgFunctionDeclaration *outFuncDecl = generateOutlinedFunction (decl);
  ROSE_ASSERT (outFuncDecl != NULL);

  //insert outlined function
  SgFunctionDeclaration *targetDecl = isSgFunctionDeclaration
    (ASTtools::getEnclosingFunctionDeclaration (decl));
  ROSE_ASSERT (targetDecl != NULL);
  SgGlobal *globalScope = isSgGlobal (ASTtools::get_globalscope (decl));
  ROSE_ASSERT (globalScope != NULL);

  ASTtools::moveUpPreprocessingInfo (isSgStatement (outFuncDecl),
				     isSgStatement (targetDecl));
  globalScope->insert_statement (isSgStatement (targetDecl),
				 isSgStatement (outFuncDecl), true);
  //generate a runtime library call
  SgStatement *rtlCall = generateParallelRTLcall (decl, outFuncDecl);
  ROSE_ASSERT (rtlCall != NULL);

  // replace the pragma and its followup code with the RTL call
  SgStatement *targetBB = isSgStatement (decl->get_parent ());
  ROSE_ASSERT (targetBB != NULL);

  SgScopeStatement *scope = decl->get_scope ();
  ROSE_ASSERT (scope != NULL);
  SgStatementPtrList & statementList = scope->getStatementList ();
  list < SgStatement * >::iterator i = statementList.begin ();
  while ((i != statementList.end ()) && ((*i) != decl))
    {
      i++;
    }
  i++;
  SgStatement *oldblock = (*i);

  targetBB->insert_statement (oldblock, rtlCall, true);
  LowLevelRewrite::remove (oldblock);
  LowLevelRewrite::remove (decl);

  return 0;
}

//----------------------------
// generate an outlined function from a basic block or a single statement following a
// pragma representing a parallel region
SgFunctionDeclaration *
OmpMidend::generateOutlinedFunction (SgPragmaDeclaration * decl)
{
  SgFunctionDeclaration *func;

  SgType *func_return_type = new SgTypeVoid ();
  SgName func_name;
  counter++;			//increase static variable to get unique function id
  //function name: __ompc_func_x
  stringstream ss;
  ss << counter;
  func_name = SgName ("__ompc_func_" + ss.str ());
  SgFunctionType *func_type = new SgFunctionType (func_return_type, false);

  SgFunctionParameterList *parameterList =
    new SgFunctionParameterList (TRANS_FILE);
  ROSE_ASSERT (parameterList != NULL);
  SgScopeStatement *cscope = decl->get_scope ();
  SgBasicBlock *func_body = new SgBasicBlock (TRANS_FILE);

  func = new SgFunctionDeclaration (TRANS_FILE, func_name, func_type);
  // set static modifier for regular function declarations
  ((func->get_declarationModifier ()).get_storageModifier ()).setStatic ();
  SgFunctionDefinition *func_def =
    new SgFunctionDefinition (TRANS_FILE, func);
  func_def->set_body (func_body);
  func_def->set_parent (func);	//necessary or not?
  func->set_parameterList (parameterList);
  func->set_scope (cscope);

  // generate the function body, 
  // direct deep copy the immediate statement after the pragma
  // get next statement
  SgStatementPtrList & statementList =
    decl->get_scope ()->getStatementList ();
  list < SgStatement * >::iterator i = statementList.begin ();
  while ((i != statementList.end ()) && ((*i) != decl))
    {
      i++;
    }
  i++;
  SgStatement *nextStatement = (*i);
  //deepcopy
  if (isSgBasicBlock (nextStatement) == NULL)
    {
      // copy a statement
      SgStatement *myStatement = new SgStatement (TRANS_FILE);
      myStatement = isSgStatement (nextStatement->copy (treeCopy));
      func_body->append_statement (myStatement);
    }
  else
    {				// deep copy every statement from the basic block
      SgStatementPtrList srcStmtList =
	isSgBasicBlock (nextStatement)->get_statements ();
      for (std::list < SgStatement * >::iterator i = srcStmtList.begin ();
	   i != srcStmtList.end (); i++)
	{
	  SgStatement *mystmt = new SgStatement (TRANS_FILE);
	  mystmt = isSgStatement ((*i)->copy (treeCopy));
	  func_body->append_statement (mystmt);
	}
    }

  return func;
}

//----------------------------
// generate call like 
//      _ompc_do_parallel(__ompc_func_x);
SgStatement *
OmpMidend::generateParallelRTLcall (SgPragmaDeclaration * pragDecl,
				    SgFunctionDeclaration * outlinedFunc)
{
  //--- build the runtime call
  // SgFunctionRefExp: _ompc_do_parallel()
  SgType *voidtype = new SgTypeVoid ();
  SgFunctionType *myFuncType = new SgFunctionType (voidtype, false);
  SgFunctionDeclaration *doParallel = new SgFunctionDeclaration (TRANS_FILE,
								 SgName
								 ("_ompc_do_parallel"),
								 myFuncType,
								 NULL);
  //RTL functions have global scope
  doParallel->
    set_scope (isSgScopeStatement (ASTtools::get_globalscope (pragDecl)));
  SgFunctionSymbol *func_symbol = new SgFunctionSymbol (doParallel);
  SgFunctionRefExp *func_ref_exp =
    new SgFunctionRefExp (TRANS_FILE, func_symbol, myFuncType);
  //SgExprListExp, one parameter (__ompc_func_x)
  SgFunctionSymbol *func_symbol2;
  func_symbol2 = new SgFunctionSymbol (outlinedFunc);
  SgFunctionType *func_type2 =outlinedFunc->get_type();
    //isSgFunctionType (outlinedFunc->get_type ()->copy (treeCopy));
  SgFunctionRefExp *func_ref_exp2 =
    new SgFunctionRefExp (TRANS_FILE, func_symbol2, func_type2);
  SgExprListExp *exp_list_exp = new SgExprListExp (TRANS_FILE);
  exp_list_exp->append_expression (func_ref_exp2);

  //build call exp
  SgFunctionCallExp *func_call_expr =
    new SgFunctionCallExp (TRANS_FILE, func_ref_exp, exp_list_exp,
			   myFuncType);
  // build SgExprStatement
  SgExpressionRoot *expRoot =
    new SgExpressionRoot (TRANS_FILE, func_call_expr, myFuncType);
  SgExprStatement *expStmt = new SgExprStatement (TRANS_FILE, expRoot);

  return expStmt;
}

//----------------------------
// bottomup processing AST tree to translate all OpenMP constructs
// the major interface of OmpMidend
//----------------------------
int
OmpMidend::bottomupProcessing (SgProject * project)
{
/*
Neither preorder or postorder tree traversal will give us 
the desired bottomup processing for OpenMP pragmas.

For example: the code
                                                                                         
#pragma omp parallel
  {
    #pragma omp single
       statement1;
  }
                                                                                         
Its AST looks like:
                                                                                         
  SgBasicBlock1
    /      \
   /        \
SgPragma1  SgBasicBlock2
             /      \
         SgPragma2  SgStatement1
                                                                                         
For preorder, the order of visit would be: 
	(bb1, pragma1, bb2, pragma2, stmt1)
For postorder, it is 
	(pragma1, pragma2, stmt1,bb2, bb1)
In both cases, pragma1(omp parallel) is always visited before pragma2(omp single).
Alternative way: use the default preorder in AST query and reverse it:
We get what we want then: 
	(stmt1, pragma2, bb2,pragma1, bb1).
*/
  list < SgNode * >pragmaList =
    NodeQuery::querySubTree (project, V_SgPragmaDeclaration);
  typedef list < SgNode * >::reverse_iterator pragmaListIterator;
  for (pragmaListIterator listElement = pragmaList.rbegin ();
       listElement != pragmaList.rend (); ++listElement)
    {
      SgPragmaDeclaration *decl = isSgPragmaDeclaration (*listElement);
      ROSE_ASSERT (decl != NULL);
      string pragmaString = decl->get_pragma ()->get_pragma ();
      if (pragmaString.find ("omp parallel") != string::npos)
	OmpMidend::transParallelRegion (decl);
    }

  return 0;
}

//-------------
// get the closest enclosing function declaration for current node
SgNode *
ASTtools::getEnclosingFunctionDeclaration (SgNode * astNode)
{
  SgNode *astnode = astNode;
  ROSE_ASSERT (astNode != NULL);
  do
    {
      astnode = astnode->get_parent ();
    }
  while ((astnode != NULL) &&
	 (isSgFunctionDeclaration (astnode) == NULL) &&
	 (isSgMemberFunctionDeclaration (astnode) == NULL));
  return astnode;
}

//-------------
SgNode *
ASTtools::get_globalscope (SgNode * astNode)
{
  SgNode *astnode = astNode;
  ROSE_ASSERT (astNode != NULL);
  if (isSgGlobal (astnode) != NULL)
    return astnode;
  do
    {
      astnode = astnode->get_parent ();
    }
  while ((astnode != NULL) && (isSgGlobal (astnode) == NULL));
  return astnode;
}

//-------------
//move preprocessingInfo of stmt2 to stmt1
// useful when insert stmt1 before stmt2 sometimes
int
ASTtools::moveUpPreprocessingInfo (SgStatement * stmt1, SgStatement * stmt2)
{
  ROSE_ASSERT (stmt1 != NULL);
  ROSE_ASSERT (stmt2 != NULL);
  AttachedPreprocessingInfoType *infoList =
    stmt2->getAttachedPreprocessingInfo ();
  AttachedPreprocessingInfoType *infoToRemoveList =
    new AttachedPreprocessingInfoType ();

  if (infoList == NULL)
    return 0;

  for (std::list < PreprocessingInfo * >::iterator i = (*infoList).begin ();
       i != (*infoList).end (); i++)
    {
      PreprocessingInfo *info = dynamic_cast < PreprocessingInfo * >(*i);
      ROSE_ASSERT (info != NULL);

      if ((info->getTypeOfDirective () ==
	   PreprocessingInfo::CpreprocessorIncludeDeclaration)
	  || (info->getTypeOfDirective () ==
	      PreprocessingInfo::CpreprocessorDefineDeclaration)
	  || (info->getTypeOfDirective () ==
	      PreprocessingInfo::CpreprocessorUndefDeclaration)
	  || (info->getTypeOfDirective () ==
	      PreprocessingInfo::CpreprocessorIfdefDeclaration)
	  || (info->getTypeOfDirective () ==
	      PreprocessingInfo::CpreprocessorIfndefDeclaration)
	  || (info->getTypeOfDirective () ==
	      PreprocessingInfo::CpreprocessorIfDeclaration)
	  || (info->getTypeOfDirective () ==
	      PreprocessingInfo::CpreprocessorElseDeclaration)
	  || (info->getTypeOfDirective () ==
	      PreprocessingInfo::CpreprocessorElifDeclaration)
	  || (info->getTypeOfDirective () ==
	      PreprocessingInfo::C_StyleComment)
	  || (info->getTypeOfDirective () ==
	      PreprocessingInfo::CpreprocessorEndifDeclaration))
	{
	  stmt1->addToAttachedPreprocessingInfo (info,
						 PreprocessingInfo::after);
	  (*infoToRemoveList).push_back (*i);
	}			// end if
    }				// end for

  // Remove the element from the list of comments at the current astNode
  AttachedPreprocessingInfoType::iterator j;
  for (j = (*infoToRemoveList).begin (); j != (*infoToRemoveList).end (); j++)
    {
      (*infoList).remove (*j);
    }
  return 0;
}

//------------------------------------
// add include "ompcLib.h" into source files, right before the first statement from users
int
OmpMidend::insertHeaders (SgProject * project)
{
  list < SgNode * >globalScopeList =
    NodeQuery::querySubTree (project, V_SgGlobal);

  for (list < SgNode * >::iterator i = globalScopeList.begin ();
       i != globalScopeList.end (); i++)
    {
      SgGlobal *globalScope = isSgGlobal (*i);
      ROSE_ASSERT (globalScope != NULL);

      SgDeclarationStatementPtrList & stmtList =
	globalScope->get_declarations ();
      for (list < SgDeclarationStatement * >::iterator i = stmtList.begin ();
	   i != stmtList.end (); i++)
	{
	  //must have this judgement, otherwise wrong file will be modified!
	  if (((*i)->get_file_info ())->
	      isSameFile (globalScope->get_file_info ()))
	    {
	      (*i)->
		addToAttachedPreprocessingInfo (new
						PreprocessingInfo
						(PreprocessingInfo::
						 CpreprocessorIncludeDeclaration,
						 string
						 ("#include \"ompcLib.h\" \n"),
						 "Compiler-gnerated in OpenMP transformation",
						 0, 0, 0,
						 PreprocessingInfo::before,
						 false, false));
	      break;
	    }			//end if
	}			//end for
    }				//end for
  return 0;			//assume always successful currently
}

#if 0
// DQ (8/16/2006): Moved to tests/roseTests/astOutlinerTests directory.
/************************************************************************/
int
main (int argc, char *argv[])
{
  SgProject *project = frontend (argc, argv);
  ROSE_ASSERT (project != NULL);

  //insert #include "ompcLib.h", the interface of runtime library to the compiler
  OmpMidend::insertHeaders (project);

  OmpMidend::bottomupProcessing (project);
  return backend (project);
}
#endif
