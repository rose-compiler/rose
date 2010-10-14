#ifndef CFG_REWRITE_H
#define CFG_REWRITE_H

#include <string>
#include <iostream>
#include <sstream>
#include "DataflowCFG.h"

using namespace std;

namespace VirtualCFG{
	
///*void insertAfterCFG(DataflowNode cfgNode, SgProject* project);
//SgStatement* getParentStmt(SgNode* node);*/
//
//void insertAfterCFG(DataflowNode& cfgNode);
//
//void placeInserts(SgProject *project);

void initCFGRewrite(SgProject* project);

// returns true if the given SgNode is a value expression or a computational expression with no side-effects
bool isNonMutatingOperator(SgNode* n);

// returns true if the given SgNode's sub-tree has no side-effects
bool isNonMutatingSubTree(SgNode* n);

// if the given SgNode is an SgStatement, returns that SgStatement. Otherwise, if it is an
//    gExpression or SgInitializedName, wraps it in an SgStatement and returns that.
static SgStatement* convertToStatement(SgNode* n);

// Replace the expression from with the expression to in the SgNode parent, which
//    must be from's parent. Function checks to ensure that it is used properly.
// Note that if parent is an SgInitializedName, to must be SgInitializer in order 
//    for the replacement to work properly.
void replaceExpressionChecked(SgNode* parent, SgExpression* from, SgExpression* to);

// replace the from statement with the to statement in the parent SgNode
// Note: Only parent = SgBasicBlock
void replaceStatement(SgNode* parent, SgStatement* from, SgStatement* to);

Sg_File_Info* getFileInfo(SgNode* n);

string getFileInfoString(SgNode* n);

string getFileInfoString(CFGNode n);

// Returns the source of n's only in-edge. Yells if n has multiple in-edges.
CFGNode theInEdge(CFGNode n);

// Returns the target of n's only out-edge. Yells if n has multiple out-edges.
CFGNode theOutEdge(CFGNode n);

// FIXME -- these functions need to split subexpressions, since the expression
// evaluation order in the CFG and the one used by the compiler may be
// different

// replace expr with (newNode, expr)
SgExpression* insertBeforeExpression(SgExpression* expr, SgExpression* newNode);

// replace expr with (expr, newNode)
SgExpression* insertAfterExpression(SgExpression* expr, SgExpression* newNode);

// replace stmt; with {newNode; stmt;}
void replaceStatementByBlockBefore(SgStatement* stmt, SgStatement* newNode) ;

// replace stmt; with {stmt; newNode;}
void replaceStatementByBlockAfter(SgStatement* stmt, SgStatement* newNode);

// creates a new variable declaration for a temporary variable
// newName: the name of the temporary variable. If newName=="", a random name is generated.
// varType: the type of the temporary variable, if byReference==true, the new variable's type is a reference type of varType
// 
// sets varname to the new variable's SgName
// sets initName to the new variable's SgInitializedName 
// sets newType to the new variable's type
// sets newType to the new variable's declaration
void createTmpVarInit(SgType* varType, string newName, bool byReference, 
                      SgName& varName, SgInitializedName *& initName, SgType*& newType, SgVariableDeclaration*& varDecl);

// creates and returns a statement contains a call to the given function with no arguments
SgStatement* createFuncCallStmt(SgFunctionDeclaration* funcDecl);

// given a SgInitializedName, returns a SgVariableSymbol for the variable declared in the SgInitializedName
SgVariableSymbol* varSymFromInitName(SgInitializedName* initName);

// given a SgInitializedName, returns a SgVarRefExp to the variable declared in the SgInitializedName
SgVarRefExp* varRefFromInitName(SgInitializedName* initName);

// replaces the given SgExpression with a SgAssignOp (lhsVar = orig)
// returns the SgAssignOp
SgAssignOp * replaceExprWithAssignOp(SgExpression* orig, SgVarRefExp* lhsVar);

// Creates a declaration of a new temporary variable of type varType and inserts it before anchor
// if before=true, the temporary variable in inserted before anchor and otherwise, after anchor
// sets initName to the SgInitializedName of the declaration
void insertVarDecl(SgStatement *anchor, SgType *varType, bool before, SgInitializedName*& initName);

// inserts the given expression before or after a given SgDeclaration that appears inside a SgForStatement
void insertAroundForInit(SgVariableDeclaration* n, SgExpression *newNode, bool before);

typedef void (*CFGTransform)(SgNode *target, SgNode* newNode, void* data);

class cfgRWTransaction
{
	typedef enum {insBef, insAft, callback} modType;
	class modRequest{
		protected:
		modType type;
		
		public:
		modRequest()
		{}
		/*modRequest(modType type_arg)
		{
			type = type_arg;
		}*/
			
		modType getType()
		{ return type; }
		
		string str() { return ""; }
	};
	
	class insertRequest: public modRequest{
		protected:
		SgExpression *newNode;
		SgNode* origNode;
		
		public:
		insertRequest(modType type_arg, SgNode* origNode, SgExpression *&newNode)
		{
			this->type = type_arg;
			this->newNode = newNode;
			this->origNode = origNode;
		}
		
		SgNode* getTgtNode() { return origNode; }
		
		string str();
		friend class cfgRWTransaction;
	};
	
	class transformRequest: public modRequest{
		CFGTransform callbackFunc;
		SgNode* target;
		SgNode* newNode;
		void* data;
		
		public:
		transformRequest(CFGTransform callbackFunc, SgNode *&target, SgNode* newNode, void* data)//: modRequest(callback)
		{
			this->type = callback;
			this->target = target;
			this->newNode = newNode;
			this->callbackFunc = callbackFunc;
			this->data = data;
		}
		
		string str();
		friend class cfgRWTransaction;
	};
	
	public: 
	list<modRequest*> requests;
	//list<void*> requests;
	
	
	cfgRWTransaction();
	
	void beginTransaction();
	
	void insertBefore(DataflowNode n, SgExpression* newNode);
	void insertBefore(SgNode* n, SgExpression* newNode);
	
	void insertAfter(DataflowNode n, SgExpression* newNode);
	void insertAfter(SgNode* n, SgExpression* newNode);
	
	void transform(CFGTransform callbackFunc, SgNode* n, SgNode* newNode, void* data);
	
	// insert an SgNode along the given CFGEdge
	void insertAlong(DataflowEdge e, SgExpression* newNode);
	
	void commitTransaction();
	
	protected:
	void do_insertBefore(DataflowNode n, SgExpression* newNode);
	void do_insertBefore(SgNode* n, SgExpression* newNode);
	void do_insertAfter(DataflowNode n, SgExpression* newNode);
	void do_insertAfter(SgNode* n, SgExpression* newNode);
};

/*************************************************************
 *** CALL-BACK FUNCTIONS FOR cfgRWTransaction::transform() ***
 *************************************************************/

// places newNode as thh first statement in the given SgScopeStmt
void prependToScopeStmt(SgNode *target, SgNode *newNode, void* data);

// places newNode as thh last statement in the given SgBasicBlock
void appendToScopeStmt(SgNode *target, SgNode *newNode, void* data);

}
#endif
