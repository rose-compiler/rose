#ifndef _TransformBaseClass_h
#define _TransformBaseClass_h

#include "supported_dimensions.h"

// This class is ment to be a base class for the design of a family of
// transformations.  It is not clear if this is a great design.
// We will add here member functions that are expected to be common to numerous 
// transformations.

#define MAX_NUMBER_OF_SCOPES 100

class ROSE_StatementBlock;

class ROSE_TransformationBase
{
  public:
	
	// This class defines the information associated with a target template
	class TargetTemplate
	{
	  public:
      
		// Use the Sage container class to represent this collection
		SgStatementPtrList targetStatementList;

		~TargetTemplate ();
		TargetTemplate ( SgStatement *targetFunctionStatement );
	};

	// This class defines the information associated with a transformation template
	class TransformationTemplate
	{
	  public:
	
		// Use the Sage container class to represent this collection
		SgStatementPtrList transformationStatementList;
		SgStatementPtrList uniqueStatementList;
		SgStatementPtrList lhsStatementList;
		SgStatementPtrList rhsStatementList;

		/* CW: this design is not very flexible. It only works for
			at most 3 dimensions. Also I need case statements to access them later!
			SgStatementPtrList outerLoopStatementList_1D;
			SgStatementPtrList outerLoopStatementList_2D;
			SgStatementPtrList outerLoopStatementList_3D;
		*/
		
		// CW: loop template for every supported array dimension 
		SgStatementPtrList outerLoopStatementList[ROSE_MAX_ARRAY_DIMENSION];
		
		SgStatementPtrList innerLoopStatementList;

		~TransformationTemplate ();
		TransformationTemplate ( SgStatement *transformationFunctionStatement );

		void collectStatementsFromTransformationBlock ( 
			SgBasicBlock *blockStmt, char* targetStringStart, 
			char* targetStringEnd, SgStatementPtrList & stmtList );

		void collectTransformationTemplateStatements          ( SgBasicBlock *blockStmt );
		void collectUniqueTransformationTemplateStatements    ( SgBasicBlock *blockStmt );
		void collectLhsTransformationTemplateStatements       ( SgBasicBlock *blockStmt );
		void collectRhsTransformationTemplateStatements       ( SgBasicBlock *blockStmt );
		void collectOuterLoopTransformationTemplateStatements ( SgBasicBlock *blockStmt );
		void collectInnerLoopTransformationTemplateStatements ( SgBasicBlock *blockStmt );
	};

  private:
	static TargetTemplate         *targetTemplateData;
	static TransformationTemplate *transformationTemplateData;

	// I don't think we need this because this is the old ROSE I of handling
	// everything at once instead of introducing separate passes for each
	// transformation as Kei has suggested for the improved desing of ROSE II
	// RoseStatement *roseStatement;

	// This is stored so that we can recognize statements from this file
	// while traversing the program tree.  Statements from this file are used
	// in the transformations.
	char* headerFileName;

	// We might want to make these SgFunctionDefinition object later
	// for now we use the SgStatement base class for simplicity
	static SgStatement *targetTemplateFunction;
        static SgStatement *transformationTemplateFunction;

  public:
        static SgVariableDeclaration* subscriptFunctionDeclarationStatement[ROSE_MAX_ARRAY_DIMENSION];
     // CW: Sage-Tree representation of the array subscript function of the inner loop
        static SgFunctionCallExp* subscriptFunctionDeclarationExpression[ROSE_MAX_ARRAY_DIMENSION];

  protected:

	SgFile *programFile;
	// RoseStatement *RoseStatementDataBase;

	// Use the Sage container class to represent this stack of blocks
	// The current block is pushed onto and popped off the stack as we traverse the program tree
	static SgStatementPtrList currentSgBasicBlockStack;

	// Keeps track of the current scope and if the transformation has been 
	// introduced already in that scope
	static SgScopeStatement* transformationIntroducedInScopeArray[MAX_NUMBER_OF_SCOPES];

	void  setTemplateHeaderFileName ( char* name ) { headerFileName = name; }
	char* getTemplateHeaderFileName ()       { return headerFileName; }

  public:
  
	~ROSE_TransformationBase();
	// ROSE_TransformationBase( RoseStatement *roseStmt );
	ROSE_TransformationBase ( SgFile *file );

	static TransformationTemplate* getTransformationTemplateData() 
	{
		ROSE_ASSERT ( transformationTemplateData != NULL);
		return transformationTemplateData; 
	}

	// We might want to make this a pure virtual function at some point!
	ROSE_StatementBlock* pass( ROSE_StatementBlock* roseProgramTree );

	SgFile *getProgramFile();

	static Sg_File_Info* buildSageFileInfo ( char* filename = "default_file_name" );

	// These traverse the statement and COPY it
	static SgStatement*        copy ( SgStatement       *stmt           , SgName oldName, SgName newName );
	static SgExpression*       copy ( SgExpression      *expr           , SgName oldName, SgName newName );
	static SgSymbol*           copy ( SgSymbol          *sym            , SgName oldName, SgName newName );
	static SgType*             copy ( SgType            *type           , SgName oldName, SgName newName );
	static SgInitializedName*  copy ( SgInitializedName *initializedName, SgName oldName, SgName newName );
     // static SgBinaryOp*         copy ( SgBinaryOp        *binaryOp       , SgName oldName, SgName newName );
	static SgInitializer*      copy ( SgInitializer     *expr           , SgName oldName, SgName newName );

#ifdef USE_SAGE3
        static SgPragma*           copy ( SgPragma          *pragma );
#endif

     // The SgStatementPtrList objects are not often referenced through pointers
	static SgStatementPtrList* copy ( SgStatementPtrList & statementList, SgName oldName, SgName newName );

	static SgStatement* buildNullStatement();
	static void makeStatementNullStatement (SgStatement* stmt );

	// Push and Pop function for the current scope (SgBasicBlock objects)
	static void popCurrentScope  ( SgScopeStatement *scopeStatement );
	static void pushCurrentScope ( SgScopeStatement *scopeStatement );
	static SgScopeStatement* getCurrentScope();

	char *transformationHeaderFile();
	void readTargetAndTransformationTemplates ();
	void setupInternalTargetAndTransformationstemplates ( SgStatement::iterator & s_iter );
	static Boolean templatesDataSetup();

	// We need to know if the transformation has been introduced globally
	static Boolean transformationIntroducedInThisScope();

	// We need to know if the transformation has already been introduced in the current scope
	static Boolean transformationIntroducedInGlobalScope();

	// We need to get out the subScript function from the declaration in the header file
	SgFunctionCallExp* extractSubscriptFunctionDeclarationExpression ( SgVariableDeclaration* X );

	// Number of Operands used on the RHS
	int numberOfRhsOperands();

	// traverse the transformation loop structure and return the loop body of the inner most loop
	static SgStatement* getInnerLoopBody ( SgStatement* stmt );

	// Significant magic is done here!
	static void editInnerLoopBodyToMatchCurrentStatement ( SgStatement *innerLoopBody, SgStatement *transformedStatement );

	// The details of the copy depend upon the specific structure of the statement 
	// and what will be edited by the edit mechanism
	// SgStatement* copyLhsStatement ( SgStatement *inputStatement );

	// This function modifes the input SgStatement
	void editLhsSpecificSubTransformation     ( SgStatement *inputStatement, char* pointerName, char* arrayName );
	void editRhsSpecificSubTransformation     ( SgStatement *inputStatement, char* pointerName, char* arrayName );
	void editOperandSpecificSubTransformation ( SgStatement *inputStatement, char* pointerName, char* arrayName );

	 // Devalued function?
	SgExpression* buildLhsOperand( char* arrayName, variant_type_enum arrayType, char* indexName, 
		variant_type_enum indexOperator, SgExpression* rhsExpression );

	void modifySymbol          ( SgSymbol* symbol );
	void modifyInitializedName ( SgInitializedName* initializedName );

	// Declaration of functions that might be built by the compiler
	// are declared here to prevent the compiler from doing so
	// That these are private prevents the user from calling them
  private:
  
	ROSE_TransformationBase();
	ROSE_TransformationBase ( const ROSE_TransformationBase & X );
	ROSE_TransformationBase & operator= ( const ROSE_TransformationBase & X );
};

#endif
