//-*-Mode: C++;-*-
#ifndef _ArrayPaddingTransform_h_
#define _ArrayPaddingTransform_h_

#include "aptScope.h"

#if USE_SAGE3
#include <sage3.h>
#else
#include <sage2.h>
#endif

#include "grammar.h"
#include "cacheInfo.h"
#include "padding_conditions.h"
#include "tlist.h"


class ArrayPaddingTransform
{
  public:

	// CW: create transformation object
    ArrayPaddingTransform();
    ~ArrayPaddingTransform();

	// CW: perform transformation and return new ROSE_NODE tree
    ROSE_StatementBlock* pass(ROSE_StatementBlock* inputTree);
	
	// CW: print out number of A++ arrays, for padding considered arrays,
	// and padded arrays
	void printStatistics(ostream &os);
  
    
  private:

	// CW: overloaded function which traverse program tree and collect
	// data into member variables
	void traverse(ROSE_Statement *inputStmt);	
	void traverse(ROSE_DoWhileStatement *inputStmt);
	void traverse(ROSE_ElseWhereStatement *inputStmt);
	void traverse(ROSE_ExpressionStatement *inputStmt);
	void traverse(ROSE_ForStatement *inputStmt);
	void traverse(ROSE_IfStatement *inputStmt);
	void traverse(ROSE_ReturnStatement *inputStmt);
	void traverse(ROSE_WhereStatement *inputStmt);
	void traverse(ROSE_WhileStatement *inputStmt);
	void traverse(ROSE_C_Statement *inputStmt);
	void traverse(ROSE_StatementBlock* inputBlock,const aptScopeType& subScopeType=aptNonLoopScopeTag);
	void traverse(ROSE_ArrayVariableDeclaration* inputDeclaration);
	void traverse(ROSE_C_VariableDeclaration* inputDeclaration);
	void traverse(ROSE_IndexingVariableDeclaration* inputDeclaration);

	// CW: use collected data and apply array padding transformation 
	// to inputTree
	void transform();
	void transform(const aptScope& currentScope,
		const cacheInfo& cInfo, const List<intra_padding_condition>& ipcList);

	// CW: applies transformation to sage program tree
	void applyTransformation(const arrayDeclInfo& declInfo, const arrayInfo& aInfo);

	// CW: returns size of an array element or -1 if failure occures.
	int getArrayElementSize(SgType *type);

	// CW: they add elements to empty list. If list is empty after call then
	// the functions failed.
	void getArrayDimensionSizes(SgInitializedName *constrName,List<const int>& sizes);
	void getArrayDimensionLowerBounds(SgInitializedName *constrName,List<const int>& bases);
	
	// CW: 
	int getIntegerValueOfExpr(SgExpression *expr,Boolean & valueIsOk);
	unsigned long getArrayBaseAddress();

	aptScope *globalScope;
	aptScope *currentScope;

	// CW: data for statistics
	int totalNoOfArrays;
	int noOfConsideredArrays;
	int noOfArraysPadded;
};

#endif
