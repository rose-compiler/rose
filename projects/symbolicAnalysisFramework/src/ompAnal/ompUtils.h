#ifndef OMP_UTILS_H
#define OMP_UTILS_H

#include <string>
#include <list>
#include <iostream>
#include <sstream>
#include <boost/regex.hpp>
#include "DataflowCFG.h"
#include "CFGRewrite.h"

using namespace std;

class ompUtils
{
	public:
	static const int unknown_type =      -1;
	static const int omp_parallel =      0;
	static const int omp_for =           1;
	static const int omp_parallel_for =	 2;
	static const int omp_sections =      3;
	static const int omp_section =       4;
	static const int omp_single =        5;
	static const int omp_master =	       6;
	static const int omp_critical =      7;
	static const int omp_barrier =       8;
	static const int omp_atomic =        9;
	static const int omp_flush =         10;
	static const int omp_threadprivate = 11;

	static bool match_OMP_String(string directive, string targetStr);

	static int getOmpType(string directive);
	static string ompTypeStr(int ompType);
};

/*******************************
 ***** R E D U C T I O N S *****
 *******************************/

class reductionOperation
{
	public:
	static const int unknown_type =          -1;
   static const int reduction_plus =        0;
   static const int reduction_multiply =    1;
   static const int reduction_minus =       2;
   static const int reduction_logic_AND =   3;
   static const int reduction_logic_XOR =   4;
   static const int reduction_logic_OR =    5;
   static const int reduction_boolean_AND = 6;
   static const int reduction_boolean_OR =  7;

	protected:
	int op;

	public:
	reductionOperation();

	reductionOperation(const reductionOperation &that);

	reductionOperation(string opStr);
	
	void init(string opStr);

	string str();
};

class ompReduction
{
	reductionOperation operation;
	list<string> variables;
	public:

	ompReduction(const ompReduction& that);

	// numRedVars - the number of reduction variables in this match (needed to get around a boost regex bug)
	ompReduction(boost::smatch what, int numRedVars);

	string str();

	int numVars();

	reductionOperation& getOp();
};


class ompReductionsSet
{
	list<ompReduction> reductions;

	public:
	ompReductionsSet();
		
	ompReductionsSet(const ompReductionsSet& that);

	ompReductionsSet(string directive);

	int numReductions();

	string str();
};

/***********************************
 ***** D A T A   C L A U S E S *****
 ***********************************/

class ompDataClause
{
	string clauseType;
	list<string> variables;
	public:

	ompDataClause();
	
	ompDataClause(const ompDataClause &that);

	ompDataClause(string directive, string clauseType);

	int numVars() const;
	
	const list<string>& getVars() const;
	
	void addVar(string var);

	string str() const;
};

class defaultVarSharing
{
	public:
	static const int var_sharing_class_unknown =   0;
	static const int var_sharing_class_none =   1;
	static const int var_sharing_class_shared = 2;

	protected:
	int type;

	public:
	defaultVarSharing(const defaultVarSharing& that);

	defaultVarSharing(string directive);
	
	string str();
};

/********************************
 ***** #pragma omp parallel *****
 ********************************/
class ompParallelFor;

class ompParallel
{
	public:
	ompReductionsSet  reductions;
	ompDataClause     privateVars;
	ompDataClause     firstPrivateVars;
	ompDataClause     lastPrivateVars;
	ompDataClause     sharedVars;
	ompDataClause     copyinVars;
	defaultVarSharing varSharingClass;

	ompParallel(string directive, bool verbose=false);
	
	ompParallel(ompParallelFor opf);

	ompParallel(ompReductionsSet oldReductions, ompDataClause oldPrivateVars,
	            ompDataClause oldFirstPrivateVars, ompDataClause oldLastPrivateVars,
	            ompDataClause oldSharedVars, ompDataClause oldCopyinVars,
	            defaultVarSharing oldVarSharingClass);

	string str();
	string str_nopragma();
};

/***************************
 ***** #pragma omp for *****
 ***************************/
class ompFor
{
	bool ordered;
	bool nowait;

	public:
	ompReductionsSet reductions;
	ompDataClause privateVars;
	ompDataClause firstPrivateVars;
	ompDataClause lastPrivateVars;
	
	ompFor(string directive, bool verbose = false);

	ompFor(ompReductionsSet oldReductions, ompDataClause oldPrivateVars,
	       ompDataClause oldFirstPrivateVars, ompDataClause oldLastPrivateVars,
	       bool oldOrdered, bool oldNoWait);
	
	ompFor(bool oldOrdered, bool oldNoWait);
	
	bool getOrdered();
	bool getNowait();

	string str();
	string str_nopragma();
};

/************************************
 ***** #pragma omp parallel for *****
 ************************************/

class ompParallelFor
{
	bool ordered;
		
	public:
	
	ompReductionsSet reductions;
	ompDataClause    privateVars;
	ompDataClause    firstPrivateVars;
	ompDataClause    lastPrivateVars;
 	ompDataClause    sharedVars;
	ompDataClause    copyinVars;

	defaultVarSharing varSharingClass;

	ompParallelFor(string directive, bool verbose = false);

	ompParallelFor(ompReductionsSet oldReductions, ompDataClause oldPrivateVars,
	               ompDataClause oldFirstPrivateVars, ompDataClause oldLastPrivateVars,
	               ompDataClause oldSharedVars, ompDataClause oldCopyinVars,
	               bool oldOrdered, defaultVarSharing oldVarSharingClass);

	bool getOrdered();
	
	string str();
	string str_nopragma();
	
	friend class ompParallel;
};

/******************************
 ***** #pragma omp single *****
 ******************************/
class ompSingle
{
	bool nowait;
	
	public:
	ompDataClause privateVars;
	ompDataClause firstPrivateVars;
	ompDataClause copyPrivateVars;
	

	ompSingle(string directive, bool verbose = false);
	
	ompSingle(ompDataClause oldPrivateVars, ompDataClause oldFirstPrivateVars, 
	          ompDataClause oldCopyPrivateVars, bool oldNoWait);

	ompSingle(bool oldNoWait);
	

	bool getNowait();
	string str();
	string str_nopragma();
};

/*************************************
 ***** #pragma omp threadprivate *****
 *************************************/
class ompThreadprivate
{
	public:
	ompDataClause     vars;

	ompThreadprivate(string directive, bool verbose=false);
	
	ompThreadprivate(ompThreadprivate& otp);

	ompThreadprivate(ompDataClause vars);

	string str();
	string str_nopragma();
};


/*************************************
 ***** OpenMP-specific insertion *****
 *************************************/

// returns the statement that follows the given pragma
SgStatement* getPragmaBody(SgPragmaDeclaration* pragma);

// calls annotateOmpFor() on all #pragma omp for SgForStatements in the given AST tree
void annotateAllOmpFors(SgNode* root);

// annotates the headers #pragma omp for loops to indicate that no insertion should take place
// in the test and update sub-trees and that all insertions into the inialization sub-tree should 
// actually be placed right before the #pragma omp for
void annotateOmpFor(SgPragmaDeclaration* pragma, SgForStatement* ompFor);

class OMPcfgRWTransaction : public cfgRWTransaction
{
	public:
	void insertBefore(DataflowNode cfgNode, SgExpression* newNode);
	void insertBefore(SgNode* n, SgExpression* newNode);
	
	void insertAfter(DataflowNode cfgNode, SgExpression* newNode);
	void insertAfter(SgNode* n, SgExpression* newNode);
	
	// insert an SgNode along the given CFGEdge
	void insertAlong(DataflowEdge e, SgExpression* newNode);
};


// surrounds the body of the given pragma with a SgBasicBlock, 
// if oldBody!=NULL sets it to this pragma's original body 
// if newBody!=NULL setts it to this pragma's new body
void wrapPragmaBody(SgPragmaDeclaration* pragma, SgStatement** oldBody=NULL, SgBasicBlock** newBody=NULL);

// surrounds the given pragma and its body with a SgBasicBlock, 
// returns this new SgBasicBlock
SgBasicBlock* wrapPragmaAndBody(SgPragmaDeclaration* pragma);

// inserts the given statement to either the top or bottom of the given OpenMP directive 
// supports any directive that may have a basic block as its body
void insertTopBottomOmpDirective(SgPragmaDeclaration* pragma, bool top, SgStatement* stmt);

// inserts statements generated by stmtCreate at either the top or the bottom of the body of all OpenMP directives of the given 
//     type in the given AST subtree
// supports any directive that may have a basic block as its body
void insertTopBottomOmpDirectives(SgNode* root, int ompType, bool top, SgStatement* (*stmtCreate)());

#endif
