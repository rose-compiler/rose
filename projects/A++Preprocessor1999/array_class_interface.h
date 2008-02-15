#ifndef __array_class_interface_h__
#define __array_class_interface_h__
#include <rose.h>
#include "tlist.h"

// include "Transform_2/grammar.h"
#include "grammar.h"

//------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------
// next three lines define levels of debugging

const int minimal_debug_level = 0;
const int normal_debug_level = 2;
const int verbose_debug_level = 4;

//------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------
// for internal use within the ArrayClassSageInterface class

struct ExpressionFunctionPointer {
  ROSE_GrammarVariants variant;
  Boolean ( *functionPointer )( SgExpression *);
       };

struct TypeFunctionPointer {
  ROSE_GrammarVariants variant;
  Boolean ( *functionPointer )( SgType *);
       };

struct StatementFunctionPointer {
  ROSE_GrammarVariants variant;
  Boolean ( *functionPointer )( SgStatement *);
       };

//------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------

class ArrayClassSageInterface
{
   private:
   
     static const char *getClassName(SgClassDefinition *);
     static Boolean isArrayInternalIndexMemberFunction(const char *);
     static Boolean isArrayInternalIndexFriendFunction(const char *);
     static Boolean isArrayInternalIndexMemberOperator(const char *);
     static Boolean isArrayInternalIndexFriendOperator(const char *);

     static Boolean isFunctionCall(SgStatement *pStatement, SgFunctionCallExp *pFnExp);
     static Boolean isArrayLibGeneralFunction(const char *pString);
     static int getArrayLibGeneralFunctionNumberOfArguments(SgFunctionCallExp *); // General Function refers to Function/Operator
     static Boolean isArrayLibFunction(const char *);			// check if is a member or friend function of an array class
     static Boolean isArrayLibMemberFunction(const char *);		
     static Boolean isArrayClassGeneralFunction(const char *);
     static Boolean isArrayClassFunction(const char *);
     static Boolean isArrayClassOperator(const char *);
     static Boolean isArrayClassMemberFunction(const char *pmemberFnName);
     static Boolean isArraySectionGeneralFunction(const char *);
     static Boolean isArraySectionFunction(const char *);
     static Boolean isArraySectionOperator(const char *);
     static Boolean isArraySectionMemberFunction(const char *pFnSymbol);
     static Boolean isArrayIndexMemberFunction(const char *pFnSymbol);
     static Boolean isArrayRangeMemberFunction(const char *pFnSymbol);
     static Boolean isArrayLibFriendFunction(const char *);
     static Boolean isArrayClassFriendFunction(const char *pfriendFnName);
     static Boolean isArraySectionFriendFunction(const char *pFnSymbol);
     static Boolean isArrayIndexFriendFunction(const char *pFnSymbol);
     static Boolean isArrayRangeFriendFunction(const char *pFnSymbol);
     static Boolean isArrayLibOperator(const char *);
     static Boolean isArrayLibMemberOperator(const char *);
     static Boolean isArrayClassMemberOperator(const char *pOpSymbol);
     static Boolean isArraySectionMemberOperator(const char *pOpSymbol);
     static Boolean isArrayIndexMemberOperator(const char *pOpSymbol);
     static Boolean isArrayRangeMemberOperator(const char *pOpSymbol);
     static Boolean isArrayLibFriendOperator(const char *);
     static Boolean isArrayClassFriendOperator(const char *pOpSymbol);
     static Boolean isArraySectionFriendOperator(const char *pOpSymbol);
     static Boolean isArrayIndexFriendOperator(const char *pOpSymbol);
     static Boolean isArrayRangeFriendOperator(const char *pOpSymbol);


     static Boolean isArrayLibType(const char *);
     static Boolean isArrayClassType(const char *);
     static Boolean isArraySectionType(const char *);

// next set of declarations

     static Boolean isArrayLibGeneralFunction(SgName &String);			// General Function refers to Function/Operator
     static Boolean isArrayLibFunction(SgName &);			// check if is a member or friend function of an array class
     static Boolean isArrayLibMemberFunction(SgName &);		
     static Boolean isArrayClassMemberFunction(SgName &memberFnName);
     static Boolean isArraySectionMemberFunction(SgName &FnSymbol);
     static Boolean isArrayIndexMemberFunction(SgName &FnSymbol);
     static Boolean isArrayRangeMemberFunction(SgName &FnSymbol);
     static Boolean isArrayLibFriendFunction(SgName &);
     static Boolean isArrayClassFriendFunction(SgName &friendFnName);
     static Boolean isArraySectionFriendFunction(SgName &FnSymbol);
     static Boolean isArrayIndexFriendFunction(SgName &FnSymbol);
     static Boolean isArrayRangeFriendFunction(SgName &FnSymbol);
     static Boolean isArrayLibOperator(SgName &);
     static Boolean isArrayLibMemberOperator(SgName &);
     static Boolean isArrayClassMemberOperator(SgName &OpSymbol);
     static Boolean isArraySectionMemberOperator(SgName &OpSymbol);
     static Boolean isArrayIndexMemberOperator(SgName &OpSymbol);
     static Boolean isArrayRangeMemberOperator(SgName &OpSymbol);
     static Boolean isArrayLibFriendOperator(SgName &);
     static Boolean isArrayClassFriendOperator(SgName &OpSymbol);
     static Boolean isArraySectionFriendOperator(SgName &OpSymbol);
     static Boolean isArrayIndexFriendOperator(SgName &OpSymbol);
     static Boolean isArrayRangeFriendOperator(SgName &OpSymbol);
     static Boolean isArrayLibType(SgName &);
     static Boolean isArrayClassType(SgName &);
     static Boolean isArraySectionType(SgName &);

// next set of declarations

     static Boolean isArrayLibGeneralFunction(SgFunctionDeclaration *pString);	// General Function refers to Function/Operator
     static Boolean isArrayLibFunction(SgFunctionDeclaration *);		// check if is a member or friend function of an array class
     static Boolean isArrayLibMemberFunction(SgFunctionDeclaration *);		
     static Boolean isArrayClassGeneralFunction(SgFunctionDeclaration *pFnSymbol);
     static Boolean isArrayClassFunction(SgFunctionDeclaration *pFnSymbol);
     static Boolean isArrayClassOperator(SgFunctionDeclaration *pFnSymbol);
     static Boolean isArrayClassMemberFunction(SgFunctionDeclaration *pFnSymbol);
     static Boolean isArraySectionGeneralFunction(SgFunctionDeclaration *pFnSymbol);
     static Boolean isArraySectionFunction(SgFunctionDeclaration *pFnSymbol);
     static Boolean isArraySectionOperator(SgFunctionDeclaration *pFnSymbol);
     static Boolean isArraySectionMemberFunction(SgFunctionDeclaration *pFnSymbol);
     static Boolean isArrayIndexMemberFunction(SgFunctionDeclaration *pFnSymbol);
     static Boolean isArrayRangeMemberFunction(SgFunctionDeclaration *pFnSymbol);
     static Boolean isArrayLibFriendFunction(SgFunctionDeclaration *);
     static Boolean isArrayClassFriendFunction(SgFunctionDeclaration *pfriendFnName);
     static Boolean isArraySectionFriendFunction(SgFunctionDeclaration *pFnSymbol);
     static Boolean isArrayIndexFriendFunction(SgFunctionDeclaration *pFnSymbol);
     static Boolean isArrayRangeFriendFunction(SgFunctionDeclaration *pFnSymbol);
     static Boolean isArrayLibOperator(SgFunctionDeclaration *);
     static Boolean isArrayLibMemberOperator(SgFunctionDeclaration *);
     static Boolean isArrayClassMemberOperator(SgFunctionDeclaration *pOpSymbol);
     static Boolean isArraySectionMemberOperator(SgFunctionDeclaration *pOpSymbol);
     static Boolean isArrayIndexMemberOperator(SgFunctionDeclaration *pOpSymbol);
     static Boolean isArrayRangeMemberOperator(SgFunctionDeclaration *pOpSymbol);
     static Boolean isArrayLibFriendOperator(SgFunctionDeclaration *);
     static Boolean isArrayClassFriendOperator(SgFunctionDeclaration *pOpSymbol);
     static Boolean isArraySectionFriendOperator(SgFunctionDeclaration *pOpSymbol);
     static Boolean isArrayIndexFriendOperator(SgFunctionDeclaration *pOpSymbol);
     static Boolean isArrayRangeFriendOperator(SgFunctionDeclaration *pOpSymbol);
     static Boolean verifyArraySectionFriendOperatorArguments(SgFunctionDeclaration *pSgFunctionDecl, const char *sectionType );

#if 0	
     static Boolean isArrayLibType(SgFunctionDeclaration *);
     static Boolean isArrayClassType(SgFunctionDeclaration *); 
     static Boolean isArraySectionType(SgFunctionDeclaration *);
#endif

// next set of declarations

     static Boolean isArrayLibGeneralFunction(SgSymbol *pString);	// General Function refers to Function/Operator
     static Boolean isArrayLibFunction(SgSymbol *);		// check if is a member or friend function of an array class
     static Boolean isArrayLibMemberFunction(SgSymbol *);		
     static Boolean isArrayClassGeneralFunction(SgSymbol *);
     static Boolean isArrayClassFunction(SgSymbol *);
     static Boolean isArrayClassOperator(SgSymbol *);
     static Boolean isArrayClassMemberFunction(SgSymbol *);
     static Boolean isArraySectionGeneralFunction(SgSymbol *);
     static Boolean isArraySectionFunction(SgSymbol *);
     static Boolean isArraySectionOperator(SgSymbol *);
     static Boolean isArraySectionMemberFunction(SgSymbol *pFnSymbol);
     static Boolean isArrayIndexMemberFunction(SgSymbol *pFnSymbol);
     static Boolean isArrayRangeMemberFunction(SgSymbol *pFnSymbol);
     static Boolean isArrayLibFriendFunction(SgSymbol *);
     static Boolean isArrayClassFriendFunction(SgSymbol *pfriendFnName);
     static Boolean isArraySectionFriendFunction(SgSymbol *pFnSymbol);
     static Boolean isArrayIndexFriendFunction(SgSymbol *pFnSymbol);
     static Boolean isArrayRangeFriendFunction(SgSymbol *pFnSymbol);
     static Boolean isArrayLibOperator(SgSymbol *);
     static Boolean isArrayLibMemberOperator(SgSymbol *);
     static Boolean isArrayClassMemberOperator(SgSymbol *pOpSymbol);
     static Boolean isArraySectionMemberOperator(SgSymbol *pOpSymbol);
     static Boolean isArrayIndexMemberOperator(SgSymbol *pOpSymbol);
     static Boolean isArrayRangeMemberOperator(SgSymbol *pOpSymbol);
     static Boolean isArrayLibFriendOperator(SgSymbol *);
     static Boolean isArrayClassFriendOperator(SgSymbol *pOpSymbol);
     static Boolean isArraySectionFriendOperator(SgSymbol *pOpSymbol);
     static Boolean isArrayIndexFriendOperator(SgSymbol *pOpSymbol);
     static Boolean isArrayRangeFriendOperator(SgSymbol *pOpSymbol);

#if 0
     static Boolean isArrayLibType(SgSymbol *);
     static Boolean isArrayClassType(SgSymbol *); 
     static Boolean isArraySectionType(SgSymbol *);
#endif

// next set of declarations

     static Boolean isArrayLibGeneralFunction( SgExpression * pString);	// General Function refers to Function/Operator
     static Boolean isArrayLibFunction( SgExpression * );		// check if is a member or friend function of an array class
     static Boolean isArrayLibMemberFunction( SgExpression * );		
     static Boolean isArrayClassGeneralFunction(SgExpression *);
     static Boolean isArrayClassFunction(SgExpression *);
     static Boolean isArrayClassOperator(SgExpression *);
     static Boolean isArrayClassMemberFunction(SgExpression *pmemberFnName);
     static Boolean isArraySectionGeneralFunction(SgExpression *);
     static Boolean isArraySectionFunction(SgExpression *);
     static Boolean isArraySectionOperator(SgExpression *);
     static Boolean isArraySectionMemberFunction( SgExpression * pFnSymbol);
     static Boolean isArrayIndexMemberFunction( SgExpression * pFnSymbol);
     static Boolean isArrayRangeMemberFunction( SgExpression * pFnSymbol);
     static Boolean isArrayLibFriendFunction( SgExpression * );
     static Boolean isArrayClassFriendFunction( SgExpression * pfriendFnName );
     static Boolean isArraySectionFriendFunction( SgExpression * pFnSymbol);
     static Boolean isArrayIndexFriendFunction( SgExpression * pFnSymbol);
     static Boolean isArrayRangeFriendFunction( SgExpression * pFnSymbol);
     static Boolean isArrayLibOperator( SgExpression * );
     static Boolean isArrayLibMemberOperator( SgExpression * );
     static Boolean isArrayClassMemberOperator( SgExpression * pOpSymbol);
     static Boolean isArraySectionMemberOperator( SgExpression * pOpSymbol);
     static Boolean isArrayIndexMemberOperator( SgExpression * pOpSymbol);
     static Boolean isArrayRangeMemberOperator( SgExpression * pOpSymbol);
     static Boolean isArrayLibFriendOperator( SgExpression * );
     static Boolean isArrayClassFriendOperator( SgExpression * pOpSymbol);
     static Boolean isArraySectionFriendOperator( SgExpression * pOpSymbol );
     static Boolean isArrayIndexFriendOperator( SgExpression * pOpSymbol );
     static Boolean isArrayRangeFriendOperator( SgExpression * pOpSymbol );

#if 0
     static Boolean isArrayLibType( SgExpression * );
     static Boolean isArrayClassType( SgExpression * ); 
     static Boolean isArraySectionType( SgExpression * );
#endif

 private:
     static Boolean isROSEWhereStatement( SgStatement *, const char *);
     static Boolean isROSEIndexingVariableExpression( SgExpression *, const char * );
     static Boolean isROSEIndexingVariableReferenceExpression( SgExpression *, const char * );
     static Boolean isROSEIndexingVariablePointerExpression( SgExpression *, const char * );
     static Boolean isROSEArrayVariableExpression( SgExpression *, const char * );
     static Boolean isROSEArrayVariableReferenceExpression( SgExpression *, const char * );
     static Boolean isROSEArrayVariablePointerExpression( SgExpression *, const char * );
     
     static Boolean verifyArrayBinaryFriendOperator( SgExpression *pExpression, const char *sOperatorName);
     static Boolean verifyArrayBinaryMemberOperator( SgExpression *pExpression, const char *sOperatorName);
     static Boolean verifyArrayClassFriendFunction( SgExpression *pExpression, const char *sOperatorName);
     static Boolean verifyArgumentsForArrayBinaryFriendOperator ( SgExpression *pExpression );
     static Boolean verifyTwoArgumentsForArrayFriendFunction ( SgExpression *pExpression );
     static Boolean verifyArgumentsForArrayFriendFunction ( SgExpression *pExpression );
     static Boolean verifySingleArgumentForArrayFriendFunction ( SgExpression *pExpression );

 protected:
     // we make some general functions which don't ask array grammar specific questions public
 public:
     static Boolean isArrayLibType( SgType * );
     static Boolean isArrayClassType( SgType * ); 
     static Boolean isArraySectionType( SgType * );

     static SgType *getType(SgExpression *);
     static SgType *getType(SgType *);

 public:
     static int bDebug;

     ArrayClassSageInterface(void);
    ~ArrayClassSageInterface(void);

//     temp code
        void Identify_Variant(int i);
///////////////////////////////////////////
	static Boolean isROSENode ( SgNode *pNode );

	static Boolean isROSEIndexArgumentList ( SgExpression *pExpression );

	static Boolean isROSEIndexExpression ( SgExpression *pExpression );
	static Boolean isROSEIndexExpressionBinaryOperatorCExpression ( SgExpression *pExpression );
	static Boolean isROSECExpressionBinaryOperatorIndexExpression ( SgExpression *pExpression );
//	static Boolean isROSEUnaryOperatorIndexOperand ( SgExpression *pExpression );
	static Boolean isROSEIndexOperand ( SgExpression *pExpression );

	static Boolean isROSEIndexingVariable ( SgExpression *pExpression );
	static Boolean isROSEIndexVariable ( SgExpression *pExpression );
	static Boolean isROSERangeVariable ( SgExpression *pExpression );
	static Boolean isROSEIndexingVariableReference ( SgExpression *pExpression );
	static Boolean isROSEIndexVariableReference ( SgExpression *pExpression );
	static Boolean isROSERangeVariableReference ( SgExpression *pExpression );
	static Boolean isROSEIndexingVariablePointer ( SgExpression *pExpression );
	static Boolean isROSEIndexVariablePointer ( SgExpression *pExpression );
	static Boolean isROSERangeVariablePointer ( SgExpression *pExpression );
        static Boolean isROSEIndexingVariableDereferencedPointer( SgExpression *pExpression );
        static Boolean isROSEIndexVariableDereferencedPointer( SgExpression *pExpression );
        static Boolean isROSERangeVariableDereferencedPointer( SgExpression *pExpression );

	static Boolean isROSEType ( SgExpression *pExpression );
	static Boolean isROSEArrayType ( SgExpression *pExpression );
	static Boolean isROSERealArrayType ( SgExpression *pExpression );
	static Boolean isROSEdoubleArrayType ( SgExpression *pExpression );
	static Boolean isROSEfloatArrayType ( SgExpression *pExpression );
	static Boolean isROSEintArrayType ( SgExpression *pExpression );
	static Boolean isROSEIndexingType ( SgExpression *pExpression );
        static Boolean isROSEInternal_IndexType ( SgExpression *pExpression );
	static Boolean isROSEIndexType ( SgExpression *pExpression );
	static Boolean isROSERangeType ( SgExpression *pExpression );

	static Boolean isROSEType ( SgType *pType );
	static Boolean isROSEArrayType ( SgType *pType );
	static Boolean isROSERealArrayType ( SgType *pType );
	static Boolean isROSEdoubleArrayType ( SgType *pType );
	static Boolean isROSEfloatArrayType ( SgType *pType );
	static Boolean isROSEintArrayType ( SgType *pType );
	static Boolean isROSEIndexingType ( SgType *pType );
        static Boolean isROSEInternal_IndexType ( SgType *pType );
	static Boolean isROSEIndexType ( SgType *pType );
	static Boolean isROSERangeType ( SgType *pType );

	static Boolean isROSEStatement ( SgStatement *pStatement );
	static Boolean isROSEVariableDeclaration ( SgStatement *pStatement );
	static Boolean isROSEC_VariableDeclaration ( SgStatement *pStatement );
	static Boolean isROSEArrayVariableDeclaration ( SgStatement *pStatement );
        static Boolean isROSEIndexingVariableDeclaration( SgStatement *pStatement );
	static Boolean isROSEStatementBlock ( SgStatement *pStatement );
	static Boolean isROSEC_Statement ( SgStatement *pStatement );
	static Boolean isROSEArrayStatement ( SgStatement *pStatement );
	static Boolean isROSEExpressionStatement ( SgStatement *pStatement );
	static Boolean isROSEWhereStatement ( SgStatement *pStatement );
	static Boolean isROSEElseWhereStatement ( SgStatement *pStatement );
	static Boolean isROSEDoWhileStatement ( SgStatement *pStatement );
	static Boolean isROSEWhileStatement ( SgStatement *pStatement );
	static Boolean isROSEForStatement ( SgStatement *pStatement );
	static Boolean isROSEIfStatement ( SgStatement *pStatement );
	static Boolean isROSEReturnStatement ( SgStatement *pStatement );

	static Boolean isROSEExpression ( SgExpression *pExpression );
	static Boolean isROSEC_Expression ( SgExpression *pExpression );
//	static Boolean isROSEUserFunction ( SgExpression *pExpression );
	static Boolean isROSEArrayExpression ( SgExpression *pExpression );

	static Boolean isROSEArrayOperator ( SgExpression *pExpression );

	static Boolean isROSEUnaryArrayOperator ( SgExpression *pExpression );
	static Boolean isROSEUnaryArrayOperatorMinus ( SgExpression *pExpression );
	static Boolean isROSEUnaryArrayOperatorPlus ( SgExpression *pExpression );
	static Boolean isROSEUnaryArrayOperatorPrefixPlusPlus ( SgExpression *pExpression );
	static Boolean isROSEUnaryArrayOperatorPostfixPlusPlus ( SgExpression *pExpression );
	static Boolean isROSEUnaryArrayOperatorPrefixMinusMinus ( SgExpression *pExpression );
	static Boolean isROSEUnaryArrayOperatorPostfixMinusMinus ( SgExpression *pExpression );
	static Boolean isROSEUnaryArrayLogicalOperator ( SgExpression *pExpression );
	static Boolean isROSEUnaryArrayOperatorNOT ( SgExpression *pExpression );

	static Boolean isROSEBinaryArrayOperator ( SgExpression *pExpression );

	static Boolean isROSEBinaryArrayOperatorEquals ( SgExpression *pExpression );
	static Boolean isROSEBinaryArrayOperatorAdd ( SgExpression *pExpression );
	static Boolean isROSEBinaryArrayOperatorAddEquals ( SgExpression *pExpression );
	static Boolean isROSEBinaryArrayOperatorMinus ( SgExpression *pExpression );
	static Boolean isROSEBinaryArrayOperatorMinusEquals ( SgExpression *pExpression );
	static Boolean isROSEBinaryArrayOperatorMultiply ( SgExpression *pExpression );
	static Boolean isROSEBinaryArrayOperatorMultiplyEquals ( SgExpression *pExpression );
	static Boolean isROSEBinaryArrayOperatorDivide ( SgExpression *pExpression );
	static Boolean isROSEBinaryArrayOperatorDivideEquals ( SgExpression *pExpression );
	static Boolean isROSEBinaryArrayOperatorModulo ( SgExpression *pExpression );
	static Boolean isROSEBinaryArrayOperatorModuloEquals ( SgExpression *pExpression );
	static Boolean isROSEBinaryArrayTestingOperator ( SgExpression *pExpression );
	static Boolean isROSEBinaryArrayOperatorLT ( SgExpression *pExpression );
	static Boolean isROSEBinaryArrayOperatorLTEquals ( SgExpression *pExpression );
	static Boolean isROSEBinaryArrayOperatorGT ( SgExpression *pExpression );
	static Boolean isROSEBinaryArrayOperatorGTEquals ( SgExpression *pExpression );
	static Boolean isROSEBinaryArrayOperatorEquivalence ( SgExpression *pExpression );
	static Boolean isROSEBinaryArrayOperatorNOTEquals ( SgExpression *pExpression );
	static Boolean isROSEBinaryArrayLogicalOperator ( SgExpression *pExpression );
	static Boolean isROSEBinaryArrayOperatorLogicalAND ( SgExpression *pExpression );
	static Boolean isROSEBinaryArrayOperatorLogicalOR ( SgExpression *pExpression );

	static Boolean isROSEArrayOperandExpression ( SgExpression *pExpression );

	static Boolean isROSEArrayVariableExpression ( SgExpression *pExpression );
	static Boolean isROSERealArrayVariableExpression ( SgExpression *pExpression );
	static Boolean isROSEdoubleArrayVariableExpression ( SgExpression *pExpression );
	static Boolean isROSEfloatArrayVariableExpression ( SgExpression *pExpression );
	static Boolean isROSEintArrayVariableExpression ( SgExpression *pExpression );
	static Boolean isROSEArrayVariableReferenceExpression ( SgExpression *pExpression );
	static Boolean isROSERealArrayVariableReferenceExpression ( SgExpression *pExpression );
	static Boolean isROSEdoubleArrayVariableReferenceExpression ( SgExpression *pExpression );
	static Boolean isROSEfloatArrayVariableReferenceExpression ( SgExpression *pExpression );
	static Boolean isROSEintArrayVariableReferenceExpression ( SgExpression *pExpression );
	static Boolean isROSEArrayVariablePointerExpression ( SgExpression *pExpression );
	static Boolean isROSERealArrayVariablePointerExpression ( SgExpression *pExpression );
	static Boolean isROSEdoubleArrayVariablePointerExpression ( SgExpression *pExpression );
	static Boolean isROSEfloatArrayVariablePointerExpression ( SgExpression *pExpression );
	static Boolean isROSEintArrayVariablePointerExpression ( SgExpression *pExpression );
        static Boolean isROSEArrayVariableDereferencedPointerExpression( SgExpression *pExpression );
        static Boolean isROSERealArrayVariableDereferencedPointerExpression( SgExpression *pExpression );
        static Boolean isROSEdoubleArrayVariableDereferencedPointerExpression( SgExpression *pExpression );
        static Boolean isROSEfloatArrayVariableDereferencedPointerExpression( SgExpression *pExpression );
        static Boolean isROSEintArrayVariableDereferencedPointerExpression( SgExpression *pExpression );

	static Boolean isROSEArrayParenthesisOperator ( SgExpression *pExpression );

	static Boolean isROSEArrayFunctionExpression ( SgExpression *pExpression );
	static Boolean isROSEArrayCOS ( SgExpression *pExpression );
	static Boolean isROSEArraySIN ( SgExpression *pExpression );
	static Boolean isROSEArrayTAN ( SgExpression *pExpression );
	static Boolean isROSEArrayMIN ( SgExpression *pExpression );
	static Boolean isROSEArrayMAX ( SgExpression *pExpression );
	static Boolean isROSEArrayFMOD ( SgExpression *pExpression );
	static Boolean isROSEArrayMOD ( SgExpression *pExpression );
	static Boolean isROSEArrayPOW ( SgExpression *pExpression );
	static Boolean isROSEArraySIGN ( SgExpression *pExpression );
	static Boolean isROSEArrayLOG ( SgExpression *pExpression );
	static Boolean isROSEArrayLOG10 ( SgExpression *pExpression );
	static Boolean isROSEArrayEXP ( SgExpression *pExpression );
	static Boolean isROSEArraySQRT ( SgExpression *pExpression );
	static Boolean isROSEArrayFABS ( SgExpression *pExpression );
	static Boolean isROSEArrayCEIL ( SgExpression *pExpression );
	static Boolean isROSEArrayFLOOR ( SgExpression *pExpression );
	static Boolean isROSEArrayABS ( SgExpression *pExpression );
	static Boolean isROSEArrayTRANSPOSE ( SgExpression *pExpression );
	static Boolean isROSEArrayACOS ( SgExpression *pExpression );
	static Boolean isROSEArrayASIN ( SgExpression *pExpression );
	static Boolean isROSEArrayATAN ( SgExpression *pExpression );
	static Boolean isROSEArrayCOSH ( SgExpression *pExpression );
	static Boolean isROSEArraySINH ( SgExpression *pExpression );
	static Boolean isROSEArrayTANH ( SgExpression *pExpression );
	static Boolean isROSEArrayACOSH ( SgExpression *pExpression );
	static Boolean isROSEArrayASINH ( SgExpression *pExpression );
	static Boolean isROSEArrayATANH ( SgExpression *pExpression );

	static SgFunctionDeclaration  *getArrayMemberFunctionDeclaration ( SgMemberFunctionRefExp *pMemberFunctionReference );
	static SgFunctionDeclaration  *getArrayMemberFunctionDeclaration ( SgFunctionCallExp * );
	static SgFunctionDeclaration  *getArrayFriendFunctionDeclaration ( SgFunctionRefExp *pFunctionRefExp );
	static SgMemberFunctionRefExp *getArrayClassMemberOperator( SgExpression *pSgExpression);
	static SgFunctionRefExp       *getArrayClassFriendOperator( SgExpression *pExpression);
	static SgFunctionRefExp       *getArrayClassFriendFunction( SgExpression *pExpression);

	static ROSE_GrammarVariants getVariant( SgExpression *pExpression);
        static ROSE_GrammarVariants getVariant( SgStatement *pStatement);
        static ROSE_GrammarVariants getVariant( SgType *pType);
};

//------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------

#endif
