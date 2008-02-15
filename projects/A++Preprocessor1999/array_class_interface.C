#ifdef HAVE_CONFIG_H
// This avoids requiring the user to use config.h and follows 
// the automake manual request that we use <> instead of ""
#include <config.h>
#endif

#include "array_class_interface.h"

int ArrayClassSageInterface::bDebug = normal_debug_level; // verbose_debug_level; // normal_debug_level;

//----------------------------------------------------------------------------------------------------------------------------------

// the enumerations below are used within the getVariant function since I don't want to write an interminable no. of else - if's
// it's not meant to be used elsewhere.

//----------------------------------------------------------------------------------------------------------------------------------
const int n_arrayFriendFunctions =      27;
const int n_arrayFriendOperators =      14;
const int n_arrayMemberOperators =      14;
const int n_statementFunctions =        13;
const int n_arrayVariableInstances =    15;
const int n_arrayDereferencedVariables = 5;
const int n_typeFunctions =              5;
const int n_indexExpressions =           2;

ExpressionFunctionPointer arrayFriendFunctions[ n_arrayFriendFunctions ] = {
     { ROSE_ArrayCOSTag,       &ArrayClassSageInterface::isROSEArrayCOS       },
     { ROSE_ArraySINTag,       &ArrayClassSageInterface::isROSEArraySIN       },
     { ROSE_ArrayTANTag,       &ArrayClassSageInterface::isROSEArrayTAN       },
     { ROSE_ArrayMINTag,       &ArrayClassSageInterface::isROSEArrayMIN       },
     { ROSE_ArrayMAXTag,       &ArrayClassSageInterface::isROSEArrayMAX       },
     { ROSE_ArrayFMODTag,      &ArrayClassSageInterface::isROSEArrayFMOD      },
     { ROSE_ArrayMODTag,       &ArrayClassSageInterface::isROSEArrayMOD       },
     { ROSE_ArrayPOWTag,       &ArrayClassSageInterface::isROSEArrayPOW       },
     { ROSE_ArraySIGNTag,      &ArrayClassSageInterface::isROSEArraySIGN      },
     { ROSE_ArrayLOGTag,       &ArrayClassSageInterface::isROSEArrayLOG       },
     { ROSE_ArrayLOG10Tag,     &ArrayClassSageInterface::isROSEArrayLOG10     },
     { ROSE_ArrayEXPTag,       &ArrayClassSageInterface::isROSEArrayEXP       },
     { ROSE_ArraySQRTTag,      &ArrayClassSageInterface::isROSEArraySQRT      },
     { ROSE_ArrayFABSTag,      &ArrayClassSageInterface::isROSEArrayFABS      },
     { ROSE_ArrayCEILTag,      &ArrayClassSageInterface::isROSEArrayCEIL      },
     { ROSE_ArrayFLOORTag,     &ArrayClassSageInterface::isROSEArrayFLOOR     },
     { ROSE_ArrayABSTag,       &ArrayClassSageInterface::isROSEArrayABS       },
     { ROSE_ArrayTRANSPOSETag, &ArrayClassSageInterface::isROSEArrayTRANSPOSE },
     { ROSE_ArrayACOSTag,      &ArrayClassSageInterface::isROSEArrayACOS      },
     { ROSE_ArrayASINTag,      &ArrayClassSageInterface::isROSEArrayASIN      },
     { ROSE_ArrayATANTag,      &ArrayClassSageInterface::isROSEArrayATAN      },
     { ROSE_ArrayCOSHTag,      &ArrayClassSageInterface::isROSEArrayCOSH      },
     { ROSE_ArraySINHTag,      &ArrayClassSageInterface::isROSEArraySINH      },
     { ROSE_ArrayTANHTag,      &ArrayClassSageInterface::isROSEArrayTANH      },
     { ROSE_ArrayACOSHTag,     &ArrayClassSageInterface::isROSEArrayACOSH     },
     { ROSE_ArrayASINHTag,     &ArrayClassSageInterface::isROSEArrayASINH     },
     { ROSE_ArrayATANHTag,     &ArrayClassSageInterface::isROSEArrayATANH     }
     };

ExpressionFunctionPointer arrayFriendOperators[ n_arrayFriendOperators ] = {
     { ROSE_BinaryArrayOperatorAddTag, &ArrayClassSageInterface::isROSEBinaryArrayOperatorAdd                 },
     { ROSE_BinaryArrayOperatorMinusTag, &ArrayClassSageInterface::isROSEBinaryArrayOperatorMinus             },
     { ROSE_BinaryArrayOperatorMultiplyTag, &ArrayClassSageInterface::isROSEBinaryArrayOperatorMultiply       },
     { ROSE_BinaryArrayOperatorDivideTag, &ArrayClassSageInterface::isROSEBinaryArrayOperatorDivide           },
     { ROSE_BinaryArrayOperatorModuloTag, &ArrayClassSageInterface::isROSEBinaryArrayOperatorModulo           },
     { ROSE_BinaryArrayTestingOperatorTag, &ArrayClassSageInterface::isROSEBinaryArrayTestingOperator         },
     { ROSE_BinaryArrayOperatorLTTag, &ArrayClassSageInterface::isROSEBinaryArrayOperatorLT                   },
     { ROSE_BinaryArrayOperatorLTEqualsTag, &ArrayClassSageInterface::isROSEBinaryArrayOperatorLTEquals       },
     { ROSE_BinaryArrayOperatorGTTag, &ArrayClassSageInterface::isROSEBinaryArrayOperatorGT                   },
     { ROSE_BinaryArrayOperatorGTEqualsTag, &ArrayClassSageInterface::isROSEBinaryArrayOperatorGTEquals       },
     { ROSE_BinaryArrayOperatorEquivalenceTag, &ArrayClassSageInterface::isROSEBinaryArrayOperatorEquivalence },
     { ROSE_BinaryArrayOperatorNOTEqualsTag, &ArrayClassSageInterface::isROSEBinaryArrayOperatorNOTEquals     },
     { ROSE_BinaryArrayOperatorLogicalANDTag, &ArrayClassSageInterface::isROSEBinaryArrayOperatorLogicalAND   },
     { ROSE_BinaryArrayOperatorLogicalORTag, &ArrayClassSageInterface::isROSEBinaryArrayOperatorLogicalOR     }
};

ExpressionFunctionPointer arrayMemberOperators[ n_arrayMemberOperators ] = {
     { ROSE_BinaryArrayOperatorEqualsTag, &ArrayClassSageInterface::isROSEBinaryArrayOperatorEquals                     },
     { ROSE_ArrayParenthesisOperatorTag, &ArrayClassSageInterface::isROSEArrayParenthesisOperator                       },
     { ROSE_BinaryArrayOperatorAddEqualsTag, &ArrayClassSageInterface::isROSEBinaryArrayOperatorAddEquals               },
     { ROSE_BinaryArrayOperatorMinusEqualsTag, &ArrayClassSageInterface::isROSEBinaryArrayOperatorMinusEquals           },
     { ROSE_BinaryArrayOperatorMultiplyEqualsTag, &ArrayClassSageInterface::isROSEBinaryArrayOperatorMultiplyEquals     },
     { ROSE_BinaryArrayOperatorDivideEqualsTag, &ArrayClassSageInterface::isROSEBinaryArrayOperatorDivideEquals         },
     { ROSE_BinaryArrayOperatorModuloEqualsTag, &ArrayClassSageInterface::isROSEBinaryArrayOperatorModuloEquals         },
     { ROSE_UnaryArrayOperatorMinusTag, &ArrayClassSageInterface::isROSEUnaryArrayOperatorMinus                         },
     { ROSE_UnaryArrayOperatorPlusTag, &ArrayClassSageInterface::isROSEUnaryArrayOperatorPlus                           },
     { ROSE_UnaryArrayOperatorPrefixPlusPlusTag, &ArrayClassSageInterface::isROSEUnaryArrayOperatorPrefixPlusPlus       },
     { ROSE_UnaryArrayOperatorPostfixPlusPlusTag, &ArrayClassSageInterface::isROSEUnaryArrayOperatorPostfixPlusPlus     },
     { ROSE_UnaryArrayOperatorPrefixMinusMinusTag, &ArrayClassSageInterface::isROSEUnaryArrayOperatorPrefixMinusMinus   },
     { ROSE_UnaryArrayOperatorPostfixMinusMinusTag, &ArrayClassSageInterface::isROSEUnaryArrayOperatorPostfixMinusMinus },
     { ROSE_UnaryArrayOperatorNOTTag, &ArrayClassSageInterface::isROSEUnaryArrayOperatorNOT },
};

// note that the enum below does not contain entries for dereferenced pointers - those are handled seperately
ExpressionFunctionPointer arrayVariableInstances[ n_arrayVariableInstances ] = {
     { ROSE_doubleArrayVariableExpressionTag, &ArrayClassSageInterface::isROSEdoubleArrayVariableExpression                   },
     { ROSE_floatArrayVariableExpressionTag, &ArrayClassSageInterface::isROSEfloatArrayVariableExpression                     },
     { ROSE_intArrayVariableExpressionTag, &ArrayClassSageInterface::isROSEintArrayVariableExpression                         },
     { ROSE_doubleArrayVariableReferenceExpressionTag, &ArrayClassSageInterface::isROSEdoubleArrayVariableReferenceExpression },
     { ROSE_floatArrayVariableReferenceExpressionTag, &ArrayClassSageInterface::isROSEfloatArrayVariableReferenceExpression   },
     { ROSE_intArrayVariableReferenceExpressionTag, &ArrayClassSageInterface::isROSEintArrayVariableReferenceExpression       },
     { ROSE_doubleArrayVariablePointerExpressionTag, &ArrayClassSageInterface::isROSEdoubleArrayVariablePointerExpression     },
     { ROSE_floatArrayVariablePointerExpressionTag, &ArrayClassSageInterface::isROSEfloatArrayVariablePointerExpression       },
     { ROSE_intArrayVariablePointerExpressionTag, &ArrayClassSageInterface::isROSEintArrayVariablePointerExpression           },
     { ROSE_IndexVariableTag, &ArrayClassSageInterface::isROSEIndexVariable                                                   },
     { ROSE_RangeVariableTag, &ArrayClassSageInterface::isROSERangeVariable                                                   },
     { ROSE_IndexVariableReferenceTag, &ArrayClassSageInterface::isROSEIndexVariableReference                                 },
     { ROSE_RangeVariableReferenceTag, &ArrayClassSageInterface::isROSERangeVariableReference                                 },
     { ROSE_IndexVariablePointerTag, &ArrayClassSageInterface::isROSEIndexVariablePointer                                     },
     { ROSE_RangeVariablePointerTag, &ArrayClassSageInterface::isROSERangeVariablePointer                                     }
};

// handling of dereferenced pointers 
ExpressionFunctionPointer arrayDereferencedVariables[ n_arrayDereferencedVariables ] = {
     { ROSE_IndexVariableDereferencedPointerTag, &ArrayClassSageInterface::isROSEIndexVariableDereferencedPointer                                 },
     { ROSE_RangeVariableDereferencedPointerTag, &ArrayClassSageInterface::isROSERangeVariableDereferencedPointer                                 },
     { ROSE_doubleArrayVariableDereferencedPointerExpressionTag, &ArrayClassSageInterface::isROSEdoubleArrayVariableDereferencedPointerExpression },
     { ROSE_floatArrayVariableDereferencedPointerExpressionTag, &ArrayClassSageInterface::isROSEfloatArrayVariableDereferencedPointerExpression   },
     { ROSE_intArrayVariableDereferencedPointerExpressionTag, &ArrayClassSageInterface::isROSEintArrayVariableDereferencedPointerExpression       },
};

ExpressionFunctionPointer indexExpressions [ n_indexExpressions ] = {
     { ROSE_IndexExpressionBinaryOperatorCExpressionTag, &ArrayClassSageInterface::isROSEIndexExpressionBinaryOperatorCExpression },
     { ROSE_CExpressionBinaryOperatorIndexExpressionTag, &ArrayClassSageInterface::isROSECExpressionBinaryOperatorIndexExpression }
};

TypeFunctionPointer typeFunctions[ n_typeFunctions ] = {
     { ROSE_doubleArrayTypeTag, &ArrayClassSageInterface::isROSEdoubleArrayType },
     { ROSE_floatArrayTypeTag, &ArrayClassSageInterface::isROSEfloatArrayType },
     { ROSE_intArrayTypeTag, &ArrayClassSageInterface::isROSEintArrayType },
     { ROSE_IndexTypeTag, &ArrayClassSageInterface::isROSEIndexType },
     { ROSE_RangeTypeTag, &ArrayClassSageInterface::isROSERangeType }
};

StatementFunctionPointer statementFunctions[ n_statementFunctions ] = {
     { ROSE_C_VariableDeclarationTag,       &ArrayClassSageInterface::isROSEC_VariableDeclaration       },
     { ROSE_ArrayVariableDeclarationTag,    &ArrayClassSageInterface::isROSEArrayVariableDeclaration    },
     { ROSE_IndexingVariableDeclarationTag, &ArrayClassSageInterface::isROSEIndexingVariableDeclaration },
     { ROSE_StatementBlockTag,              &ArrayClassSageInterface::isROSEStatementBlock              },
     { ROSE_C_StatementTag,                 &ArrayClassSageInterface::isROSEC_Statement                 },
     // the next line will be put in when changes have been made elsewhere - and we remember!
     //     { ROSE_IndexingStatementTag,                 &ArrayClassSageInterface::isROSEIndexingStatement                 },
     { ROSE_ExpressionStatementTag,         &ArrayClassSageInterface::isROSEExpressionStatement         },
     { ROSE_ReturnStatementTag,             &ArrayClassSageInterface::isROSEReturnStatement             },
     { ROSE_WhereStatementTag,              &ArrayClassSageInterface::isROSEWhereStatement              },
     { ROSE_ElseWhereStatementTag,          &ArrayClassSageInterface::isROSEElseWhereStatement          },
     { ROSE_DoWhileStatementTag,            &ArrayClassSageInterface::isROSEDoWhileStatement            },
     { ROSE_WhileStatementTag,              &ArrayClassSageInterface::isROSEWhileStatement              },
     { ROSE_ForStatementTag,                &ArrayClassSageInterface::isROSEForStatement                },
     { ROSE_IfStatementTag,                 &ArrayClassSageInterface::isROSEIfStatement                 }
};

ArrayClassSageInterface::ArrayClassSageInterface()
{
}

ArrayClassSageInterface::~ArrayClassSageInterface()
{
}


// -----------------------------------------------------------------------------------------------------
// utility functions which should probably go in the ROSE class
// -----------------------------------------------------------------------------------------------------
const char * ArrayClassSageInterface::getClassName ( SgClassDefinition *pclassDefinition) 
{
  ROSE_ASSERT( pclassDefinition != NULL );

  SgClassDeclaration *pclassDeclaration = isSgClassDeclaration( pclassDefinition->get_declaration() );
  ROSE_ASSERT ( pclassDeclaration != NULL );

  SgName sageClassName = pclassDeclaration->get_name();

  printf ("In ArrayClassSageInterface::getClassName(): sageClassName = %s \n", sageClassName.str() );

  ROSE_ASSERT(sageClassName.str() != NULL);
  return strdup(sageClassName.str());
}

// could convert this into a proper interface function at some point if required.
// currently it's here purely for testing the code in this file/
Boolean ArrayClassSageInterface::isFunctionCall(SgStatement *pStatement, SgFunctionCallExp *pFnExp)
{
  Boolean bretValue = FALSE;
  SgExpression *pExp = NULL;
  pFnExp = NULL;

  ROSE_ASSERT ( pStatement != NULL );
  SgExprStatement *pExprStatement = isSgExprStatement(pStatement);
  if( pExprStatement != NULL )    // it is an expression statement
    {
      pExp = isSgFunctionCallExp ( pExprStatement->get_the_expr() );
      if(pExp != NULL)
	{
	  pFnExp = (SgFunctionCallExp *)pExp;
	  bretValue = TRUE;
	}
    }
  return bretValue;
}
// -----------------------------------------------------------------------------------------------------
// functions to get specific things - as ambiguous as possible!
// -----------------------------------------------------------------------------------------------------

// note that we assume that checks are already in place to ensure that this is an A++ library function
// the number of arguments will be determined from the ExprListExp class
// probably need to add code to take case of SgModifier, SgReference cases

int ArrayClassSageInterface::getArrayLibGeneralFunctionNumberOfArguments( SgFunctionCallExp *pSgFunctionCallExp)
{
  int iretValue = -1;   
  
  if ( bDebug == verbose_debug_level )
    printf ("Entering ArrayClassSageInterface::getArrayLibGeneralFunctionNumberOfArguments \n");

  ROSE_ASSERT ( pSgFunctionCallExp != NULL );

  SgExprListExp *pExprList = pSgFunctionCallExp->get_args();

  ROSE_ASSERT ( pExprList != NULL );
  // may need to add code for 0 no of args - check!!!
  SgExpressionPtrList expressionPtrList = pExprList->get_expressions();

  iretValue = expressionPtrList.size();

  if ( bDebug == verbose_debug_level )
    printf ("Exiting ArrayClassSageInterface::getArrayLibGeneralFunctionNumberOfArguments \n");

  return iretValue;
}

// -----------------------------------------------------------------------------------------------------
// first come the function definitions for all the 'isArray' functions with const char * args
// -----------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isArrayLibGeneralFunction(const char *pFnString)
{	
	Boolean  bretValue = FALSE;

     	if (bDebug == verbose_debug_level)
        	  printf ("Inside of ArrayClassSageInterface::isArrayLibGeneralFunction \n");

        ROSE_ASSERT( pFnString != NULL );
     	
	if(!(bretValue = isArrayLibFunction(pFnString)))		// check whether it is a function
		bretValue  = isArrayLibOperator(pFnString);

     	if (bDebug == verbose_debug_level)
        	  printf ("Exiting of ArrayClassSageInterface::isArrayLibGeneralFunction \n");

	return bretValue;		
}

Boolean ArrayClassSageInterface::isArrayLibFunction(const char *pFnString)
{	
	Boolean  bretValue = FALSE;

     	if (bDebug == verbose_debug_level)
        	  printf ("Inside of ArrayClassSageInterface::isArrayLibFunction \n");

        ROSE_ASSERT( pFnString != NULL );
     	
	if(!(bretValue = isArrayLibMemberFunction(pFnString)))		// check whether it is a function
		bretValue  = isArrayLibFriendFunction(pFnString);

     	if (bDebug == verbose_debug_level)
        	  printf ("Exiting of ArrayClassSageInterface::isArrayLibFunction \n");

	return bretValue;		
}

Boolean ArrayClassSageInterface::isArrayLibOperator(const char *pFnString)
{	
	Boolean  bretValue = FALSE;

     	if (bDebug == verbose_debug_level)
        	  printf ("Inside of ArrayClassSageInterface::isArrayLibOperator \n");

        ROSE_ASSERT( pFnString != NULL );
     	
	if(!(bretValue = isArrayLibMemberOperator(pFnString)))		// check whether it is a function
		bretValue  = isArrayLibFriendOperator(pFnString);

     	if (bDebug == verbose_debug_level)
        	  printf ("Exiting of ArrayClassSageInterface::isArrayLibOperator \n");

	return bretValue;		
}


Boolean ArrayClassSageInterface::isArrayLibMemberFunction(const char *pFnSymbol)
{
  Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArrayLibMemberFunction \n");

  ROSE_ASSERT(pFnSymbol != NULL);
  
  if(!(bretValue = isArrayClassMemberFunction(pFnSymbol)))
    bretValue = isArraySectionMemberFunction(pFnSymbol);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayLibMemberFunction \n");

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayLibFriendFunction(const char *pFnSymbol)
{
  Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArrayLibFriendFunction \n");

  ROSE_ASSERT(pFnSymbol != NULL);
  
  if(!(bretValue = isArrayClassFriendFunction(pFnSymbol)))
    bretValue = isArraySectionFriendFunction(pFnSymbol);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayLibFriendFunction \n");

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayClassGeneralFunction(const char *pFnSymbol)
{
   Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface:isArrayGeneralClassFunction: \n");

  ROSE_ASSERT(pFnSymbol != NULL);
  
  if(!(bretValue = isArrayClassFunction(pFnSymbol)))
    bretValue = isArrayClassOperator(pFnSymbol);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayClassGeneralFunction \n");

  return bretValue; 
}

Boolean ArrayClassSageInterface::isArrayClassFunction(const char *pFnSymbol)
{
   Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface:isArrayClassFunction: \n");

  ROSE_ASSERT(pFnSymbol != NULL);
  
  if(!(bretValue = isArrayClassMemberFunction(pFnSymbol)))
    bretValue = isArrayClassFriendFunction(pFnSymbol);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayClassFunction \n");

  return bretValue; 
}

Boolean ArrayClassSageInterface::isArrayClassOperator(const char *pFnSymbol)
{
   Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArrayClassOperator \n");

  ROSE_ASSERT(pFnSymbol != NULL);
  
  if(!(bretValue = isArrayClassMemberOperator(pFnSymbol)))
    bretValue = isArrayClassFriendOperator(pFnSymbol);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayClassOperator \n");

  return bretValue; 
}

Boolean ArrayClassSageInterface::isArrayClassMemberFunction(const char *pFnSymbol)
{
  Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArrayClassMemberFunction\n");

  ROSE_ASSERT(pFnSymbol != NULL);
  
  if (bDebug == verbose_debug_level)
    printf ("Looking for match with %s \n",pFnSymbol);

  // List all types of A++ objects we want to recognize
  // Comment out the member functions that we can ignore for now
     if ( !strcmp ( pFnSymbol , "evaluate")                    ||
          !strcmp ( pFnSymbol , "reference")                   ||
       // !strcmp ( pFnSymbol , "getDataPointer")              ||
       // !strcmp ( pFnSymbol , "getDataReference")            ||
       // !strcmp ( pFnSymbol , "getArrayDescriptorPointer")   ||
       // !strcmp ( pFnSymbol , "getArrayDescriptorReference") ||
       // !strcmp ( pFnSymbol , "MarkAsTemporary")             ||
          !strcmp ( pFnSymbol , "Array_ID")                    ||
          !strcmp ( pFnSymbol , "fill")                        ||
          !strcmp ( pFnSymbol , "seqAdd")                      ||
          !strcmp ( pFnSymbol , "indexMap")                    ||
          !strcmp ( pFnSymbol , "setBase")                     ||
          !strcmp ( pFnSymbol , "getBase")                     ||
          !strcmp ( pFnSymbol , "getStride")                   ||
          !strcmp ( pFnSymbol , "getBound")                    ||
       // !strcmp ( pFnSymbol , "getGeometryBase")             ||
       // !strcmp ( pFnSymbol , "setGeometryBase")             ||
          !strcmp ( pFnSymbol , "dimension")                   ||
          !strcmp ( pFnSymbol , "elementCount")                ||
          !strcmp ( pFnSymbol , "cols")                        ||
          !strcmp ( pFnSymbol , "rows")                        ||
          !strcmp ( pFnSymbol , "lock")                        ||
          !strcmp ( pFnSymbol , "unlock")                      ||
          !strcmp ( pFnSymbol , "redim")                       ||
          !strcmp ( pFnSymbol , "reshape")                     ||
          !strcmp ( pFnSymbol , "resize")                      ||
          !strcmp ( pFnSymbol , "adopt")                       ||
          !strcmp ( pFnSymbol , "copy")                        ||
          !strcmp ( pFnSymbol , "useFortranIndexing")          ||
          !strcmp ( pFnSymbol , "replace") )
        {
          if (bDebug == verbose_debug_level)
               printf ("%s is an A++ member function\n",pFnSymbol );
          bretValue = TRUE;
        }
       else
        {
          if (bDebug == verbose_debug_level)
               printf ("%s is NOT a A++ member function \n", pFnSymbol);
        }

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayClassMemberFunction\n");
  
  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayClassFriendFunction(const char *pFnSymbol)
{
  Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArrayClassFriendFunction \n");

  ROSE_ASSERT(pFnSymbol != NULL);

  if (bDebug == verbose_debug_level)
      printf ("Looking for match with %s \n",pFnSymbol);

  // List all types of A++ objects we want to recognize
     if ( !strcmp ( pFnSymbol , "min")       ||
          !strcmp ( pFnSymbol , "max")       ||
          !strcmp ( pFnSymbol , "fmod")      ||
          !strcmp ( pFnSymbol , "mod")       ||
          !strcmp ( pFnSymbol , "pow")       ||
          !strcmp ( pFnSymbol , "sign")      ||
          !strcmp ( pFnSymbol , "sum")       ||
          !strcmp ( pFnSymbol , "log")       ||
          !strcmp ( pFnSymbol , "log10")     ||
          !strcmp ( pFnSymbol , "exp")       ||
          !strcmp ( pFnSymbol , "sqrt")      ||
          !strcmp ( pFnSymbol , "fabs")      ||
          !strcmp ( pFnSymbol , "ceil")      ||
          !strcmp ( pFnSymbol , "floor")     ||
          !strcmp ( pFnSymbol , "abs")       ||
          !strcmp ( pFnSymbol , "transpose") ||
          !strcmp ( pFnSymbol , "cos")       ||
          !strcmp ( pFnSymbol , "sin")       ||
          !strcmp ( pFnSymbol , "tan")       ||
          !strcmp ( pFnSymbol , "acos")      ||
          !strcmp ( pFnSymbol , "asin")      ||
          !strcmp ( pFnSymbol , "atan")      ||
          !strcmp ( pFnSymbol , "cosh")      ||
          !strcmp ( pFnSymbol , "sinh")      ||
          !strcmp ( pFnSymbol , "tanh")      ||
          !strcmp ( pFnSymbol , "acosh")     ||
          !strcmp ( pFnSymbol , "asinh")     ||
          !strcmp ( pFnSymbol , "atanh") )
        {
          if (bDebug == verbose_debug_level)
               printf ("Function name is %s an array class friend function \n",pFnSymbol);
          bretValue = TRUE;
        }
       else
        {
          if (bDebug == verbose_debug_level)
               printf ("%s is not an A++ friend function \n", pFnSymbol);
        } 

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayClassFriendFunction \n");

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayLibMemberOperator(const char *pOpSymbol)
{
  Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArrayLibMemberOperator \n");

  ROSE_ASSERT(pOpSymbol != NULL);

  if(!(bretValue = isArrayClassMemberOperator(pOpSymbol)))
    bretValue = isArraySectionMemberOperator(pOpSymbol);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayLibMemberOperator \n");

  return bretValue;
}


Boolean ArrayClassSageInterface::isArrayClassMemberOperator(const char *pOpSymbol)
{
  Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArrayClassMemberOperator \n");

  ROSE_ASSERT(pOpSymbol != NULL);
  
    // List all types of A++ objects we want to recognize
  // Comment out the operator functions that we can not consider as transformable
     if ( !strcmp ( pOpSymbol , "operator()") ||
	  !strcmp ( pOpSymbol , "operator=")  ||
          !strcmp ( pOpSymbol , "operator+")  ||
          !strcmp ( pOpSymbol , "operator-")  ||
          !strcmp ( pOpSymbol , "operator!")  ||        // CONFIRM WITH DAN that it's implemented
          //!strcmp ( pOpSymbol , "operator*")  ||      // appears to be a friend operator only 
          //!strcmp ( pOpSymbol , "operator/")  ||      // appears to be a friend operator only 
          //!strcmp ( pOpSymbol , "operator%")  ||      // appears to be a friend operator only 
          !strcmp ( pOpSymbol , "operator+=") ||
          !strcmp ( pOpSymbol , "operator-=") ||
          !strcmp ( pOpSymbol , "operator*=") ||
          !strcmp ( pOpSymbol , "operator/=") ||
          !strcmp ( pOpSymbol , "operator%=") ||
          !strcmp ( pOpSymbol , "operator++") ||
          !strcmp ( pOpSymbol , "operator--") )
        {
          if (bDebug == verbose_debug_level)
               printf ("Operator function %s is an A++ member operator \n",pOpSymbol );
	  bretValue = TRUE;
        }
     else
       {
	 if (bDebug == verbose_debug_level)
            printf ("Operator function %s is not an A++ member operator \n",pOpSymbol );
       }

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayClassMemberOperator \n");

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayLibFriendOperator(const char *pOpSymbol)
{
  Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArrayLibFriendOperator \n");

  ROSE_ASSERT(pOpSymbol != NULL);

  if(!(bretValue = isArrayClassFriendOperator(pOpSymbol)))
    bretValue = isArraySectionFriendOperator(pOpSymbol);

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArrayLibFriendOperator \n");

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayClassFriendOperator(const char *pOpSymbol)
{
  Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArrayClassFriendOperator \n");

  ROSE_ASSERT(pOpSymbol != NULL);
  
      // List all types of A++ objects we want to recognize
  // Comment out the operator functions that we can not consider as transformable
  if (
        !strcmp ( pOpSymbol , "operator+")   ||
        !strcmp ( pOpSymbol , "operator-")   ||
        !strcmp ( pOpSymbol , "operator*")   ||      
        !strcmp ( pOpSymbol , "operator/")   ||      
        !strcmp ( pOpSymbol , "operator%")   ||     
        !strcmp ( pOpSymbol , "operator<")   ||     
        !strcmp ( pOpSymbol , "operator<=")  ||     
        !strcmp ( pOpSymbol , "operator>")   ||     
        !strcmp ( pOpSymbol , "operator>=")  ||            
        !strcmp ( pOpSymbol , "operator==")  ||     
        !strcmp ( pOpSymbol , "operator!=")  ||     
        !strcmp ( pOpSymbol , "operator&&")  ||     
        !strcmp ( pOpSymbol , "operator||") )
	{
          if (bDebug == verbose_debug_level)
               printf ("Operator function %s is an A++ class friend operator \n",pOpSymbol );
	  bretValue = TRUE;
        }
     else
       {
	 if (bDebug == verbose_debug_level)
            printf ("Operator function %s is not an A++ class friend operator \n",pOpSymbol );
       }


  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayClassFriendOperator \n");

  return bretValue;
}

Boolean ArrayClassSageInterface::isArraySectionGeneralFunction(const char *pFnSymbol)
{
   Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface:isArrayGeneralSectionFunction: \n");

  ROSE_ASSERT(pFnSymbol != NULL);
  
  if(!(bretValue = isArraySectionFunction(pFnSymbol)))
    bretValue = isArraySectionOperator(pFnSymbol);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArraySectionGeneralFunction \n");

  return bretValue; 
}

Boolean ArrayClassSageInterface::isArraySectionFunction(const char *pFnSymbol)
{
   Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface:isArraySectionFunction: \n");

  ROSE_ASSERT(pFnSymbol != NULL);
  
  if(!(bretValue = isArraySectionMemberFunction(pFnSymbol)))
    bretValue = isArraySectionFriendFunction(pFnSymbol);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArraySectionFunction \n");

  return bretValue; 
}

Boolean ArrayClassSageInterface::isArraySectionOperator(const char *pFnSymbol)
{
   Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArraySectionOperator \n");

  ROSE_ASSERT(pFnSymbol != NULL);
  
  if(!(bretValue = isArraySectionMemberOperator(pFnSymbol)))
    bretValue = isArraySectionFriendOperator(pFnSymbol);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArraySectionOperator \n");

  return bretValue; 
}

Boolean ArrayClassSageInterface::isArraySectionMemberFunction(const char *pFnSymbol)
{
  Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArraySectionMemberFunction\n");

  ROSE_ASSERT(pFnSymbol != NULL);
  
  if(!(bretValue = isArrayIndexMemberFunction(pFnSymbol)))
    bretValue = isArrayRangeMemberFunction(pFnSymbol);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArraySectionMemberFunction\n");
  
  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayIndexMemberFunction(const char *pFnSymbol)
{
  Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArrayIndexMemberFunction\n");

  ROSE_ASSERT(pFnSymbol != NULL);
  
  bretValue = isArrayInternalIndexMemberFunction(pFnSymbol);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayIndexMemberFunction\n");
  
  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayRangeMemberFunction(const char *pFnSymbol)
{
  Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArrayRangeMemberFunction\n");

  ROSE_ASSERT(pFnSymbol != NULL);

  bretValue = isArrayInternalIndexMemberFunction(pFnSymbol);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayRangeMemberFunction\n");
  
  return bretValue;
}

Boolean ArrayClassSageInterface::isArraySectionFriendFunction(const char *pFnSymbol)
{
  Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArraySectionFriendFunction\n");

  ROSE_ASSERT(pFnSymbol != NULL);
  
  if(!(bretValue = isArrayIndexFriendFunction(pFnSymbol)))
    bretValue = isArrayRangeFriendFunction(pFnSymbol);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArraySectionFriendFunction\n");
  
  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayIndexFriendFunction(const char *pFnSymbol)
{
  Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArrayIndexFriendFunction\n");

  ROSE_ASSERT(pFnSymbol != NULL);

  bretValue = isArrayInternalIndexFriendFunction(pFnSymbol);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayIndexFriendFunction\n");
  
  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayRangeFriendFunction(const char *pFnSymbol)
{
  Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArrayRangeFriendFunction\n");

  ROSE_ASSERT(pFnSymbol != NULL);

  bretValue = isArrayInternalIndexFriendFunction(pFnSymbol);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayRangeFriendFunction\n");
  
  return bretValue;
}

Boolean ArrayClassSageInterface::isArraySectionMemberOperator(const char *pOpSymbol)
{
  Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArraySectionMemberOperator \n");

  ROSE_ASSERT(pOpSymbol != NULL);
  
  if(!(bretValue = isArrayIndexMemberOperator(pOpSymbol)))
    bretValue = isArrayRangeMemberOperator(pOpSymbol);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArraySectionMemberOperator \n");
  
  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayIndexMemberOperator(const char *pOpSymbol)
{
  Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArrayIndexMemberOperator \n");

  ROSE_ASSERT(pOpSymbol != NULL);

  // List all types of A++ objects we want to recognize
  // Comment out the operator functions that we can not consider as transformable
  if (!strcmp ( pOpSymbol , "operator="))               // looks like we only have this operator to check for
    	{
          if (bDebug == verbose_debug_level)
               printf ("Operator function %s is an A++ index member operator \n",pOpSymbol );
	  bretValue = TRUE;
        }
   else
       bretValue = isArrayInternalIndexMemberOperator(pOpSymbol);

  if((!bretValue)&&(bDebug == verbose_debug_level))
    printf("Operator function %s is not an A++ index member operator", pOpSymbol);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayIndexMemberOperator \n");
  
  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayRangeMemberOperator(const char *pOpSymbol)
{
  Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArrayRangeMemberOperator \n");

  ROSE_ASSERT(pOpSymbol != NULL);

  // List all types of A++ objects we want to recognize
  // Comment out the operator functions that we can not consider as transformable
  if (!strcmp ( pOpSymbol , "operator="))               // looks like we only have this operator to check for
    	{
          if (bDebug == verbose_debug_level)
               printf ("Operator function %s is an A++ range member operator \n",pOpSymbol );
	  bretValue = TRUE;
        }
  else
       bretValue = isArrayInternalIndexMemberOperator(pOpSymbol);

  if((!bretValue)&&(bDebug == verbose_debug_level))
    printf("Operator function %s is not an A++ range member operator", pOpSymbol);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayRangeMemberOperator \n");
  
  return bretValue;
}

Boolean ArrayClassSageInterface::isArraySectionFriendOperator(const char *pOpSymbol)
{
  Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArraySectionFriendOperator \n");

  ROSE_ASSERT(pOpSymbol != NULL);
  
  if(!(bretValue = isArrayIndexFriendOperator(pOpSymbol)))
    bretValue = isArrayRangeFriendOperator(pOpSymbol);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArraySectionFriendOperator\n");
  
  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayIndexFriendOperator(const char *pOpSymbol)
{
  Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArrayIndexFriendOperator \n");

  ROSE_ASSERT(pOpSymbol != NULL);

  bretValue = isArrayInternalIndexFriendOperator(pOpSymbol);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayIndexFriendOperator\n");
  
  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayRangeFriendOperator(const char *pOpSymbol)
{
  Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArrayRangeFriendOperator \n");

  ROSE_ASSERT(pOpSymbol != NULL);

  bretValue = isArrayInternalIndexFriendOperator(pOpSymbol);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayRangeFriendOperator\n");
  
  return bretValue;
}

// the functions below are for the InternalIndex class from which the Index and Range classes are derived

Boolean ArrayClassSageInterface::isArrayInternalIndexMemberFunction( const char *pOpSymbol)
{
  Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entering ArrayClassSageInterface::isArrayInternalIndexMemberFunction \n");

  ROSE_ASSERT( pOpSymbol != NULL );

  if(
     !strcmp ( pOpSymbol , "getBase"                 )  ||
     !strcmp ( pOpSymbol , "getBound"                )  ||
     !strcmp ( pOpSymbol , "getCount"                )  ||
     !strcmp ( pOpSymbol , "getMode"                 )  ||
     !strcmp ( pOpSymbol , "getStride"               )  ||
     !strcmp ( pOpSymbol , "setBoundsCheck"          )  ||           // will have to check whether these functions need to be examined
     !strcmp ( pOpSymbol , "getBaseReference"        )  ||
     !strcmp ( pOpSymbol , "getBoundReference"       )  ||
     !strcmp ( pOpSymbol , "getStrideReference"      )  ||
     !strcmp ( pOpSymbol , "getModeReference"        )  ||
     !strcmp ( pOpSymbol , "getLocalPart"            )  ||
     !strcmp ( pOpSymbol , "getPointerToLocalPart"   )  ||
     !strcmp ( pOpSymbol , "adjustBase"              )  ||
     !strcmp ( pOpSymbol , "adjustBound"             )  ||
     !strcmp ( pOpSymbol , "display"             )  ||
     !strcmp ( pOpSymbol , "length"   ) )
    {
      if(bDebug == verbose_debug_level)
	printf("Function %s is an A++ InternalIndex member function \n", pOpSymbol);
      bretValue = TRUE;
    }
  else
    {
      if(bDebug == verbose_debug_level)
	printf("Function %s is not an A++ InternalIndex member function \n", pOpSymbol);
    }

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface:isArrayInternalIndexMemberFunction \n");

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayInternalIndexFriendFunction( const char *pOpSymbol)
{
  Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayInternalIndexFriendFunction \n");

  // the InternalIndex class has no friend functions definesd so return the default FALSE
  if (bDebug == verbose_debug_level)
    printf("Function %s is not an A++ InternalIndex friend function \n", pOpSymbol);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface:isArrayInternalIndexFriendFunction \n");

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayInternalIndexFriendOperator( const char *pOpSymbol)
{
  Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArrayInternalIndexFriendOperator \n");

  ROSE_ASSERT(pOpSymbol != NULL);
  
  // List all types of A++ objects we want to recognize
  // Comment out the operator functions that we can not consider as transformable
  if (
        !strcmp ( pOpSymbol , "operator+")   ||
        !strcmp ( pOpSymbol , "operator-")   ||
        !strcmp ( pOpSymbol , "operator*")   ||      
        !strcmp ( pOpSymbol , "operator/") ) 
	{
          if (bDebug == verbose_debug_level)
               printf ("Operator function %s is an A++ internal index friend operator \n", pOpSymbol );
	  bretValue = TRUE;
        }
     else
       {
	 if (bDebug == verbose_debug_level)
            printf ("Operator function %s is not an A++ internal index friend operator \n", pOpSymbol );
       }

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayInternalIndexFriendOperator\n");
  
  return bretValue;
}


Boolean ArrayClassSageInterface::isArrayInternalIndexMemberOperator( const char *pOpSymbol)
{
  Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArrayInternalIndexMemberOperator \n");

  ROSE_ASSERT(pOpSymbol != NULL);
  
  // List all types of A++ objects we want to recognize
  // Comment out the operator functions that we can not consider as transformable
  if (
        !strcmp ( pOpSymbol , "operator+=")   ||
        !strcmp ( pOpSymbol , "operator-=")   ||
        !strcmp ( pOpSymbol , "operator*=")   ||      
        !strcmp ( pOpSymbol , "operator/=")   ||
        !strcmp ( pOpSymbol , "operator==")   ||
        !strcmp ( pOpSymbol , "operator!=")   ||
        !strcmp ( pOpSymbol , "operator()")   ||
	!strcmp ( pOpSymbol , "operator-" ) )        // unary minus operator
	{
          if (bDebug == verbose_debug_level)
               printf ("Operator function %s is an A++ internal index member operator \n", pOpSymbol );
	  bretValue = TRUE;
        }
     else
       {
	 if (bDebug == verbose_debug_level)
            printf ("Operator function %s is not an A++ internal index member operator \n", pOpSymbol );
       }

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayInternalIndexMemberOperator\n");
  
  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayLibType( const char *ptypeSymbol)
{
  Boolean bretValue = FALSE;  
  
  if (bDebug == verbose_debug_level)
    printf("Entering ArrayClassSageInterface::isArrayLibType \n");

  ROSE_ASSERT( ptypeSymbol != NULL );

  if(!(bretValue = isArrayClassType(ptypeSymbol)))
    bretValue = isArraySectionType(ptypeSymbol);
  
  if((bretValue) && ( bDebug == verbose_debug_level))
    printf("Type symbol %s is an A++ library type \n", ptypeSymbol);
  else
    printf("Type symbol %s is not an A++ library type \n", ptypeSymbol);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayLibType \n");

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayClassType( const char *ptypeSymbol)
{
  Boolean bretValue = FALSE;

  ROSE_ASSERT( ptypeSymbol != NULL );

  printf ("ArrayClassSageInterface::isArrayClassType(): ptypeSymbol = %s \n",ptypeSymbol);
  
  if (bDebug == verbose_debug_level)
    printf("Entering ArrayClassSageInterface::isArrayClassType \n");

  if(
     !strcmp ( ptypeSymbol , "doubleArray" ) ||             // maybe additional checks are required later
     !strcmp ( ptypeSymbol , "floatArray"  ) ||
     !strcmp ( ptypeSymbol , "intArray"    ) )
    {
      if (bDebug == verbose_debug_level)
	printf("Type symbol %s is an A++ class type \n", ptypeSymbol);
      bretValue = TRUE;
    }
  else
    {
      if (bDebug == verbose_debug_level)
	printf("Type symbol %s is not an A++ class type \n", ptypeSymbol);
    }

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayClassType \n");

  return bretValue;
}

Boolean ArrayClassSageInterface::isArraySectionType( const char *ptypeSymbol)
{
  Boolean bretValue = FALSE;  
  
  if (bDebug == verbose_debug_level)
    printf("Entering ArrayClassSageInterface::isArraySectionType \n");

  ROSE_ASSERT( ptypeSymbol != NULL );

  if(
     !strcmp ( ptypeSymbol , "Index"  ) ||             // maybe additional checks are required later
     !strcmp ( ptypeSymbol , "Range"  ) )
    {
      if (bDebug == verbose_debug_level)
	printf("Type symbol %s is an A++ section type \n", ptypeSymbol);
      bretValue = TRUE;
    }
  else
    {
      if (bDebug == verbose_debug_level)
	printf("Type symbol %s is not an A++ section type \n", ptypeSymbol);
    }

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArraySectionType \n");

  return bretValue;
}

// -----------------------------------------------------------------------------------------------------
// the function definitions for all the 'isArray' functions with SgName * args, these are basically 
// wrapper functions
// -----------------------------------------------------------------------------------------------------
// suggest that these be removed as they are unnecessary and we can live without them

Boolean ArrayClassSageInterface::isArrayLibGeneralFunction(SgName &sgFunctionName)		// General Function refers to Function/Operator
{
  return (isArrayLibGeneralFunction(sgFunctionName.str()));
}

Boolean ArrayClassSageInterface::isArrayLibFunction(SgName &sgFunctionName)	// check if is a member or friend function of an array class
{
  return (isArrayLibFunction(sgFunctionName.str()));
}

Boolean ArrayClassSageInterface::isArrayLibMemberFunction(SgName &sgFunctionName)		
{
  
  return (isArrayLibMemberFunction(sgFunctionName.str()));
}

Boolean ArrayClassSageInterface::isArrayClassMemberFunction(SgName &sgFunctionName)
{
  return ( isArrayClassMemberFunction(sgFunctionName.str()) );
}

Boolean ArrayClassSageInterface::isArraySectionMemberFunction(SgName &sgFunctionName)
{
  return ( isArraySectionMemberFunction(sgFunctionName.str()) );
}
		
Boolean ArrayClassSageInterface::isArrayIndexMemberFunction(SgName &sgFunctionName)
{
  return ( isArrayIndexMemberFunction(sgFunctionName.str()) );
}

Boolean ArrayClassSageInterface::isArrayRangeMemberFunction(SgName &sgFunctionName)
{
  return ( isArrayRangeMemberFunction(sgFunctionName.str()) );
}

Boolean ArrayClassSageInterface::isArrayLibFriendFunction(SgName &sgFunctionName)
{
  return ( isArrayLibFriendFunction(sgFunctionName.str()) );
}

Boolean ArrayClassSageInterface::isArrayClassFriendFunction(SgName &sgFunctionName)
{
  return ( isArrayClassFriendFunction(sgFunctionName.str()) );
}
		
Boolean ArrayClassSageInterface::isArraySectionFriendFunction(SgName &sgFunctionName)
{
  return ( isArraySectionFriendFunction(sgFunctionName.str()) );
}

Boolean ArrayClassSageInterface::isArrayIndexFriendFunction(SgName &sgFunctionName)
{
  return ( isArrayIndexFriendFunction(sgFunctionName.str()) );
}

Boolean ArrayClassSageInterface::isArrayRangeFriendFunction(SgName &sgFunctionName)
{
  return ( isArrayRangeFriendFunction(sgFunctionName.str()) );
}

Boolean ArrayClassSageInterface::isArrayLibOperator(SgName &sgOperatorName)
{
  return ( isArrayLibOperator(sgOperatorName.str()) );
}
		
Boolean ArrayClassSageInterface::isArrayLibMemberOperator(SgName &sgOperatorName)
{
  return ( isArrayLibMemberOperator(sgOperatorName.str()) );
}

Boolean ArrayClassSageInterface::isArrayClassMemberOperator(SgName &sgOperatorName)
{
  return ( isArrayClassMemberOperator(sgOperatorName.str()) );
}
						
Boolean ArrayClassSageInterface::isArraySectionMemberOperator(SgName &sgOperatorName)
{
  return ( isArraySectionMemberOperator(sgOperatorName.str()) );
}

Boolean ArrayClassSageInterface::isArrayIndexMemberOperator(SgName &sgOperatorName)
{
  return ( isArrayIndexMemberOperator(sgOperatorName.str()) );
}

Boolean ArrayClassSageInterface::isArrayRangeMemberOperator(SgName &sgOperatorName)
{
  return ( isArrayRangeMemberOperator(sgOperatorName.str()) );
}

Boolean ArrayClassSageInterface::isArrayLibFriendOperator(SgName &sgOperatorName)
{
  return ( isArrayLibFriendOperator(sgOperatorName.str()) );
}

Boolean ArrayClassSageInterface::isArrayClassFriendOperator(SgName &sgOperatorName)
{
  return ( isArrayClassFriendOperator(sgOperatorName.str()) );
}

Boolean ArrayClassSageInterface::isArraySectionFriendOperator(SgName &sgOperatorName)
{
  return ( isArraySectionFriendOperator(sgOperatorName.str()) );
}

Boolean ArrayClassSageInterface::isArrayIndexFriendOperator(SgName &sgOperatorName)
{
  return ( isArrayIndexFriendOperator(sgOperatorName.str()) );
}

Boolean ArrayClassSageInterface::isArrayRangeFriendOperator(SgName &sgOperatorName)
{
  return ( isArrayRangeFriendOperator(sgOperatorName.str()) );
}
		
Boolean ArrayClassSageInterface::isArrayLibType(SgName &sgTypeName)
{
  return ( isArrayLibType(sgTypeName.str()) );
}

Boolean ArrayClassSageInterface::isArrayClassType(SgName &sgTypeName)
{
  return ( isArrayClassType(sgTypeName.str()) );
}

Boolean ArrayClassSageInterface::isArraySectionType(SgName &sgTypeName)
{
  return ( isArraySectionType(sgTypeName.str()) );
}

// -----------------------------------------------------------------------------------------------------
// am writing the isArray* functions with SgFunctionDeclaration * argument as I believe those having 
// SgExpression, SgSymbol *, will end up calling these at some point.
// -----------------------------------------------------------------------------------------------------

// next set of definitions

Boolean ArrayClassSageInterface::isArrayLibGeneralFunction(SgFunctionDeclaration *pSgFunctionDecl) 
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayLibGeneralFunction \n" );

  ROSE_ASSERT(pSgFunctionDecl != NULL);

  if ( !( bretValue = isArrayLibFunction( pSgFunctionDecl )) )
    bretValue = isArrayLibOperator( pSgFunctionDecl );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayLibGeneralFunction \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayLibFunction(SgFunctionDeclaration *pSgFunctionDecl)	
{                                           // check if is a member or friend function of an array class
  Boolean bretValue = FALSE;
  
  if( bDebug == verbose_debug_level )
    printf( "Entering ArrayClassSageInterface::isArrayLibFunction \n" );

  ROSE_ASSERT( pSgFunctionDecl!= NULL );

  if ( !( bretValue = isArrayLibMemberFunction( pSgFunctionDecl )) )
    bretValue = isArrayLibFriendFunction( pSgFunctionDecl );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if( bDebug == verbose_debug_level )
    printf( "Exiting ArrayClassSageInterface::isArrayLibFunction \n" );

  return bretValue;
}


Boolean ArrayClassSageInterface::isArrayLibMemberFunction(SgFunctionDeclaration *pSgFunctionDecl)		
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayLibMemberFunction \n" );

  ROSE_ASSERT( pSgFunctionDecl != NULL );

  if ( !( bretValue = isArrayClassMemberFunction( pSgFunctionDecl )) )
    bretValue = isArraySectionMemberFunction( pSgFunctionDecl );

  // am not adding ROSE_DEBUG statements here as yet. Will/may add them later after talking to Dan

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayLibMemberFunction \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayClassGeneralFunction(SgFunctionDeclaration *pSgFunctionDecl)
{
   Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface:isArrayGeneralClassFunction: \n");

  ROSE_ASSERT(pSgFunctionDecl != NULL);
  
  if(!(bretValue = isArrayClassFunction(pSgFunctionDecl)))
    bretValue = isArrayClassOperator(pSgFunctionDecl);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayClassGeneralFunction \n");

  return bretValue; 
}

Boolean ArrayClassSageInterface::isArrayClassFunction(SgFunctionDeclaration *pSgFunctionDecl)
{
   Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface:isArrayClassFunction: \n");

  ROSE_ASSERT(pSgFunctionDecl != NULL);
  
  if(!(bretValue = isArrayClassMemberFunction(pSgFunctionDecl)))
    bretValue = isArrayClassFriendFunction(pSgFunctionDecl);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayClassFunction \n");

  return bretValue; 
}

Boolean ArrayClassSageInterface::isArrayClassOperator(SgFunctionDeclaration *pSgFunctionDecl)
{
   Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArrayClassOperator \n");

  ROSE_ASSERT(pSgFunctionDecl != NULL);
  
  if(!(bretValue = isArrayClassMemberOperator(pSgFunctionDecl)))
    bretValue = isArrayClassFriendOperator(pSgFunctionDecl);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayClassOperator \n");

  return bretValue; 
}

// we're given a function declaration statement and should figure out given this information
// to the best of our ability whether it is a member function of an A++ array class

Boolean ArrayClassSageInterface::isArrayClassMemberFunction(SgFunctionDeclaration *pSgFunctionDecl)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayClassMemberFunction \n" );

  ROSE_ASSERT( pSgFunctionDecl!= NULL);

  SgMemberFunctionDeclaration *pmemberFunctionDeclaration = isSgMemberFunctionDeclaration(pSgFunctionDecl); 
  ROSE_ASSERT ( pmemberFunctionDeclaration != NULL );     // make sure it's a member function

  SgFunctionDefinition *pfunctionDefinition = pmemberFunctionDeclaration->get_definition();
  ROSE_ASSERT (pfunctionDefinition != NULL);

  SgFunctionDeclaration* pfunctionDeclarationStatement = pfunctionDefinition->get_declaration();
  ROSE_ASSERT (pfunctionDeclarationStatement != NULL);

  // ------------------------------------------------------------------------------
  // check whether the class of the function is doubleArray, floatArray or intArray

  // SUGGEST adding utility function const char *get_class_name(SgClassDefinition *)
  //  to utility_functions.*

  if ( !pfunctionDeclarationStatement->isOperator() )             //  after ensuring it's not an operator
    {
      SgClassDefinition *pclassDefinition = isSgClassDefinition( pmemberFunctionDeclaration->get_scope() );

      ROSE_ASSERT( pclassDefinition != NULL );

      // the assert should be ok as we should be able to get a proper class definition 
      // from the member function declaration
      // not sure whether base classes should be checked as user defined classes derived from  
      // A++ base classes could be present ?????

      SgClassDeclaration *pclassDeclaration = isSgClassDeclaration( pclassDefinition->get_declaration() );
      ROSE_ASSERT ( pclassDeclaration != NULL );

      SgName sageClassName = pclassDeclaration->get_name();

      const char *pclassNameString = sageClassName.str();

      bretValue = isArrayClassType( pclassNameString );

      // ------------------------------------------------------------------------------

      // check whether the function name is an A++ array member function

      // from showSgFunctionDeclaration (SgStatement*)
      // SgFunctionDeclaration* functionDeclarationStatement = isSgFunctionDeclaration( pfunctionDeclarationStatement );
      // ROSE_ASSERT (pfunctionDeclarationStatement != NULL);

      if ( bretValue )
	{

	  SgName sageFunctionName = pfunctionDeclarationStatement->get_name();

	  const char *pfunctionName = sageFunctionName.str();

	  bretValue = bretValue && isArrayClassMemberFunction(pfunctionName);
	
	}

      // ------------------------------------------------------------------------------
    }

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayClassMemberFunction \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArraySectionGeneralFunction(SgFunctionDeclaration *pSgFunctionDecl)
{
   Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface:isArrayGeneralSectionFunction: \n");

  ROSE_ASSERT(pSgFunctionDecl != NULL);
  
  if(!(bretValue = isArraySectionFunction(pSgFunctionDecl)))
    bretValue = isArraySectionOperator(pSgFunctionDecl);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArraySectionGeneralFunction \n");

  return bretValue; 
}

Boolean ArrayClassSageInterface::isArraySectionFunction(SgFunctionDeclaration *pSgFunctionDecl)
{
   Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface:isArraySectionFunction: \n");

  ROSE_ASSERT(pSgFunctionDecl != NULL);
  
  if(!(bretValue = isArraySectionMemberFunction(pSgFunctionDecl)))
    bretValue = isArraySectionFriendFunction(pSgFunctionDecl);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArraySectionFunction \n");

  return bretValue; 
}

Boolean ArrayClassSageInterface::isArraySectionOperator(SgFunctionDeclaration *pSgFunctionDecl)
{
   Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArraySectionOperator \n");

  ROSE_ASSERT(pSgFunctionDecl != NULL);
  
  if(!(bretValue = isArraySectionMemberOperator(pSgFunctionDecl)))
    bretValue = isArraySectionFriendOperator(pSgFunctionDecl);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArraySectionOperator \n");

  return bretValue; 
}

Boolean ArrayClassSageInterface::isArraySectionMemberFunction( SgFunctionDeclaration *pSgFunctionDecl )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArraySectionMemberFunction \n" );

  ROSE_ASSERT( pSgFunctionDecl!= NULL );

  if ( !( bretValue = isArrayIndexMemberFunction( pSgFunctionDecl )) )
    bretValue = isArrayRangeMemberFunction( pSgFunctionDecl );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArraySectionMemberFunction \n" );

  return bretValue;
}


Boolean ArrayClassSageInterface::isArrayIndexMemberFunction(SgFunctionDeclaration *pSgFunctionDecl)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayIndexMemberFunction \n" );

  ROSE_ASSERT( pSgFunctionDecl!= NULL);

  SgMemberFunctionDeclaration *pmemberFunctionDeclaration = isSgMemberFunctionDeclaration(pSgFunctionDecl); 
  ROSE_ASSERT ( pmemberFunctionDeclaration != NULL );     // make sure it's a member function

  SgFunctionDefinition *pfunctionDefinition = pmemberFunctionDeclaration->get_definition();
  ROSE_ASSERT (pfunctionDefinition != NULL);

  SgFunctionDeclaration* pfunctionDeclarationStatement = pfunctionDefinition->get_declaration();
  ROSE_ASSERT (pfunctionDeclarationStatement != NULL);

  if ( !pfunctionDeclarationStatement->isOperator() )   // ensure it's not an operator
    {

      // ------------------------------------------------------------------------------
      // check whether the class of the function is Index

      // SUGGEST adding utility function const char *get_class_name(SgClassDefinition *)
      // to utility_functions.*
      // for now add the function to the ArrayClassSageInterface class.

      SgClassDefinition *pclassDefinition = isSgClassDefinition( pmemberFunctionDeclaration->get_scope() );

      // not sure whether base classes should be checked as user defined classes derived from  
      // A++ base classes could be present ?????

      char *pName = (char *)ArrayClassSageInterface::getClassName(pclassDefinition);
      bretValue = !strcmp(pName, "Index") || !strcmp ( pName, "Internal_Index" );

      // ------------------------------------------------------------------------------

      // check whether the function name is an A++ Index member function

      // from showSgFunctionDeclaration (SgStatement*)
      // SgFunctionDeclaration* functionDeclarationStatement = isSgFunctionDeclaration( pfunctionDeclarationStatement );
      // ROSE_ASSERT (pfunctionDeclarationStatement != NULL);
      
      if ( bretValue )
	{
	  SgName sageFunctionName = pfunctionDeclarationStatement->get_name();

	  const char *pfunctionName = sageFunctionName.str();

	  bretValue = isArrayIndexMemberFunction(pfunctionName);
	}

      // ------------------------------------------------------------------------------
    }

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayIndexMemberFunction \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayRangeMemberFunction(SgFunctionDeclaration *pSgFunctionDecl)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayRangeMemberFunction \n" );

  ROSE_ASSERT( pSgFunctionDecl!= NULL);

  SgMemberFunctionDeclaration *pmemberFunctionDeclaration = isSgMemberFunctionDeclaration(pSgFunctionDecl); 
  ROSE_ASSERT ( pmemberFunctionDeclaration != NULL );     // make sure it's a member function

  SgFunctionDefinition *pfunctionDefinition = pmemberFunctionDeclaration->get_definition();
  ROSE_ASSERT (pfunctionDefinition != NULL);

  SgFunctionDeclaration* pfunctionDeclarationStatement = pfunctionDefinition->get_declaration();
  ROSE_ASSERT (pfunctionDeclarationStatement != NULL);

  if ( !pfunctionDeclarationStatement->isOperator() )   // ensure it's not an operator
    {
      // ------------------------------------------------------------------------------ 
      // check whether the class of the function is Range

      // SUGGEST adding utility function const char *get_class_name(SgClassDefinition *) 
      //  to utility_functions.*

      SgClassDefinition *pclassDefinition = isSgClassDefinition( pmemberFunctionDeclaration->get_scope() );

      // not sure whether base classes should be checked as user defined classes derived from   
      // A++ base classes could be present ?????

      bretValue = !strcmp( getClassName(pclassDefinition), "Range") || !strcmp(getClassName(pclassDefinition), "Internal_Index");

      // ------------------------------------------------------------------------------

      // check whether the function name is an A++ Range member function
 
      // from showSgFunctionDeclaration (SgStatement*)
      // SgFunctionDeclaration* functionDeclarationStatement = isSgFunctionDeclaration( pfunctionDeclarationStatement );
      // ROSE_ASSERT (pfunctionDeclarationStatement != NULL);

      if ( bretValue )
	{
	  SgName sageFunctionName = pfunctionDeclarationStatement->get_name();

	  const char *pfunctionName = sageFunctionName.str();

	  bretValue = isArrayRangeMemberFunction(pfunctionName);

	}
      // ------------------------------------------------------------------------------
    }

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayRangeMemberFunction \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayLibFriendFunction(SgFunctionDeclaration *pSgFunctionDecl )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayLibFriendFunction \n" );

  ROSE_ASSERT( pSgFunctionDecl!= NULL );

  if ( !( bretValue = isArrayClassFriendFunction( pSgFunctionDecl )) )
    bretValue = isArraySectionFriendFunction( pSgFunctionDecl );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayLibFriendFunction \n" );

  return bretValue;
}


Boolean ArrayClassSageInterface::isArrayClassFriendFunction(SgFunctionDeclaration *pSgFunctionDecl)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayClassFriendFunction \n" );

  ROSE_ASSERT( pSgFunctionDecl!= NULL);

  // the next may/probably be an unnecessary check
  SgFunctionDeclaration *pSgFunctionDeclarationStatement = isSgFunctionDeclaration(pSgFunctionDecl);
  ROSE_ASSERT(pSgFunctionDeclarationStatement);

  SgFunctionDefinition *pSgFunctionDefinition = pSgFunctionDeclarationStatement->get_definition();
  ROSE_ASSERT( pSgFunctionDefinition != NULL );

  SgFunctionDeclaration *pSgFunctionDeclaration = pSgFunctionDefinition->get_declaration();
  ROSE_ASSERT(pSgFunctionDeclaration);

  // make sure it's a friend and also not an operator
  if ( (!pSgFunctionDeclaration->isOperator()) && (pSgFunctionDeclaration->isFriend()) )
    {
      // we cannot really make a check beyond this as we don't have access to the class definition
      // within which the friend declaration was made.
      // note that we haven't considered the case of functions which are friends of a given
      // class thanks to a friend class declaration being made.
      // will have to see whether we need this feature or not.
      SgName sageName = pSgFunctionDeclaration->get_name();
      bretValue = isArrayClassFriendFunction(sageName.str());
      if ( bretValue )
	{ // if we have access to the function arguments do case by case checks
	  // now proceed to verify arguments
	  SgInitializedNameList &argList = pSgFunctionDeclaration->get_args();
       // if( ( argList != NULL ) && ( argList.size() == 2) )
	  if( argList.size() == 2 )
	    {
	      SgInitializedNameList ::iterator i = argList.begin(); 
	      bretValue = FALSE;
	      SgType *argTypes[2];
	      int j = 0;
	      while (i != argList.end()) 
		{
		  SgInitializedName argPtr = *i;
		  argTypes[j++] = getType(argPtr.get_type());  // get the basic type
		  i++;
		}
	      // now check for the exact argument types
	      // case 1: both are class types
	      if ( (argTypes[0]->variant() == T_CLASS ) && ( argTypes[1]->variant() == T_CLASS ) )
		{
		  if ( isArrayClassType(argTypes[0]) )
		    {
		      // sorry!!
		      if( !strcmp ( ( ( (SgClassType *)argTypes[0] )->get_name()).str(),( ( (SgClassType *)argTypes[1] )->get_name()).str() ) )
			bretValue = TRUE;
		    }
		}
	      // case 2: one is a class type, the other isn't
	      if ( (argTypes[0]->variant() == T_CLASS ) && ( argTypes[1]->variant() != T_CLASS ) )	      
		{
		  // not a sufficiently concrete test, will modify later
		  if ( isArrayClassType(argTypes[0]) )
		    bretValue = TRUE;
		}
	      // case 3: one is a class type, the other isn't
	      if ( (argTypes[0]->variant() != T_CLASS ) && ( argTypes[1]->variant() == T_CLASS ) )
		{// not a sufficiently concrete test, will modify later
		  if ( isArrayClassType(argTypes[1]) )
		    bretValue = TRUE;
		}	       
	    }
	}
    }

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayClassFriendFunction \n" );

  return bretValue;
}
		

Boolean ArrayClassSageInterface::isArraySectionFriendFunction( SgFunctionDeclaration *pSgFunctionDecl )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArraySectionFriendFunction \n" );

  ROSE_ASSERT( pSgFunctionDecl!= NULL);

  if ( !( bretValue = isArrayIndexFriendFunction( pSgFunctionDecl )) )
    bretValue = isArrayRangeFriendFunction( pSgFunctionDecl );

  // am not adding ROSE_DEBUG statements here as yet. Will/may add them later after talking to Dan

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArraySectionFriendFunction \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayIndexFriendFunction(SgFunctionDeclaration *pSgFunctionDecl)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayIndexFriendFunction \n" );

  ROSE_ASSERT( pSgFunctionDecl!= NULL);

 // the next may/probably be an unnecessary check
  SgFunctionDeclaration *pSgFunctionDeclarationStatement = isSgFunctionDeclaration(pSgFunctionDecl);
  ROSE_ASSERT(pSgFunctionDeclarationStatement);

  SgFunctionDefinition *pSgFunctionDefinition = pSgFunctionDeclarationStatement->get_definition();
  ROSE_ASSERT( pSgFunctionDefinition != NULL );

  SgFunctionDeclaration *pSgFunctionDeclaration = pSgFunctionDefinition->get_declaration();
  ROSE_ASSERT(pSgFunctionDeclaration);

  // make sure it's a friend and also not an operator
  if ( (!pSgFunctionDeclaration->isOperator()) && (pSgFunctionDeclaration->isFriend()) )
    {
      // we cannot really make a check beyond this as we don't have access to the class definition
      // within which the friend declaration was made.
      // note that we haven't considered the case of functions which are friends of a given
      // class thanks to a friend class declaration being made.
      // will have to see whether we need this feature or not.
      SgName sageName = pSgFunctionDeclaration->get_name();
      bretValue = isArrayIndexFriendFunction(sageName.str());
    }

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayIndexFriendFunction \n" );

  return bretValue;
}


Boolean ArrayClassSageInterface::isArrayRangeFriendFunction(SgFunctionDeclaration *pSgFunctionDecl)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayRangeFriendFunction \n" );

  ROSE_ASSERT( pSgFunctionDecl!= NULL);

  // the next may/probably be an unnecessary check
  SgFunctionDeclaration *pSgFunctionDeclarationStatement = isSgFunctionDeclaration(pSgFunctionDecl);
  ROSE_ASSERT(pSgFunctionDeclarationStatement);

  SgFunctionDefinition *pSgFunctionDefinition = pSgFunctionDeclarationStatement->get_definition();
  ROSE_ASSERT( pSgFunctionDefinition != NULL );

  SgFunctionDeclaration *pSgFunctionDeclaration = pSgFunctionDefinition->get_declaration();
  ROSE_ASSERT(pSgFunctionDeclaration);

  // make sure it's a friend and also not an operator
  if ( (!pSgFunctionDeclaration->isOperator()) && (pSgFunctionDeclaration->isFriend()) )
    {
      // we cannot really make a check beyond this as we don't have access to the class definition
      // within which the friend declaration was made.
      // note that we haven't considered the case of functions which are friends of a given
      // class thanks to a friend class declaration being made.
      // will have to see whether we need this feature or not.
      SgName sageName = pSgFunctionDeclaration->get_name();
      bretValue = isArrayRangeFriendFunction(sageName.str());
    }

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayRangeFriendFunction \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayLibOperator ( SgFunctionDeclaration *pSgFunctionDecl )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayLibOperator \n" );

  ROSE_ASSERT( pSgFunctionDecl!= NULL);

  if ( !( bretValue = isArrayLibMemberOperator( pSgFunctionDecl )) )
    bretValue = isArrayLibFriendOperator( pSgFunctionDecl );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayLibOperator \n" );

  return bretValue;
}
		
Boolean ArrayClassSageInterface::isArrayLibMemberOperator( SgFunctionDeclaration *pSgFunctionDecl )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayLibMemberOperator \n" );

  ROSE_ASSERT( pSgFunctionDecl != NULL );

  if ( !( bretValue = isArrayClassMemberOperator( pSgFunctionDecl )) )
    bretValue = isArraySectionMemberOperator( pSgFunctionDecl );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayLibMemberOperator \n" );

  return bretValue;
}


// may need to rewrite the first part as we could get a member fn decl in the end
Boolean ArrayClassSageInterface::isArrayClassMemberOperator(SgFunctionDeclaration *pSgFunctionDecl)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayClassMemberOperator \n" );

  ROSE_ASSERT( pSgFunctionDecl!= NULL);
  SgMemberFunctionDeclaration *pmemberFunctionDeclaration = isSgMemberFunctionDeclaration(pSgFunctionDecl); 
  ROSE_ASSERT ( pmemberFunctionDeclaration != NULL );     // make sure it's a member function

  SgFunctionDefinition *pfunctionDefinition = pmemberFunctionDeclaration->get_definition();
  ROSE_ASSERT (pfunctionDefinition != NULL);

  SgFunctionDeclaration* pfunctionDeclarationStatement = pfunctionDefinition->get_declaration();
  ROSE_ASSERT (pfunctionDeclarationStatement != NULL);


  if ( pfunctionDeclarationStatement->isOperator() )
    {
      // ------------------------------------------------------------------------------ 
      // check whether the class of the function is doubleArray, floatArray, or intArray
      // SUGGEST adding utility function const char *get_class_name(SgClassDefinition *) 
      //  to utility_functions.*

      SgClassDefinition *pclassDefinition = isSgClassDefinition( pmemberFunctionDeclaration->get_scope() );

      // not sure whether base classes should be checked as user defined classes derived from   
      // A++ base classes could be present ?????
      // Alternatively suggest ensuring A++ classes cannot be derived from

      bretValue = isArrayClassType(getClassName(pclassDefinition));

      // ------------------------------------------------------------------------------

      // check whether the function name is an A++ Range member function  
      // from showSgFunctionDeclaration (SgStatement*) 
      // SgFunctionDeclaration* functionDeclarationStatement = isSgFunctionDeclaration( pfunctionDeclarationStatement );
      // ROSE_ASSERT (pfunctionDeclarationStatement != NULL);

      if ( bretValue )
	{
	  SgName sageFunctionName = pfunctionDeclarationStatement->get_name();

	  const char *pfunctionName = sageFunctionName.str();

	  bretValue = isArrayClassMemberOperator(pfunctionName);
	}
      // ------------------------------------------------------------------------------
    }

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayClassMemberOperator \n" );

  return bretValue;
}
						
Boolean ArrayClassSageInterface::isArraySectionMemberOperator( SgFunctionDeclaration *pSgFunctionDecl )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArraySectionMemberOperator \n" );

  ROSE_ASSERT( pSgFunctionDecl!= NULL );

  if ( !( bretValue = isArrayIndexMemberOperator( pSgFunctionDecl )) )
    bretValue = isArrayRangeMemberOperator( pSgFunctionDecl );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArraySectionMemberOperator \n" );

  return bretValue;
}

// may need to rewrite the first part as we could get a member fn decl in the end
Boolean ArrayClassSageInterface::isArrayIndexMemberOperator(SgFunctionDeclaration *pSgFunctionDecl)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayIndexMemberOperator \n" );

  ROSE_ASSERT( pSgFunctionDecl!= NULL);

  SgMemberFunctionDeclaration *pmemberFunctionDeclaration = isSgMemberFunctionDeclaration(pSgFunctionDecl); 
  ROSE_ASSERT ( pmemberFunctionDeclaration != NULL );     // make sure it's a member function

  SgFunctionDefinition *pfunctionDefinition = pmemberFunctionDeclaration->get_definition();
  ROSE_ASSERT (pfunctionDefinition != NULL);

  SgFunctionDeclaration* pfunctionDeclarationStatement = pfunctionDefinition->get_declaration();
  ROSE_ASSERT (pfunctionDeclarationStatement != NULL);

  if ( pfunctionDeclarationStatement->isOperator() )    // ensure we are dealing with an operator
    {
      // ------------------------------------------------------------------------------
      // check whether the class of the function is Index

      // SUGGEST adding utility function const char *get_class_name(SgClassDefinition *)
      // to utility_functions.*
      // for now add the function to the ArrayClassSageInterface class.

      SgClassDefinition *pclassDefinition = isSgClassDefinition( pmemberFunctionDeclaration->get_scope() );

      // not sure whether base classes should be checked as user defined classes derived from  
      // A++ base classes could be present ?????

      bretValue = !strcmp(getClassName(pclassDefinition), "Index") || !strcmp(getClassName(pclassDefinition), "Internal_Index");

      // ------------------------------------------------------------------------------

      // check whether the function name is an A++ Index member function

      // from showSgFunctionDeclaration (SgStatement*)
      // SgFunctionDeclaration* functionDeclarationStatement = isSgFunctionDeclaration( pfunctionDeclarationStatement );
      // ROSE_ASSERT (pfunctionDeclarationStatement != NULL);
      if ( bretValue )
	{

	  SgName sageFunctionName = pfunctionDeclarationStatement->get_name();

	  const char *pfunctionName = sageFunctionName.str();

	  bretValue = isArrayIndexMemberOperator(pfunctionName);
	}

      // ------------------------------------------------------------------------------
    }

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayIndexMemberOperator \n" );

  return bretValue;
}

// may need to rewrite the first part as we could get a member fn decl in the end
Boolean ArrayClassSageInterface::isArrayRangeMemberOperator(SgFunctionDeclaration *pSgFunctionDecl)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayRangeMemberOperator \n" );

  ROSE_ASSERT( pSgFunctionDecl!= NULL);

  SgMemberFunctionDeclaration *pmemberFunctionDeclaration = isSgMemberFunctionDeclaration(pSgFunctionDecl); 
  ROSE_ASSERT ( pmemberFunctionDeclaration != NULL );     // make sure it's a member function

  SgFunctionDefinition *pfunctionDefinition = pmemberFunctionDeclaration->get_definition();
  ROSE_ASSERT (pfunctionDefinition != NULL);

  SgFunctionDeclaration* pfunctionDeclarationStatement = pfunctionDefinition->get_declaration();
  ROSE_ASSERT (pfunctionDeclarationStatement != NULL);

  if(pfunctionDeclarationStatement->isOperator())    // first make sure we are dealing with an operator
    {
      // ------------------------------------------------------------------------------ 
      // check whether the class of the function is Range

      // SUGGEST adding utility function const char *get_class_name(SgClassDefinition *) 
      //  to utility_functions.*

      SgClassDefinition *pclassDefinition = isSgClassDefinition( pmemberFunctionDeclaration->get_scope() );

      // not sure whether base classes should be checked as user defined classes derived from   
      // A++ base classes could be present ?????

      bretValue = !strcmp( getClassName(pclassDefinition), "Range") || !strcmp( getClassName(pclassDefinition), "Internal_Index");

      // ------------------------------------------------------------------------------

      // check whether the function name is an A++ Range member function
 
      // from showSgFunctionDeclaration (SgStatement*)
      // SgFunctionDeclaration* functionDeclarationStatement = isSgFunctionDeclaration( pfunctionDeclarationStatement );
      // ROSE_ASSERT (pfunctionDeclarationStatement != NULL);

      if ( bretValue )
	{
	  
	  SgName sageFunctionName = pfunctionDeclarationStatement->get_name();

	  const char *pfunctionName = sageFunctionName.str();

	  bretValue = isArrayRangeMemberOperator(pfunctionName);
	}

      // ------------------------------------------------------------------------------
    }

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayRangeMemberOperator \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayLibFriendOperator(SgFunctionDeclaration *pSgFunctionDecl)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayLibFriendOperator \n" );

  ROSE_ASSERT( pSgFunctionDecl!= NULL);

  if ( !( bretValue = isArrayClassFriendOperator( pSgFunctionDecl )) )
    bretValue = isArraySectionFriendOperator( pSgFunctionDecl );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayLibFriendOperator \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayClassFriendOperator(SgFunctionDeclaration *pSgFunctionDecl)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayClassFriendOperator \n" );

  ROSE_ASSERT( pSgFunctionDecl!= NULL );
  
  SgFunctionDeclaration *pFunctionDeclaration = isSgFunctionDeclaration(pSgFunctionDecl); 
  ROSE_ASSERT ( pFunctionDeclaration != NULL );     

  SgFunctionDefinition *pfunctionDefinition = pFunctionDeclaration->get_definition();
  ROSE_ASSERT (pfunctionDefinition != NULL);

  SgFunctionDeclaration* pfunctionDeclarationStatement = pfunctionDefinition->get_declaration();
  ROSE_ASSERT (pfunctionDeclarationStatement != NULL);

  // make sure it is an operator and a friend function
  if ( (pfunctionDeclarationStatement->isOperator()) && (pfunctionDeclarationStatement->isFriend() ))
    {
      SgName sageName = pfunctionDeclarationStatement->get_name();
      bretValue = isArrayClassFriendOperator(sageName.str());
      if ( bretValue )
	{ // if the function argument list is available do case by case checks on argument types
	  // we get lucky for now as all the operators have 3 possible sets of arguments and they are the same
	  SgInitializedNameList &argList = pfunctionDeclarationStatement->get_args();
	  if (argList.size() != 2) 
	    bretValue = FALSE;
	  else  
	    {
	      bretValue = FALSE;
	      SgType *argTypes[2];
	      int j = 0;
	      for (SgInitializedNameList::iterator i = argList.begin(); i != argList.end(); i++)
		argTypes[j++] = getType( (*i).get_type() );
	      // now check for the exact argument types
	      // case 1: both are class types
	      if ( (argTypes[0]->variant() == T_CLASS ) && ( argTypes[1]->variant() == T_CLASS ) )
		{
		  if ( isArrayClassType(argTypes[0]) )
		    {
		      // sorry!!
		      if( !strcmp ( ( ( (SgClassType *)argTypes[0] )->get_name()).str(),( ( (SgClassType *)argTypes[1] )->get_name()).str() ) )
			bretValue = TRUE;
		    }
		}
	      // case 2: one is a class type, the other isn't
	      if ( (argTypes[0]->variant() == T_CLASS ) && ( argTypes[1]->variant() != T_CLASS ) )	      
		{
		  // not a sufficiently concrete test, will modify later
		  if ( isArrayClassType(argTypes[0]) )
		    bretValue = TRUE;
		}
	      // case 3: one is a class type, the other isn't
	      if ( (argTypes[0]->variant() != T_CLASS ) && ( argTypes[1]->variant() == T_CLASS ) )
		{
		  if ( isArrayClassType(argTypes[1]) )
		    bretValue = TRUE;
		}
	    }
	}
    }

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayClassFriendOperator \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArraySectionFriendOperator(SgFunctionDeclaration *pSgFunctionDecl)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArraySectionFriendOperator \n" );

  ROSE_ASSERT( pSgFunctionDecl!= NULL);

  if ( !( bretValue = isArrayIndexFriendOperator( pSgFunctionDecl )) )
    bretValue = isArrayRangeFriendOperator( pSgFunctionDecl );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArraySectionFriendOperator \n" );

  return bretValue;
}


Boolean ArrayClassSageInterface::isArrayIndexFriendOperator(SgFunctionDeclaration *pSgFunctionDecl)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayIndexFriendOperator \n" );
  
  bretValue = verifyArraySectionFriendOperatorArguments( pSgFunctionDecl, "Index" );

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayIndexFriendOperator \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayRangeFriendOperator(SgFunctionDeclaration *pSgFunctionDecl)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayRangeFriendOperator \n" );
  
  bretValue = verifyArraySectionFriendOperatorArguments( pSgFunctionDecl, "Range" );

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayRangeFriendOperator \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::verifyArraySectionFriendOperatorArguments(SgFunctionDeclaration *pSgFunctionDecl, const char *sectionType )
{
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::verifyArraySectionFriendOperatorArguments \n" );

  ROSE_ASSERT( pSgFunctionDecl!= NULL);
  SgFunctionDeclaration *pFunctionDeclaration = isSgFunctionDeclaration(pSgFunctionDecl); 
  ROSE_ASSERT ( pFunctionDeclaration != NULL );    

  SgFunctionDefinition *pfunctionDefinition = pFunctionDeclaration->get_definition();
  ROSE_ASSERT (pfunctionDefinition != NULL);

  SgFunctionDeclaration* pfunctionDeclarationStatement = pfunctionDefinition->get_declaration();
  ROSE_ASSERT (pfunctionDeclarationStatement != NULL);

  // make sure it's an operator and a friend function
  if ( (pfunctionDeclarationStatement->isOperator() ) && (pfunctionDeclarationStatement->isFriend()) )
    {
      const char *sName = (pfunctionDeclarationStatement->get_name() ).str();
      bretValue = ( !strcmp("Index", sectionType )) ? \
                  isArrayIndexFriendOperator( sName ) : isArrayRangeFriendOperator( sName );
	
      if ( bretValue )
	{ // if we can perform any checks based on arguments
	  SgInitializedNameList &argList = pfunctionDeclarationStatement->get_args();
	  if (argList.size() != 2) 
	    bretValue = FALSE;
	  else  
	    {
	      bretValue = FALSE;
	      SgType *argTypes[2];
	      int j = 0;
	      for (SgInitializedNameList::iterator i = argList.begin(); i != argList.end(); i++)
		argTypes[j++] = getType( (*i).get_type() );
	      // now check for the exact argument types 
	      // case 1: one is a class type, the other isn't
	      if ( (argTypes[0]->variant() == T_CLASS ) && ( argTypes[1]->variant() == T_INT ) )	      
		{
		      if( !strcmp ( ( ( (SgClassType *)argTypes[0] )->get_name()).str(), sectionType ))
			  bretValue = TRUE;
		}

	      // case 2: one is a class type, the other isn't
	      if ( (argTypes[0]->variant() == T_INT ) && ( argTypes[1]->variant() == T_CLASS ) )
		{
		      if( !strcmp ( ( ( (SgClassType *)argTypes[1] )->get_name()).str(), sectionType ))
			  bretValue = TRUE;
		}
	    }
	}
    }

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::verifyArraySectionFriendOperatorArguments \n" );

  return bretValue;
}

#if 0
// am not sure whether these can exist thanks to my own ignorance!		
Boolean ArrayClassSageInterface::isArrayLibType(SgFunctionDeclaration *)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayLibType \n" );

  ROSE_ASSERT( != NULL);

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayLibType \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayClassType(SgFunctionDeclaration *)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayClassType \n" );

  ROSE_ASSERT( != NULL);

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayClassType \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArraySectionType(SgFunctionDeclaration *)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArraySectionType \n" );

  ROSE_ASSERT( != NULL);

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArraySectionType \n" );

  return bretValue;
}

#endif

// -----------------------------------------------------------------------------------------------------
// next set of definitions with SgSymbol * args
// -----------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isArrayLibGeneralFunction(SgSymbol *pSgSymbol) 
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayLibGeneralFunction \n" );

  ROSE_ASSERT(pSgSymbol != NULL);

  if ( !( bretValue = isArrayLibFunction( pSgSymbol )) )
    bretValue = isArrayLibOperator( pSgSymbol );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayLibGeneralFunction \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayLibFunction(SgSymbol *pSgSymbol)	
{                                           // check if is a member or friend function of an array class
  Boolean bretValue = FALSE;
  
  if( bDebug == verbose_debug_level )
    printf( "Entering ArrayClassSageInterface::isArrayLibFunction \n" );

  ROSE_ASSERT( pSgSymbol!= NULL );

  if ( !( bretValue = isArrayLibMemberFunction( pSgSymbol )) )
    bretValue = isArrayLibFriendFunction( pSgSymbol );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if( bDebug == verbose_debug_level )
    printf( "Exiting ArrayClassSageInterface::isArrayLibFunction \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayLibMemberFunction(SgSymbol *pSgSymbol)		
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayLibMemberFunction \n" );

  ROSE_ASSERT( pSgSymbol != NULL );

  if ( !( bretValue = isArrayClassMemberFunction( pSgSymbol )) )
    bretValue = isArraySectionMemberFunction( pSgSymbol );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayLibMemberFunction \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayClassGeneralFunction(SgSymbol *pSgSymbol)
{
   Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface:isArrayGeneralClassFunction: \n");

  ROSE_ASSERT(pSgSymbol != NULL);
  
  if(!(bretValue = isArrayClassFunction(pSgSymbol)))
    bretValue = isArrayClassOperator(pSgSymbol);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayClassGeneralFunction \n");

  return bretValue; 
}

Boolean ArrayClassSageInterface::isArrayClassFunction(SgSymbol *pSgSymbol)
{
   Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface:isArrayClassFunction: \n");

  ROSE_ASSERT(pSgSymbol != NULL);
  
  if(!(bretValue = isArrayClassMemberFunction(pSgSymbol)))
    bretValue = isArrayClassFriendFunction(pSgSymbol);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayClassFunction \n");

  return bretValue; 
}

Boolean ArrayClassSageInterface::isArrayClassOperator(SgSymbol *pSgSymbol)
{
   Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArrayClassOperator \n");

  ROSE_ASSERT(pSgSymbol != NULL);
  
  if(!(bretValue = isArrayClassMemberOperator(pSgSymbol)))
    bretValue = isArrayClassFriendOperator(pSgSymbol);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayClassOperator \n");

  return bretValue; 
}

// we're given a symbol and should figure out given this information
// to the best of our ability whether it is a member function of an A++ array class
// may have to rewrite code related to member function declaration
Boolean ArrayClassSageInterface::isArrayClassMemberFunction(SgSymbol *pSgSymbol)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayClassMemberFunction \n" );

  ROSE_ASSERT( pSgSymbol!= NULL);
  
  // from showSgSymbol, case MEMBER_FUNC_NAME
  SgMemberFunctionSymbol* pmemberFunctionSymbol = isSgMemberFunctionSymbol( pSgSymbol );
  ROSE_ASSERT ( pmemberFunctionSymbol != NULL );

  SgMemberFunctionDeclaration* pmemberFunctionDeclaration = pmemberFunctionSymbol->get_declaration();
  ROSE_ASSERT ( pmemberFunctionDeclaration!=NULL );

  bretValue = isArrayClassMemberFunction( pmemberFunctionDeclaration );
  
  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayClassMemberFunction \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArraySectionGeneralFunction(SgSymbol *pSgSymbol)
{
   Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface:isArrayGeneralSectionFunction: \n");

  ROSE_ASSERT(pSgSymbol != NULL);
  
  if(!(bretValue = isArraySectionFunction(pSgSymbol)))
    bretValue = isArraySectionOperator(pSgSymbol);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArraySectionGeneralFunction \n");

  return bretValue; 
}

Boolean ArrayClassSageInterface::isArraySectionFunction(SgSymbol *pSgSymbol)
{
   Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface:isArraySectionFunction: \n");

  ROSE_ASSERT(pSgSymbol != NULL);
  
  if(!(bretValue = isArraySectionMemberFunction(pSgSymbol)))
    bretValue = isArraySectionFriendFunction(pSgSymbol);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArraySectionFunction \n");

  return bretValue; 
}

Boolean ArrayClassSageInterface::isArraySectionOperator(SgSymbol *pSgSymbol)
{
   Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArraySectionOperator \n");

  ROSE_ASSERT(pSgSymbol != NULL);
  
  if(!(bretValue = isArraySectionMemberOperator(pSgSymbol)))
    bretValue = isArraySectionFriendOperator(pSgSymbol);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArraySectionOperator \n");

  return bretValue; 
}

Boolean ArrayClassSageInterface::isArraySectionMemberFunction( SgSymbol *pSgSymbol )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArraySectionMemberFunction \n" );

  ROSE_ASSERT( pSgSymbol!= NULL );

  if ( !( bretValue = isArrayIndexMemberFunction( pSgSymbol )) )
    bretValue = isArrayRangeMemberFunction( pSgSymbol );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArraySectionMemberFunction \n" );

  return bretValue;
}

// may have to rewrite code corr to member function decl
Boolean ArrayClassSageInterface::isArrayIndexMemberFunction(SgSymbol *pSgSymbol)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayIndexMemberFunction \n" );

  ROSE_ASSERT( pSgSymbol!= NULL);
 
 // from showSgSymbol, case MEMBER_FUNC_NAME
  SgMemberFunctionSymbol* pmemberFunctionSymbol = isSgMemberFunctionSymbol( pSgSymbol );
  ROSE_ASSERT ( pmemberFunctionSymbol != NULL );

  SgMemberFunctionDeclaration* pmemberFunctionDeclaration = pmemberFunctionSymbol->get_declaration();
  ROSE_ASSERT ( pmemberFunctionDeclaration!=NULL );

  bretValue = isArrayIndexMemberFunction( pmemberFunctionDeclaration );
 
  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayIndexMemberFunction \n" );

  return bretValue;
}

// may have to rewrite code related to member function decl
Boolean ArrayClassSageInterface::isArrayRangeMemberFunction(SgSymbol *pSgSymbol)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayRangeMemberFunction \n" );

  ROSE_ASSERT( pSgSymbol!= NULL);
 
 // from showSgSymbol, case MEMBER_FUNC_NAME
  SgMemberFunctionSymbol* pmemberFunctionSymbol = isSgMemberFunctionSymbol( pSgSymbol );
  ROSE_ASSERT ( pmemberFunctionSymbol != NULL );

  SgMemberFunctionDeclaration* pmemberFunctionDeclaration = pmemberFunctionSymbol->get_declaration();
  ROSE_ASSERT ( pmemberFunctionDeclaration!=NULL );

  bretValue = isArrayRangeMemberFunction( pmemberFunctionDeclaration );

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayRangeMemberFunction \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayLibFriendFunction(SgSymbol *pSgSymbol )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayLibFriendFunction \n" );

  ROSE_ASSERT( pSgSymbol!= NULL );

  if ( !( bretValue = isArrayClassFriendFunction( pSgSymbol )) )
    bretValue = isArraySectionFriendFunction( pSgSymbol );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayLibFriendFunction \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayClassFriendFunction( SgSymbol *pSgSymbol )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayClassFriendFunction \n" );

  ROSE_ASSERT( pSgSymbol!= NULL);

  SgFunctionSymbol* pFunctionSymbol = isSgFunctionSymbol( pSgSymbol );
  ROSE_ASSERT ( pFunctionSymbol != NULL );

  SgFunctionDeclaration* pFunctionDeclaration = pFunctionSymbol->get_declaration();
  ROSE_ASSERT ( pFunctionDeclaration!=NULL );

  bretValue = isArrayClassFriendFunction(pFunctionDeclaration);
  
  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayClassFriendFunction \n" );

  return bretValue;
}
		
Boolean ArrayClassSageInterface::isArraySectionFriendFunction( SgSymbol *pSgSymbol )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArraySectionFriendFunction \n" );

  ROSE_ASSERT( pSgSymbol!= NULL);

  if ( !( bretValue = isArrayIndexFriendFunction( pSgSymbol )) )
    bretValue = isArrayRangeFriendFunction( pSgSymbol );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArraySectionFriendFunction \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayIndexFriendFunction( SgSymbol *pSgSymbol )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayIndexFriendFunction \n" );

  ROSE_ASSERT( pSgSymbol!= NULL);

  SgFunctionSymbol* pFunctionSymbol = isSgFunctionSymbol( pSgSymbol );
  ROSE_ASSERT ( pFunctionSymbol != NULL );

  SgFunctionDeclaration* pFunctionDeclaration = pFunctionSymbol->get_declaration();
  ROSE_ASSERT ( pFunctionDeclaration!=NULL );

  bretValue = isArrayIndexFriendFunction(pFunctionDeclaration);
 
  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayIndexFriendFunction \n" );

  return bretValue;
}


Boolean ArrayClassSageInterface::isArrayRangeFriendFunction( SgSymbol *pSgSymbol )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayRangeFriendFunction \n" );

  ROSE_ASSERT( pSgSymbol!= NULL);

  SgFunctionSymbol* pFunctionSymbol = isSgFunctionSymbol( pSgSymbol );
  ROSE_ASSERT ( pFunctionSymbol != NULL );

  SgFunctionDeclaration* pFunctionDeclaration = pFunctionSymbol->get_declaration();
  ROSE_ASSERT ( pFunctionDeclaration!=NULL );

  bretValue = isArrayRangeFriendFunction(pFunctionDeclaration);
 
  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayRangeFriendFunction \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayLibOperator ( SgSymbol *pSgSymbol )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayLibOperator \n" );

  ROSE_ASSERT( pSgSymbol!= NULL);

  if ( !( bretValue = isArrayLibMemberOperator( pSgSymbol )) )
    bretValue = isArrayLibFriendOperator( pSgSymbol );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayLibOperator \n" );

  return bretValue;
}
		
Boolean ArrayClassSageInterface::isArrayLibMemberOperator( SgSymbol *pSgSymbol )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayLibMemberOperator \n" );

  ROSE_ASSERT( pSgSymbol != NULL );

  if ( !( bretValue = isArrayClassMemberOperator( pSgSymbol )) )
    bretValue = isArraySectionMemberOperator( pSgSymbol );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayLibMemberOperator \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayClassMemberOperator(SgSymbol *pSgSymbol)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayClassMemberOperator \n" );

  ROSE_ASSERT( pSgSymbol!= NULL);
 
  SgFunctionSymbol* pFunctionSymbol = isSgFunctionSymbol( pSgSymbol );
  ROSE_ASSERT ( pFunctionSymbol != NULL );

  SgFunctionDeclaration* pFunctionDeclaration = pFunctionSymbol->get_declaration();
  ROSE_ASSERT ( pFunctionDeclaration!=NULL );

  bretValue = isArrayClassMemberOperator( pFunctionDeclaration );

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayClassMemberOperator \n" );

  return bretValue;
}
						
Boolean ArrayClassSageInterface::isArraySectionMemberOperator( SgSymbol *pSgSymbol )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArraySectionMemberOperator \n" );

  ROSE_ASSERT( pSgSymbol!= NULL );

  if ( !( bretValue = isArrayIndexMemberOperator( pSgSymbol )) )
    bretValue = isArrayRangeMemberOperator( pSgSymbol );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArraySectionMemberOperator \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayIndexMemberOperator(SgSymbol *pSgSymbol)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayIndexMemberOperator \n" );

  ROSE_ASSERT( pSgSymbol!= NULL);

  // since we want a member operator this is a test
  SgMemberFunctionSymbol *pMemberFunctionSymbol = isSgMemberFunctionSymbol( pSgSymbol );

  if ( pMemberFunctionSymbol != NULL )
    {
      SgClassDefinition *pDefn  = pMemberFunctionSymbol->get_scope();
      if( pDefn != NULL )
	{
	  const char *pClassName = getClassName( pDefn );
	  if ( ( !strcmp("Internal_Index", pClassName) ) || ( !strcmp("Internal_Index", pClassName) ) )
	    {
	      SgFunctionDeclaration* pFunctionDeclaration = pMemberFunctionSymbol->get_declaration();
	      ROSE_ASSERT ( pFunctionDeclaration!=NULL );
	      bretValue = isArrayIndexMemberOperator( pFunctionDeclaration );
	    }
	}
    }
  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayIndexMemberOperator \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayRangeMemberOperator(SgSymbol *pSgSymbol)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayRangeMemberOperator \n" );

  ROSE_ASSERT( pSgSymbol!= NULL);

  SgFunctionSymbol* pFunctionSymbol = isSgFunctionSymbol( pSgSymbol );
  ROSE_ASSERT ( pFunctionSymbol != NULL );

  SgFunctionDeclaration* pFunctionDeclaration = pFunctionSymbol->get_declaration();
  ROSE_ASSERT ( pFunctionDeclaration!=NULL );

  bretValue = isArrayRangeMemberOperator( pFunctionDeclaration );

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayRangeMemberOperator \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayLibFriendOperator(SgSymbol *pSgSymbol)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayLibFriendOperator \n" );

  ROSE_ASSERT( pSgSymbol!= NULL);

  if ( !( bretValue = isArrayClassFriendOperator( pSgSymbol )) )
    bretValue = isArraySectionFriendOperator( pSgSymbol );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayLibFriendOperator \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayClassFriendOperator(SgSymbol *pSgSymbol)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayClassFriendOperator \n" );

  ROSE_ASSERT( pSgSymbol!= NULL );

  SgFunctionSymbol* pFunctionSymbol = isSgFunctionSymbol( pSgSymbol );
  ROSE_ASSERT ( pFunctionSymbol != NULL );

  SgFunctionDeclaration* pFunctionDeclaration = pFunctionSymbol->get_declaration();
  ROSE_ASSERT ( pFunctionDeclaration!=NULL );

  bretValue = isArrayClassFriendOperator( pFunctionDeclaration );

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayClassFriendOperator \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArraySectionFriendOperator(SgSymbol *pSgSymbol)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArraySectionFriendOperator \n" );

  ROSE_ASSERT( pSgSymbol!= NULL);

  if ( !( bretValue = isArrayIndexFriendOperator( pSgSymbol )) )
    bretValue = isArrayRangeFriendOperator( pSgSymbol );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArraySectionFriendOperator \n" );

  return bretValue;
}


Boolean ArrayClassSageInterface::isArrayIndexFriendOperator(SgSymbol *pSgSymbol)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayIndexFriendOperator \n" );

  ROSE_ASSERT( pSgSymbol!= NULL);

  SgFunctionSymbol* pFunctionSymbol = isSgFunctionSymbol( pSgSymbol );
  ROSE_ASSERT ( pFunctionSymbol != NULL );

  SgFunctionDeclaration* pFunctionDeclaration = pFunctionSymbol->get_declaration();
  ROSE_ASSERT ( pFunctionDeclaration!=NULL );

  bretValue = isArrayIndexFriendOperator( pFunctionDeclaration );
  
  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayIndexFriendOperator \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayRangeFriendOperator(SgSymbol *pSgSymbol)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayRangeFriendOperator \n" );

  ROSE_ASSERT( pSgSymbol!= NULL);

  SgFunctionSymbol* pFunctionSymbol = isSgFunctionSymbol( pSgSymbol );
  ROSE_ASSERT ( pFunctionSymbol != NULL );

  SgFunctionDeclaration* pFunctionDeclaration = pFunctionSymbol->get_declaration();
  ROSE_ASSERT ( pFunctionDeclaration!=NULL );

  bretValue = isArrayRangeFriendOperator( pFunctionDeclaration );
  
  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayRangeFriendOperator \n" );

  return bretValue;
}

#if 0
// am not sure whether these can exist thanks to my own ignorance!		
Boolean ArrayClassSageInterface::isArrayLibType(SgSymbol *pSgSymbol)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayLibType \n" );

  ROSE_ASSERT( != NULL);

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayLibType \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayClassType(SgSymbol *pSgSymbol)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayClassType \n" );

  ROSE_ASSERT( != NULL);

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayClassType \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArraySectionType(SgSymbol *pSgSymbol)
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArraySectionType \n" );

  ROSE_ASSERT( != NULL);

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArraySectionType \n" );

  return bretValue;
}

#endif


// -----------------------------------------------------------------------------------------------------
// next set of definitions with SgExpression * args
// -----------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isArrayLibGeneralFunction( SgExpression *pSgExpression ) 
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayLibGeneralFunction \n" );

  ROSE_ASSERT(pSgExpression != NULL);

  if ( !( bretValue = isArrayLibFunction( pSgExpression )) )
    bretValue = isArrayLibOperator( pSgExpression );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayLibGeneralFunction \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayLibFunction( SgExpression *pSgExpression )	
{                                           // check if is a member or friend function of an array class
  Boolean bretValue = FALSE;
  
  if( bDebug == verbose_debug_level )
    printf( "Entering ArrayClassSageInterface::isArrayLibFunction \n" );

  ROSE_ASSERT( pSgExpression!= NULL );

  if ( !( bretValue = isArrayLibMemberFunction( pSgExpression )) )
    bretValue = isArrayLibFriendFunction( pSgExpression );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if( bDebug == verbose_debug_level )
    printf( "Exiting ArrayClassSageInterface::isArrayLibFunction \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayLibMemberFunction( SgExpression *pSgExpression )		
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayLibMemberFunction \n" );

  ROSE_ASSERT( pSgExpression != NULL );

  if ( !( bretValue = isArrayClassMemberFunction( pSgExpression )) )
    bretValue = isArraySectionMemberFunction( pSgExpression );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayLibMemberFunction \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayClassGeneralFunction(SgExpression *pSgExpression)
{
   Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface:isArrayGeneralClassFunction: \n");

  ROSE_ASSERT(pSgExpression != NULL);
  
  if(!(bretValue = isArrayClassFunction(pSgExpression)))
    bretValue = isArrayClassOperator(pSgExpression);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayClassGeneralFunction \n");

  return bretValue; 
}

Boolean ArrayClassSageInterface::isArrayClassFunction(SgExpression *pSgExpression)
{
   Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface:isArrayClassFunction: \n");

  ROSE_ASSERT(pSgExpression != NULL);
  
  if(!(bretValue = isArrayClassMemberFunction(pSgExpression)))
    bretValue = isArrayClassFriendFunction(pSgExpression);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayClassFunction \n");

  return bretValue; 
}

Boolean ArrayClassSageInterface::isArrayClassOperator(SgExpression *pSgExpression)
{
   Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArrayClassOperator \n");

  ROSE_ASSERT(pSgExpression != NULL);
  
  if(!(bretValue = isArrayClassMemberOperator(pSgExpression)))
    bretValue = isArrayClassFriendOperator(pSgExpression);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArrayClassOperator \n");

  return bretValue; 
}

// we're given a symbol and should figure out given this information
// to the best of our ability whether it is a member function of an A++ array class

Boolean ArrayClassSageInterface::isArrayClassMemberFunction( SgExpression *pSgExpression )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayClassMemberFunction \n" );

  ROSE_ASSERT( pSgExpression!= NULL);

  // this function is by no means assumed to be complete, additions will be made from time
  // to time as we learn more.
  // we only consider the cases SgFunctionCallExp, SgFunctionRefExp
  // SgFunctionCallExp may in turn deal with SgDotExp, SgArrowExp, SgFunctionRefExp

  switch ( pSgExpression->variant() )
    {
    case MEMBER_FUNCTION_REF:
      {
	SgMemberFunctionRefExp *pMemberFunctionReference = isSgMemberFunctionRefExp(pSgExpression);
	ROSE_ASSERT ( pMemberFunctionReference != NULL );
	
	SgMemberFunctionSymbol *pMemberFunctionSymbol = pMemberFunctionReference->get_symbol();
	bretValue = isArrayClassMemberFunction( pMemberFunctionSymbol );
	break;
      }
    case FUNC_CALL: 
      {
	SgFunctionCallExp *pFunctionCallExp = isSgFunctionCallExp(pSgExpression);
	ROSE_ASSERT ( pFunctionCallExp != NULL );

	SgExpression *pFunctionExp = pFunctionCallExp->get_function();
	ROSE_ASSERT ( pFunctionExp!= NULL );
	bretValue = isArrayClassMemberFunction( pFunctionExp );   // recursive call
	break;
      }
    case POINTST_OP:    // am assuming this is the arrow operator, though we'll have to make sure
      {
	SgArrowExp *pArrowExp = isSgArrowExp(pSgExpression);
	ROSE_ASSERT ( pArrowExp != NULL );
	bretValue = isArrayClassMemberFunction( pArrowExp->get_rhs_operand() );
	break;
      }
    case RECORD_REF:    
      {
	SgDotExp *pDotExp = isSgDotExp(pSgExpression);
	ROSE_ASSERT ( pDotExp != NULL );
	// commented out because it interferes with the editing of the Sage tree that ROSE does
#if 0
	// first perform a check on the LHS operand and ensure that
	// it is of array class type

	SgExpression *pSgLhsExpression = pDotExp->get_lhs_operand();
	ROSE_ASSERT ( pSgLhsExpression != NULL );
	if ( pSgLhsExpression->variant() == VAR_REF )
	  {
	    SgVariableSymbol *pVarSymbol = ((SgVarRefExp *)pSgLhsExpression)->get_symbol();
	    SgInitializedName *pDeclaration = pVarSymbol->get_declaration();
	    SgType *pType = pDeclaration->get_type();
	    ROSE_ASSERT ( pType != NULL );
	    if( isArrayClassType(pType) )
	      bretValue = isArrayClassMemberFunction( pDotExp->get_rhs_operand() );
	  }
#endif
	 bretValue = isArrayClassMemberFunction( pDotExp->get_rhs_operand() );
	break;
      }
    case CONSTRUCTOR_INIT:
      {
	SgConstructorInitializer *pSgConstructorInitializer = isSgConstructorInitializer( pSgExpression );
	ROSE_ASSERT (pSgConstructorInitializer!= NULL );
	SgClassDeclaration *pSgClassDeclaration = pSgConstructorInitializer->get_class_decl();
	SgName sageName = pSgClassDeclaration->get_name();
	bretValue = isArrayClassType( sageName.str() );
	break;
      }
    default:
      {
      break;
      }
    }

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayClassMemberFunction \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArraySectionGeneralFunction(SgExpression *pSgExpression)
{
   Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface:isArrayGeneralSectionFunction: \n");

  ROSE_ASSERT(pSgExpression != NULL);
  
  if(!(bretValue = isArraySectionFunction(pSgExpression)))
    bretValue = isArraySectionOperator(pSgExpression);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArraySectionGeneralFunction \n");

  return bretValue; 
}

Boolean ArrayClassSageInterface::isArraySectionFunction(SgExpression *pSgExpression)
{
   Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface:isArraySectionFunction: \n");

  ROSE_ASSERT(pSgExpression != NULL);
  
  if(!(bretValue = isArraySectionMemberFunction(pSgExpression)))
    bretValue = isArraySectionFriendFunction(pSgExpression);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArraySectionFunction \n");

  return bretValue; 
}

Boolean ArrayClassSageInterface::isArraySectionOperator(SgExpression *pSgExpression)
{
   Boolean bretValue = FALSE;

  if (bDebug == verbose_debug_level)
    printf("Entered ArrayClassSageInterface::isArraySectionOperator \n");

  ROSE_ASSERT(pSgExpression != NULL);
  
  if(!(bretValue = isArraySectionMemberOperator(pSgExpression)))
    bretValue = isArraySectionFriendOperator(pSgExpression);

  if (bDebug == verbose_debug_level)
    printf("Exiting ArrayClassSageInterface::isArraySectionOperator \n");

  return bretValue; 
}

Boolean ArrayClassSageInterface::isArraySectionMemberFunction(  SgExpression *pSgExpression  )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArraySectionMemberFunction \n" );

  ROSE_ASSERT( pSgExpression!= NULL );

  if ( !( bretValue = isArrayIndexMemberFunction( pSgExpression )) )
    bretValue = isArrayRangeMemberFunction( pSgExpression );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArraySectionMemberFunction \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayIndexMemberFunction( SgExpression *pSgExpression )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayIndexMemberFunction \n" );

  ROSE_ASSERT( pSgExpression!= NULL);

  // this function is by no means assumed to be complete, additions will be made from time
  // to time as we learn more.
  // we only consider the cases SgFunctionCallExp, SgFunctionRefExp
  // SgFunctionCallExp may in turn deal with SgDotExp, SgArrowExp, SgFunctionRefExp

  switch ( pSgExpression->variant() )
    {
    case MEMBER_FUNCTION_REF:
      {
	SgMemberFunctionRefExp *pMemberFunctionReference = isSgMemberFunctionRefExp(pSgExpression);
	ROSE_ASSERT ( pMemberFunctionReference != NULL );

	SgMemberFunctionSymbol *pMemberFunctionSymbol = pMemberFunctionReference->get_symbol();
	bretValue = isArrayIndexMemberFunction( pMemberFunctionSymbol );
	break;
      }
    case FUNC_CALL: 
      {
	SgFunctionCallExp *pFunctionCallExp = isSgFunctionCallExp(pSgExpression);
	ROSE_ASSERT ( pFunctionCallExp != NULL );
	
	SgExpression *pFunctionExp = pFunctionCallExp->get_function();
	ROSE_ASSERT ( pFunctionExp!= NULL );
	bretValue = isArrayIndexMemberFunction( pFunctionExp );   // recursive call
	break;
      }
    case POINTST_OP:    // am assuming this is the arrow operator, though we'll have to make sure
      {
	SgArrowExp *pArrowExp = isSgArrowExp(pSgExpression);
	ROSE_ASSERT ( pArrowExp != NULL );
	bretValue = isArrayIndexMemberFunction( pArrowExp->get_rhs_operand() );
	break;
      }
    case RECORD_REF:    
      {
	SgDotExp *pDotExp = isSgDotExp(pSgExpression);
	ROSE_ASSERT ( pDotExp != NULL );
	
	// the code to test the LHS operand only does a check 
	// for the variable reference case. we should probably
	// have to rewrite this for more complicated cases
	SgExpression *pSgLhsExpression = pDotExp->get_lhs_operand();
	ROSE_ASSERT ( pSgLhsExpression != NULL );
#if 0 // interferes with editing of the expression tree
	if ( pSgLhsExpression->variant() == VAR_REF )
	  {
	    SgVariableSymbol *pVarSymbol = ((SgVarRefExp *)pSgLhsExpression)->get_symbol();
	    SgInitializedName *pDeclaration = pVarSymbol->get_declaration();
	    SgType *pSgType = pDeclaration->get_type();
	    ROSE_ASSERT ( pSgType != NULL );

	    SgClassType* pClassType = isSgClassType( pSgType );
	    ROSE_ASSERT (pClassType != NULL);
	    SgName sageName = pClassType->get_name();
	    if ( !strcmp ( sageName.str(), "Index" ) )
	      bretValue = isArrayIndexMemberFunction( pDotExp->get_rhs_operand() );
	  }
#endif
	bretValue = isArrayIndexMemberFunction( pDotExp->get_rhs_operand() );
      break;
      }
    case CONSTRUCTOR_INIT:
      {
	SgConstructorInitializer *pSgConstructorInitializer = isSgConstructorInitializer( pSgExpression );
	ROSE_ASSERT (pSgConstructorInitializer!= NULL );
	SgClassDeclaration *pSgClassDeclaration = pSgConstructorInitializer->get_class_decl();
	SgName sageName = pSgClassDeclaration->get_name();
	bretValue = !strcmp ( sageName.str(), "Index" );
	break;
      }
    default:
      {
      break;
      }
    }

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayIndexMemberFunction \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayRangeMemberFunction( SgExpression *pSgExpression )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayRangeMemberFunction \n" );

  ROSE_ASSERT( pSgExpression!= NULL);
  // this function is by no means assumed to be complete, additions will be made from time
  // to time as we learn more.
  // we only consider the cases SgFunctionCallExp, SgFunctionRefExp
  // SgFunctionCallExp may in turn deal with SgDotExp, SgArrowExp, SgFunctionRefExp

  switch ( pSgExpression->variant() )
    {
    case MEMBER_FUNCTION_REF:
      {
	SgMemberFunctionRefExp *pMemberFunctionReference = isSgMemberFunctionRefExp(pSgExpression);
	ROSE_ASSERT ( pMemberFunctionReference != NULL );

	SgMemberFunctionSymbol *pMemberFunctionSymbol = pMemberFunctionReference->get_symbol();
	bretValue = isArrayRangeMemberFunction( pMemberFunctionSymbol );
	break;
      }
    case FUNC_CALL: 
      {
	SgFunctionCallExp *pFunctionCallExp = isSgFunctionCallExp(pSgExpression);
	ROSE_ASSERT ( pFunctionCallExp != NULL );

	SgExpression *pFunctionExp = pFunctionCallExp->get_function();
	ROSE_ASSERT ( pFunctionExp!= NULL );
	bretValue = isArrayRangeMemberFunction( pFunctionExp );   // recursive call
	break;
      }
    case POINTST_OP:    // am assuming this is the arrow operator, though we'll have to make sure
      {
	SgArrowExp *pArrowExp = isSgArrowExp(pSgExpression);
	ROSE_ASSERT ( pArrowExp != NULL );
	bretValue = isArrayRangeMemberFunction( pArrowExp->get_rhs_operand() );
	break;
      }
    case RECORD_REF:    
      {
	
	// the code to test the LHS operand only does a check 
	// for the variable reference case. we should probably 
	// have to rewrite this for more complicated cases
	SgDotExp *pDotExp = isSgDotExp(pSgExpression);
	ROSE_ASSERT ( pDotExp != NULL );
#if 0 // interferes with editing of Sage tree by ROSE
	SgExpression *pSgLhsExpression = pDotExp->get_lhs_operand();
	ROSE_ASSERT ( pSgLhsExpression != NULL );
	if ( pSgLhsExpression->variant() == VAR_REF )
	  {
	    SgVariableSymbol *pVarSymbol = ( (SgVarRefExp *) pSgLhsExpression)->get_symbol();
	    SgInitializedName *pDeclaration = pVarSymbol->get_declaration();
	    SgType *pSgType = pDeclaration->get_type();
	    ROSE_ASSERT ( pSgType != NULL );

	    SgClassType* pClassType = isSgClassType( pSgType );
	    ROSE_ASSERT (pClassType != NULL);
	    SgName sageName = pClassType->get_name();
	    if ( !strcmp ( sageName.str(), "Range" ) )
	      bretValue = isArrayRangeMemberFunction( pDotExp->get_rhs_operand() );
	  }
#endif
	
	bretValue = isArrayRangeMemberFunction( pDotExp->get_rhs_operand() );
	break;
      }
    case CONSTRUCTOR_INIT:
      {
	SgConstructorInitializer *pSgConstructorInitializer = isSgConstructorInitializer( pSgExpression );
	ROSE_ASSERT (pSgConstructorInitializer!= NULL );
	SgClassDeclaration *pSgClassDeclaration = pSgConstructorInitializer->get_class_decl();
	SgName sageName = pSgClassDeclaration->get_name();
	bretValue = !strcmp ( sageName.str(), "Range" );
	break;
      }
    default:
      {
	break;
      }
    }

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayRangeMemberFunction \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayLibFriendFunction( SgExpression *pSgExpression  )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayLibFriendFunction \n" );

  ROSE_ASSERT( pSgExpression!= NULL );

  if ( !( bretValue = isArrayClassFriendFunction( pSgExpression )) )
    bretValue = isArraySectionFriendFunction( pSgExpression );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayLibFriendFunction \n" );

  return bretValue;
}


Boolean ArrayClassSageInterface::isArrayClassFriendFunction(  SgExpression *pSgExpression  )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayClassFriendFunction \n" );

  ROSE_ASSERT( pSgExpression!= NULL);

  // this function is by no means assumed to be complete, additions will be made from time
  // to time as we learn more.
  // we only consider the cases SgFunctionCallExp, SgFunctionRefExp
  // SgFunctionCallExp may in turn deal with SgDotExp, SgArrowExp, SgFunctionRefExp

  switch ( pSgExpression->variant() )
    {
    case FUNC_CALL: 
      {
	SgFunctionCallExp *pFunctionCallExp = isSgFunctionCallExp(pSgExpression);
	ROSE_ASSERT ( pFunctionCallExp != NULL );

	SgExpression *pFunctionExp = pFunctionCallExp->get_function();
	ROSE_ASSERT ( pFunctionExp!= NULL );
	bretValue = isArrayClassFriendFunction( pFunctionExp );   // recursive call
	break;
      }
    case FUNCTION_REF:
      {
	SgFunctionRefExp *pFunctionRefExp = isSgFunctionRefExp(pSgExpression);
	ROSE_ASSERT ( pFunctionRefExp != NULL );
      
	SgFunctionSymbol *pFunctionSymbol = pFunctionRefExp->get_symbol();
	bretValue = isArrayClassFriendFunction( pFunctionSymbol );
	break;
      }
    default:
      {
	break;
      }
    }

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayClassFriendFunction \n" );

  return bretValue;
}
		
Boolean ArrayClassSageInterface::isArraySectionFriendFunction(  SgExpression *pSgExpression  )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArraySectionFriendFunction \n" );

  ROSE_ASSERT( pSgExpression!= NULL);

  if ( !( bretValue = isArrayIndexFriendFunction( pSgExpression )) )
    bretValue = isArrayRangeFriendFunction( pSgExpression );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArraySectionFriendFunction \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayIndexFriendFunction(  SgExpression *pSgExpression  )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayIndexFriendFunction \n" );

  ROSE_ASSERT( pSgExpression!= NULL);

  // this function is by no means assumed to be complete, additions will be made from time
  // to time as we learn more.
  // we only consider the cases SgFunctionCallExp, SgFunctionRefExp
  // SgFunctionCallExp may in turn deal with SgDotExp, SgArrowExp, SgFunctionRefExp

  switch ( pSgExpression->variant() )
    {
    case FUNC_CALL: 
      {
	SgFunctionCallExp *pFunctionCallExp = isSgFunctionCallExp(pSgExpression);
	ROSE_ASSERT ( pFunctionCallExp != NULL );

	SgExpression *pFunctionExp = pFunctionCallExp->get_function();
	ROSE_ASSERT ( pFunctionExp!= NULL );
	bretValue = isArrayIndexFriendFunction( pFunctionExp );   // recursive call
	break;
      }
    case FUNCTION_REF:
      {
	SgFunctionRefExp *pFunctionRefExp = isSgFunctionRefExp(pSgExpression);
	ROSE_ASSERT ( pFunctionRefExp != NULL );
      
	SgFunctionSymbol *pFunctionSymbol = pFunctionRefExp->get_symbol();
	bretValue = isArrayIndexFriendFunction( pFunctionSymbol );
	break;
      }
    default:
      {
	break;
      }
    }

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayIndexFriendFunction \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayRangeFriendFunction(  SgExpression *pSgExpression  )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayRangeFriendFunction \n" );

  ROSE_ASSERT( pSgExpression!= NULL);

  // this function is by no means assumed to be complete, additions will be made from time
  // to time as we learn more.
  // we only consider the cases SgFunctionCallExp, SgFunctionRefExp
  // SgFunctionCallExp may in turn deal with SgDotExp, SgArrowExp, SgFunctionRefExp

  switch ( pSgExpression->variant() )
    {
    case FUNC_CALL: 
      {
	SgFunctionCallExp *pFunctionCallExp = isSgFunctionCallExp(pSgExpression);
	ROSE_ASSERT ( pFunctionCallExp != NULL );

	SgExpression *pFunctionExp = pFunctionCallExp->get_function();
	ROSE_ASSERT ( pFunctionExp!= NULL );
	bretValue = isArrayRangeFriendFunction( pFunctionExp );   // recursive call
	break;
      }
    case FUNCTION_REF:
      {
	SgFunctionRefExp *pFunctionRefExp = isSgFunctionRefExp(pSgExpression);
	ROSE_ASSERT ( pFunctionRefExp != NULL );
      
	SgFunctionSymbol *pFunctionSymbol = pFunctionRefExp->get_symbol();
	bretValue = isArrayRangeFriendFunction( pFunctionSymbol );
	break;
      }
    default:
      {
	break;
      }
    }

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayRangeFriendFunction \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayLibOperator (  SgExpression *pSgExpression  )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayLibOperator \n" );

  ROSE_ASSERT( pSgExpression!= NULL);

  if ( !( bretValue = isArrayLibMemberOperator( pSgExpression )) )
    bretValue = isArrayLibFriendOperator( pSgExpression );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayLibOperator \n" );

  return bretValue;
}
		
Boolean ArrayClassSageInterface::isArrayLibMemberOperator(  SgExpression *pSgExpression  )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayLibMemberOperator \n" );

  ROSE_ASSERT( pSgExpression != NULL );

  if ( !( bretValue = isArrayClassMemberOperator( pSgExpression )) )
    bretValue = isArraySectionMemberOperator( pSgExpression );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayLibMemberOperator \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayClassMemberOperator( SgExpression *pSgExpression )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayClassMemberOperator \n" );

  ROSE_ASSERT( pSgExpression!= NULL);
  // this function is by no means assumed to be complete, additions will be made from time
  // to time as we learn more.
  // we only consider the cases SgFunctionCallExp, SgFunctionRefExp
  // SgFunctionCallExp may in turn deal with SgDotExp, SgArrowExp, SgFunctionRefExp

  switch ( pSgExpression->variant() )
    {
    case MEMBER_FUNCTION_REF:
      {
	SgMemberFunctionRefExp *pMemberFunctionReference = isSgMemberFunctionRefExp(pSgExpression);
	ROSE_ASSERT ( pMemberFunctionReference != NULL );

	SgMemberFunctionSymbol *pMemberFunctionSymbol = pMemberFunctionReference->get_symbol();
	bretValue = isArrayClassMemberOperator( pMemberFunctionSymbol );
	break;
      }
    case FUNC_CALL: 
      {
	SgFunctionCallExp *pFunctionCallExp = isSgFunctionCallExp(pSgExpression);
	ROSE_ASSERT ( pFunctionCallExp != NULL );

	SgExpression *pFunctionExp = pFunctionCallExp->get_function();
	ROSE_ASSERT ( pFunctionExp!= NULL );
	bretValue = isArrayClassMemberOperator( pFunctionExp );   // recursive call
	break;
      }
    case POINTST_OP:    // am assuming this is the arrow operator, though we'll have to make sure
      {
	SgArrowExp *pArrowExp = isSgArrowExp(pSgExpression);
	ROSE_ASSERT ( pArrowExp != NULL );
        bretValue = isArrayClassMemberOperator( pArrowExp->get_rhs_operand() );
	break;
      }
    case RECORD_REF:    
      {
	SgDotExp *pDotExp = isSgDotExp(pSgExpression);
	ROSE_ASSERT ( pDotExp != NULL );
#if 0 // interferes with editing of Sage Tree by ROSE
	SgExpression *pSgLhsExpression = pDotExp->get_lhs_operand();
	ROSE_ASSERT ( pSgLhsExpression != NULL );
	if ( pSgLhsExpression->variant() == VAR_REF )
	  {
	    SgVariableSymbol *pVarSymbol = ( (SgVarRefExp *) pSgLhsExpression)->get_symbol();
	    SgInitializedName *pDeclaration = pVarSymbol->get_declaration();
	    SgType *pSgType = pDeclaration->get_type();
	    ROSE_ASSERT ( pSgType != NULL );
	    if( isArrayClassType( pSgType ) )
	      bretValue = isArrayClassMemberOperator( pDotExp->get_rhs_operand() );
	  }
#endif
        bretValue = isArrayClassMemberOperator( pDotExp->get_rhs_operand() );
	break;
      }
    default:
      {
	break;
      }
    }

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayClassMemberOperator \n" );

  return bretValue;
}
						
Boolean ArrayClassSageInterface::isArraySectionMemberOperator(  SgExpression *pSgExpression  )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArraySectionMemberOperator \n" );

  ROSE_ASSERT( pSgExpression!= NULL );

  if ( !( bretValue = isArrayIndexMemberOperator( pSgExpression )) )
    bretValue = isArrayRangeMemberOperator( pSgExpression );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArraySectionMemberOperator \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayIndexMemberOperator( SgExpression *pSgExpression )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayIndexMemberOperator \n" );

  ROSE_ASSERT( pSgExpression!= NULL);
  // this function is by no means assumed to be complete, additions will be made from time
  // to time as we learn more.
  // we only consider the cases SgFunctionCallExp, SgFunctionRefExp
  // SgFunctionCallExp may in turn deal with SgDotExp, SgArrowExp, SgFunctionRefExp

  switch ( pSgExpression->variant() )
    {
    case MEMBER_FUNCTION_REF:
      {
	SgMemberFunctionRefExp *pMemberFunctionReference = isSgMemberFunctionRefExp(pSgExpression);
	ROSE_ASSERT ( pMemberFunctionReference != NULL );
	
	SgMemberFunctionSymbol *pMemberFunctionSymbol = pMemberFunctionReference->get_symbol();
	bretValue = isArrayIndexMemberOperator( pMemberFunctionSymbol );
	break;
      }
    case FUNC_CALL: 
      {
	SgFunctionCallExp *pFunctionCallExp = isSgFunctionCallExp(pSgExpression);
	ROSE_ASSERT ( pFunctionCallExp != NULL );

	SgExpression *pFunctionExp = pFunctionCallExp->get_function();
	ROSE_ASSERT ( pFunctionExp!= NULL );
	bretValue = isArrayIndexMemberOperator( pFunctionExp );   // recursive call
	break;
      }
    case POINTST_OP:    // am assuming this is the arrow operator, though we'll have to make sure
      {
	SgArrowExp *pArrowExp = isSgArrowExp(pSgExpression);
	ROSE_ASSERT ( pArrowExp != NULL );
	bretValue = isArrayIndexMemberOperator ( pArrowExp->get_rhs_operand());
	break;
      }
    case RECORD_REF:    
      {
	// the code to test the LHS operand only does a check 
	// for the variable reference case. we should probably 
	// have to rewrite this for more complicated cases
	SgDotExp *pDotExp = isSgDotExp(pSgExpression);
	ROSE_ASSERT ( pDotExp != NULL );
#if 0 // interferes with editing of Sage Tree by ROSE
	SgExpression *pSgLhsExpression = pDotExp->get_lhs_operand();
	ROSE_ASSERT ( pSgLhsExpression != NULL );
	if ( pSgLhsExpression->variant() == VAR_REF )
	  {
	    SgVariableSymbol *pVarSymbol = ( (SgVarRefExp *) pSgLhsExpression)->get_symbol();
	    SgInitializedName *pDeclaration = pVarSymbol->get_declaration();
	    SgType *pSgType = pDeclaration->get_type();
	    ROSE_ASSERT ( pSgType != NULL );

	    SgClassType* pClassType = isSgClassType( pSgType );
	    ROSE_ASSERT (pClassType != NULL);
	    SgName sageName = pClassType->get_name();
	    if ( !strcmp ( sageName.str(), "Index" ) )
	      bretValue = isArrayIndexMemberOperator( pDotExp->get_rhs_operand() );
	  }
#endif
	bretValue = isArrayIndexMemberOperator( pDotExp->get_rhs_operand() );
	break;
      }
    default:
      {
	break;
      }
    }

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayIndexMemberOperator \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayRangeMemberOperator( SgExpression *pSgExpression )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayRangeMemberOperator \n" );

  ROSE_ASSERT( pSgExpression!= NULL);
  // this function is by no means assumed to be complete, additions will be made from time
  // to time as we learn more.
  // we only consider the cases SgFunctionCallExp, SgFunctionRefExp
  // SgFunctionCallExp may in turn deal with SgDotExp, SgArrowExp, SgFunctionRefExp

  switch ( pSgExpression->variant() )
    {
    case MEMBER_FUNCTION_REF:
      {
	SgMemberFunctionRefExp *pMemberFunctionReference = isSgMemberFunctionRefExp(pSgExpression);
	ROSE_ASSERT ( pMemberFunctionReference != NULL );

	SgMemberFunctionSymbol *pMemberFunctionSymbol = pMemberFunctionReference->get_symbol();
	bretValue = isArrayRangeMemberOperator( pMemberFunctionSymbol );
	break;
      }
    case FUNC_CALL: 
      {
	SgFunctionCallExp *pFunctionCallExp = isSgFunctionCallExp(pSgExpression);
	ROSE_ASSERT ( pFunctionCallExp != NULL );

	SgExpression *pFunctionExp = pFunctionCallExp->get_function();
	ROSE_ASSERT ( pFunctionExp!= NULL );
	bretValue = isArrayRangeMemberOperator( pFunctionExp );   // recursive call
	break;
      }
    case POINTST_OP:    // am assuming this is the arrow operator, though we'll have to make sure
      {
	SgArrowExp *pArrowExp = isSgArrowExp(pSgExpression);
	ROSE_ASSERT ( pArrowExp != NULL );
	bretValue = isArrayRangeMemberOperator( pArrowExp->get_rhs_operand() );
	break;
      }
    case RECORD_REF:    
      {
	// the code to test the LHS operand only does a check 
	// for the variable reference case. we should probably 
	// have to rewrite this for more complicated cases
	SgDotExp *pDotExp = isSgDotExp(pSgExpression);
	ROSE_ASSERT ( pDotExp != NULL );
#if 0 // interferes with editing of Sage tree by ROSE
	SgExpression *pSgLhsExpression = pDotExp->get_lhs_operand();
	ROSE_ASSERT ( pSgLhsExpression != NULL );
	if ( pSgLhsExpression->variant() == VAR_REF )
	  {
	    SgVariableSymbol *pVarSymbol = ( (SgVarRefExp *) pSgLhsExpression)->get_symbol();
	    SgInitializedName *pDeclaration = pVarSymbol->get_declaration();
	    SgType *pSgType = pDeclaration->get_type();
	    ROSE_ASSERT ( pSgType != NULL );

	    SgClassType* pClassType = isSgClassType( pSgType );
	    ROSE_ASSERT (pClassType != NULL);
	    SgName sageName = pClassType->get_name();
	    if ( !strcmp ( sageName.str(), "Range" ) )
	      bretValue = isArrayRangeMemberOperator( pDotExp->get_rhs_operand() );
	  }
#endif
	bretValue = isArrayRangeMemberOperator( pDotExp->get_rhs_operand() );
	break;
      }
    case CONSTRUCTOR_INIT:
      {
	break;
      }
    default:
      {
	break;
      }
    }

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayRangeMemberOperator \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayLibFriendOperator( SgExpression *pSgExpression )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayLibFriendOperator \n" );

  ROSE_ASSERT( pSgExpression!= NULL);

  if ( !( bretValue = isArrayClassFriendOperator( pSgExpression )) )
    bretValue = isArraySectionFriendOperator( pSgExpression );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayLibFriendOperator \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayClassFriendOperator( SgExpression *pSgExpression )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayClassFriendOperator \n" );

  ROSE_ASSERT( pSgExpression!= NULL );
  

  // this function is by no means assumed to be complete, additions will be made from time
  // to time as we learn more.
  // we only consider the cases SgFunctionCallExp, SgFunctionRefExp
  // SgFunctionCallExp may in turn deal with SgDotExp, SgArrowExp, SgFunctionRefExp

  switch ( pSgExpression->variant() )
    {
    case FUNC_CALL: 
      {
	SgFunctionCallExp *pFunctionCallExp = isSgFunctionCallExp(pSgExpression);
	ROSE_ASSERT ( pFunctionCallExp != NULL );

	SgExpression *pFunctionExp = pFunctionCallExp->get_function();
	ROSE_ASSERT ( pFunctionExp!= NULL );
	bretValue = isArrayClassFriendOperator( pFunctionExp );   // recursive call
	break;
      }
    case FUNCTION_REF:
      {
	SgFunctionRefExp *pFunctionRefExp = isSgFunctionRefExp(pSgExpression);
	ROSE_ASSERT ( pFunctionRefExp != NULL );
      
	SgFunctionSymbol *pFunctionSymbol = pFunctionRefExp->get_symbol();
	bretValue = isArrayClassFriendOperator( pFunctionSymbol );
	break;
      }
    default:
      {
	break;
      }
    }

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayClassFriendOperator \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArraySectionFriendOperator( SgExpression *pSgExpression )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArraySectionFriendOperator \n" );

  ROSE_ASSERT( pSgExpression!= NULL);

  if ( !( bretValue = isArrayIndexFriendOperator( pSgExpression )) )
    bretValue = isArrayRangeFriendOperator( pSgExpression );

  // am not adding bDebug statements here as yet. Will/may add them later after talking to Dan

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArraySectionFriendOperator \n" );

  return bretValue;
}


Boolean ArrayClassSageInterface::isArrayIndexFriendOperator( SgExpression *pSgExpression )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayIndexFriendOperator \n" );

  ROSE_ASSERT( pSgExpression!= NULL);
  // this function is by no means assumed to be complete, additions will be made from time
  // to time as we learn more.
  // we only consider the cases SgFunctionCallExp, SgFunctionRefExp
  // SgFunctionCallExp may in turn deal with SgDotExp, SgArrowExp, SgFunctionRefExp

  switch ( pSgExpression->variant() )
    {
    case FUNC_CALL: 
      {
	SgFunctionCallExp *pFunctionCallExp = isSgFunctionCallExp(pSgExpression);
	ROSE_ASSERT ( pFunctionCallExp != NULL );

	SgExpression *pFunctionExp = pFunctionCallExp->get_function();
	ROSE_ASSERT ( pFunctionExp!= NULL );
	bretValue = isArrayIndexFriendOperator( pFunctionExp );   // recursive call
	break;
      }
    case FUNCTION_REF:
      {
	SgFunctionRefExp *pFunctionRefExp = isSgFunctionRefExp(pSgExpression);
	ROSE_ASSERT ( pFunctionRefExp != NULL );
	
	SgFunctionSymbol *pFunctionSymbol = pFunctionRefExp->get_symbol();
	bretValue = isArrayIndexFriendOperator( pFunctionSymbol );
	break;
      }
    default:
      {
	break;
      }
    }

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayIndexFriendOperator \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayRangeFriendOperator( SgExpression *pSgExpression )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayRangeFriendOperator \n" );

  ROSE_ASSERT( pSgExpression!= NULL);
  // this function is by no means assumed to be complete, additions will be made from time
  // to time as we learn more.
  // we only consider the cases SgFunctionCallExp, SgFunctionRefExp
  // SgFunctionCallExp may in turn deal with SgDotExp, SgArrowExp, SgFunctionRefExp

  switch ( pSgExpression->variant() )
    {
    case FUNC_CALL: 
      {
	SgFunctionCallExp *pFunctionCallExp = isSgFunctionCallExp(pSgExpression);
	ROSE_ASSERT ( pFunctionCallExp != NULL );

	SgExpression *pFunctionExp = pFunctionCallExp->get_function();
	ROSE_ASSERT ( pFunctionExp!= NULL );
	bretValue = isArrayRangeFriendOperator( pFunctionExp );   // recursive call
	break;
      }
    case FUNCTION_REF:
      {
	SgFunctionRefExp *pFunctionRefExp = isSgFunctionRefExp(pSgExpression);
	ROSE_ASSERT ( pFunctionRefExp != NULL );
      
	SgFunctionSymbol *pFunctionSymbol = pFunctionRefExp->get_symbol();
	bretValue = isArrayRangeFriendOperator( pFunctionSymbol );
	break;
      }
    default:
      {
	break;
      }
    }

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayRangeFriendOperator \n" );

  return bretValue;
}

#if 0
Boolean ArrayClassSageInterface::isArrayLibType( SgExpression *pSgExpression )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayLibType \n" );

  ROSE_ASSERT( != NULL);

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayLibType \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayClassType( SgExpression *pSgExpression )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayClassType \n" );

  ROSE_ASSERT( != NULL);

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayClassType \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArraySectionType( SgExpression *pSgExpression )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArraySectionType \n" );

  ROSE_ASSERT( != NULL);

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArraySectionType \n" );

  return bretValue;
}

#endif

Boolean ArrayClassSageInterface::isArrayLibType( SgType *pSgType )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayLibType \n" );

  ROSE_ASSERT( pSgType!= NULL);
  
  if (! (bretValue = isArrayClassType(pSgType) ) )
    bretValue = isArraySectionType(pSgType);

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayLibType \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArrayClassType( SgType *pSgType )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArrayClassType \n" );

  ROSE_ASSERT( pSgType!= NULL);
  
  SgType *pType = getType(pSgType);
  ROSE_ASSERT(pType != NULL);
//printf ("pType->sage_class_name() = %s \n",pType->sage_class_name());

  SgClassType* pClassType = isSgClassType( pType );

  if( pClassType != NULL )
    {
      SgName sageName = pClassType->get_name();
   // printf ("In ArrayClassSageInterface::isArrayClassType(SgType*): Class name = %s \n",sageName.str());
      
      bretValue = isArrayClassType(sageName.str());
    }
  else
    {
   // printf ("In ArrayClassSageInterface::isArrayClassType(SgType*): NOT a SgClassType \n");
    }

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArrayClassType \n" );

  return bretValue;
}

Boolean ArrayClassSageInterface::isArraySectionType( SgType *pSgType )
{                                           
  Boolean bretValue = FALSE;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::isArraySectionType \n" );

  ROSE_ASSERT( pSgType!= NULL);

  SgType *pType = getType(pSgType);

  SgClassType* pClassType = isSgClassType( pType );

  if ( pClassType != NULL )
    {
      SgName sageName = pClassType->get_name();
      bretValue = isArraySectionType(sageName.str());
    }

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::isArraySectionType \n" );

  return bretValue;
}

SgType * ArrayClassSageInterface::getType(SgType *pType)
{
  SgType *preturnType = NULL;

  ROSE_ASSERT ( pType != NULL );
//printf ("In ArrayClassSageInterface::getType(SgType*): pType->sage_class_name() = %s \n",pType->sage_class_name());

  switch ( pType->variant() )
    {
    case T_POINTER:
      {
	SgPointerType *typePointer = isSgPointerType (pType);
	ROSE_ASSERT (typePointer != NULL);
	preturnType = getType( typePointer->get_base_type() );       // recursive call
	break;
      }
    case T_MEMBER_POINTER:	
      {
	SgPointerMemberType *pointerMemberType = isSgPointerMemberType (pType);
	ROSE_ASSERT (pointerMemberType != NULL);
	preturnType = getType( pointerMemberType->get_base_type() );
	break;
      }
    case T_REFERENCE:	
      {
	SgReferenceType *referenceType = isSgReferenceType(pType);
	ROSE_ASSERT (referenceType != NULL);
	preturnType = getType( referenceType->get_base_type() );
	break;
      }
    case T_CLASS:
      {
	SgClassType* pClassType = isSgClassType( pType );
	ROSE_ASSERT (pClassType != NULL);
	preturnType = pClassType;
	break;
      }
    case T_MODIFIER:	
      {
	SgModifierType *ptypeModifier = isSgModifierType (pType);
	ROSE_ASSERT (ptypeModifier != NULL);
	preturnType = ptypeModifier->get_base_type();
	break;
      }
    case T_FUNCTION:	
      {
	SgFunctionType* functionType = isSgFunctionType( pType );
	ROSE_ASSERT (functionType != NULL);
	preturnType = getType( functionType->get_return_type() );
	break;
      }
    case T_MEMBERFUNCTION:
      {
	SgMemberFunctionType* memberFunctionType = isSgMemberFunctionType( pType );
	ROSE_ASSERT (memberFunctionType != NULL);
	preturnType = getType( memberFunctionType->get_return_type() );
	break;
      }
    case T_PARTIAL_FUNCTION:
      {
	SgPartialFunctionType* partialFunctionType = isSgPartialFunctionType( pType );
	ROSE_ASSERT ( partialFunctionType != NULL);
	preturnType = getType( partialFunctionType->get_return_type() );
	break;
      }
    case T_ARRAY:	
      {
	SgArrayType *parrayType = isSgArrayType( pType );
	ROSE_ASSERT ( parrayType != NULL );
	preturnType = getType( parrayType->get_base_type() );
	break;
      }
    default:
      {
       preturnType = pType;    // return what you got
       break;
     }
    }

//printf ("In ArrayClassSageInterface::getType(SgType*): preturnType->sage_class_name() = %s \n",preturnType->sage_class_name());
  return preturnType;
}

SgType * ArrayClassSageInterface::getType(SgExpression *pSgExpression)
{
  ROSE_ASSERT ( pSgExpression != NULL );
  return ( getType( pSgExpression->get_type() ) );
}


//-------------------------------------------------------------------------------------------------
// Function   : isROSENode
// Arguments  : SgNode *pNode
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSENode ( SgNode *pNode )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pNode != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSENode ( SgNode *pNode )\n");
     }
   // looks like we have trouble here as there is no isSgStatement & isSgExpression expects
   // a LocatedNode arg. so will likely remove this function at some point as it's really
   // of not much functionality
#if 0
SgExpression *pExpression = isSgExpression(pNode);

if( pExpression ) 
  {
    bretValue = isROSEIndexArgumentList ( pExpression ) || \
                isROSEType ( pExpression )              || \
                isROSEExpression ( pExpression );
  }
else 
  {
    SgStatement *pStatement = isSgStatement( pNode );
    if( pStatement )
              bretValue = isROSEStatement ( pStatement ); 
  }
#endif              
 if ( bDebug == verbose_debug_level )
   {
     printf("Exiting ArrayClassSageInterface::isROSENode ( SgNode *pNode )\n");
   }
 
 return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEIndexArgumentList
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEIndexArgumentList ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEIndexArgumentList ( SgExpression *pExpression )\n");
     }

   // will only consider the case of a SgExprListExp though ConstructorInitializer might come in
   if ( pExpression->variant() == EXPR_LIST )
     {
       SgExprListExp* exprListExp = isSgExprListExp( pExpression );
       ROSE_ASSERT (exprListExp != NULL);

      SgExpressionPtrList & expressionPtrList = exprListExp->get_expressions();
      if( expressionPtrList.size() > 0 )
	{
	  SgExpressionPtrList::iterator i = expressionPtrList.begin();
	   while (i != expressionPtrList.end()) 
              {
#if USE_SAGE3
             // Sage3 uses STL and this simplifies the syntax for using lists
		SgExpression *pExpr = *i;
#else
	        SgExpressionPtr expressionPtr = *i;
		SgExpression *pExpr = expressionPtr.operator->();
#endif
             // will have to strengthen this later, currently leave it as it is
		if( isROSEIndexType( pExpr) || isROSERangeType( pExpr ) || isROSEInternal_IndexType( pExpr))
		  bretValue = TRUE;
	        i++;
	      }
         }      
     }

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEIndexArgumentList (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEIndexExpression
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEIndexExpression ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEIndexExpression ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEIndexExpressionBinaryOperatorCExpression ( pExpression ) || \
               isROSECExpressionBinaryOperatorIndexExpression ( pExpression ) || \
               isROSEIndexOperand ( pExpression );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEIndexExpression (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEIndexExpressionBinaryOperatorCExpression
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEIndexExpressionBinaryOperatorCExpression ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEIndexExpressionBinaryOperatorCExpression ( SgExpression *pExpression )\n");
     }

   // we'll leave the code at this state until we know whether it is necessary to differentiate 
   // between these two cases at all - I+1 & 1+I ; which is what we have two functions for
   bretValue = isArraySectionOperator( pExpression );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEIndexExpressionBinaryOperatorCExpression (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSECExpressionBinaryOperatorIndexExpression
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSECExpressionBinaryOperatorIndexExpression ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSECExpressionBinaryOperatorIndexExpression ( SgExpression *pExpression )\n");
     }

   bretValue = isArraySectionOperator( pExpression );   

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSECExpressionBinaryOperatorIndexExpression (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     
#if 0
//-------------------------------------------------------------------------------------------------
// Function   : isROSEUnaryOperatorIndexOperand
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEUnaryOperatorIndexOperand ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEUnaryOperatorIndexOperand ( SgExpression *pExpression )\n");
     }

   

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEUnaryOperatorIndexOperand (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     
#endif

//-------------------------------------------------------------------------------------------------
// Function   : isROSEIndexOperand
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEIndexOperand ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEIndexOperand ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEIndexingVariable ( pExpression ) || \
               isROSEIndexingVariableReference ( pExpression ) || \
               isROSEIndexingVariablePointer ( pExpression );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEIndexOperand (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEIndexingVariable
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEIndexingVariable ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEIndexingVariable ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEIndexVariable ( pExpression ) || \
              isROSERangeVariable ( pExpression );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEIndexingVariable (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEIndexVariable
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEIndexVariable ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEIndexVariable ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEIndexingVariableExpression( pExpression, "Index" ) || \
               isROSEIndexingVariableExpression( pExpression, "Internal_Index");   

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEIndexVariable (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSERangeVariable
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSERangeVariable ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSERangeVariable ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEIndexingVariableExpression( pExpression, "Range" ) || \
               isROSEIndexingVariableExpression( pExpression, "Internal_Index");

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSERangeVariable (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     
//-------------------------------------------------------------------------------------------------
// Function   : isROSEIndexingVariableExpression
// Arguments  : SgExpression *pExpression, const char *sName
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEIndexingVariableExpression ( SgExpression *pExpression, const char *sName )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface:isROSEIndexingVariableExpression ( SgExpression *pExpression, const char *sName ):\n");
     }

   switch ( pExpression->variant())
     {
     case VAR_REF: 
       {
	 SgVarRefExp *pExpr = isSgVarRefExp(pExpression);
	 if ( pExpr != NULL ) 
	   {
	     SgVariableSymbol *pSymbol = pExpr->get_symbol();
	     SgClassType *pClassType = isSgClassType( pSymbol->get_type() );
		 if( (pClassType != NULL ) && ( !strcmp( sName, (pClassType->get_name()).str() ) ) ) 
		   bretValue = TRUE;
	   }
	 break;
       }
     default:
	{
	  break;
	}   
     }

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEIndexingVariableExpression ( SgExpression *pExpression, const char *sName )\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEIndexingVariableReference
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEIndexingVariableReference ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEIndexingVariableReference ( SgExpression *pExpression )\n");
     }

     bretValue = isROSEIndexVariableReference ( pExpression ) || \
                 isROSERangeVariableReference ( pExpression );
 
   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEIndexingVariableReference (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEIndexVariableReference
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEIndexVariableReference ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEIndexVariableReference ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEIndexingVariableReferenceExpression( pExpression, "Index" ) || \
               isROSEIndexingVariableReferenceExpression( pExpression, "Internal_Index");    

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEIndexVariableReference (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSERangeVariableReference
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSERangeVariableReference ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSERangeVariableReference ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEIndexingVariableReferenceExpression( pExpression, "Range" ) || \
               isROSEIndexingVariableReferenceExpression( pExpression, "Internal_Index");   


   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSERangeVariableReference (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     
//-------------------------------------------------------------------------------------------------
// Function   : isROSEIndexingVariableReferenceExpression
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEIndexingVariableReferenceExpression ( SgExpression *pExpression, const char *sName )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface:isROSEIndexingVariableReferenceExpression ( SgExpression *pExpression, const char *sName ):\n");
     }

   switch ( pExpression->variant())
     {
     case VAR_REF: 
       {
	 SgVarRefExp *pExpr = isSgVarRefExp(pExpression);
	 if ( pExpr != NULL ) 
	   {
	     // now make sure it is a reference and then ensure it's an array reference
	     SgVariableSymbol *pSymbol = pExpr->get_symbol();
	     SgReferenceType *pRefType = isSgReferenceType( pSymbol->get_type() );
	     if( pRefType != NULL )
	       {
		 SgClassType *pClassType = isSgClassType( pRefType->get_base_type());
		 if( (pClassType != NULL ) && ( !strcmp( sName, (pClassType->get_name()).str() ) ) ) 
		   bretValue = TRUE;
	       }
	   }
	 break;
       }
     default:
       break;
     }   

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEIndexingVariableReferenceExpression ( SgExpression *pExpression, const char *sName )\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEIndexingVariablePointer
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEIndexingVariablePointer ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEIndexingVariablePointer ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEIndexVariablePointer ( pExpression ) || \
               isROSERangeVariablePointer ( pExpression );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEIndexingVariablePointer (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEIndexVariablePointer
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEIndexVariablePointer ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEIndexVariablePointer ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEIndexingVariablePointerExpression( pExpression, "Index" ) || \
               isROSEIndexingVariablePointerExpression( pExpression, "Internal_Index");   
 
   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEIndexVariablePointer (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSERangeVariablePointer
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSERangeVariablePointer ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSERangeVariablePointer ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEIndexingVariablePointerExpression( pExpression, "Range" ) || \
               isROSEIndexingVariablePointerExpression( pExpression, "Internal_Index");      

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSERangeVariablePointer (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEIndexingVariablePointerExpression
// Arguments  : SgExpression *pExpression, const char *sName
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEIndexingVariablePointerExpression ( SgExpression *pExpression, const char *sName )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface:isROSEIndexingVariablePointerExpression ( SgExpression *pExpression, const char *sName ):\n");
     }

   switch ( pExpression->variant())
     {
     case VAR_REF: 
       {
	 SgVarRefExp *pExpr = isSgVarRefExp(pExpression);
	 if ( pExpr != NULL ) 
	   {
	     // now make sure it is a reference and then ensure it's an array reference
	     SgVariableSymbol *pSymbol = pExpr->get_symbol();
	     // will probably have to tighten this to include member pointers too
	     SgPointerType *pType = isSgPointerType( pSymbol->get_type() );
	     if( pType != NULL )
	       {
		 SgClassType *pClassType = isSgClassType( pType->get_base_type());
		 if( (pClassType != NULL ) && ( !strcmp( sName, (pClassType->get_name()).str() ) ) ) 
		   bretValue = TRUE;
	       }
	   }
	 break;
       }
     default:
       break;
     }   

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEIndexingVariablePointerExpression ( SgExpression *pExpression, const char *sName )\n");
     }

   return bretValue;
}
          
//-------------------------------------------------------------------------------------------------
// Function   : isROSEIndexingVariableDereferencedPointer
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEIndexingVariableDereferencedPointer ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEIndexingVariableDereferencedPointer ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEIndexVariableDereferencedPointer( pExpression ) || \
               isROSERangeVariableDereferencedPointer( pExpression );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEIndexingVariableDereferencedPointer ( SgExpression *pExpression )\n");
     }

   return bretValue;
}
     
          
//-------------------------------------------------------------------------------------------------
// Function   : isROSEIndexVariableDereferencedPointer
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEIndexVariableDereferencedPointer ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEIndexVariableDereferencedPointer ( SgExpression *pExpression )\n");
     }

   // may need to add more cases as we come across wierd stuff
   switch ( pExpression->variant())
     {
     case FUNC_CALL:
       {
	 bretValue = isROSEIndexVariableDereferencedPointer( ((SgFunctionCallExp *)pExpression )->get_function() );
	 break;
       }
     case POINTST_OP:
       {
	 SgArrowExp *pExp = isSgArrowExp( pExpression );
	 if( pExp != NULL )
	   bretValue = isROSEIndexVariablePointer( pExp->get_lhs_operand() );
	 break;
       }
     default:
       break;
     }

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEIndexVariableDereferencedPointer ( SgExpression *pExpression )\n");
     }

   return bretValue;
}
          
//-------------------------------------------------------------------------------------------------
// Function   : isROSERangeVariableDereferencedPointer
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface:: isROSERangeVariableDereferencedPointer( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface:: isROSERangeVariableDereferencedPointer( SgExpression *pExpression )\n");
     }

   // may need to add more cases as we come across wierd stuff
   switch ( pExpression->variant())
     {
     case FUNC_CALL:
       {
	 bretValue = isROSERangeVariableDereferencedPointer( ((SgFunctionCallExp *)pExpression )->get_function() );
	 break;
       }
     case POINTST_OP:
       {
	 SgArrowExp *pExp = isSgArrowExp( pExpression );
	 if( pExp != NULL )
	   bretValue = isROSERangeVariablePointer( pExp->get_lhs_operand() );
	 break;
       }
     default:
       break;
     }

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSERangeVariableDereferencedPointer ( SgExpression *pExpression )\n");
     }

   return bretValue;
}
     
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEType
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEType ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEType ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEArrayType ( pExpression ) || \
               isROSEIndexingType ( pExpression );
 
   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEType (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayType
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayType ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayType ( SgExpression *pExpression )\n");
     }

   bretValue = isROSERealArrayType ( pExpression ) || \
               isROSEintArrayType ( pExpression );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayType (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSERealArrayType
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSERealArrayType ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSERealArrayType ( SgExpression *pExpression )\n");
     }

    bretValue = isROSEdoubleArrayType ( pExpression ) || \
                isROSEfloatArrayType ( pExpression );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSERealArrayType (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEdoubleArrayType
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEdoubleArrayType ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEdoubleArrayType ( SgExpression *pExpression )\n");
     }

   SgType *pType = getType(pExpression);
   SgClassType *pClassType = isSgClassType( pType );
   if ( (pClassType != NULL ) && !strcmp ( ( pClassType->get_name() ).str(), "doubleArray" ) )
     bretValue = TRUE;

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEdoubleArrayType (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEfloatArrayType
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEfloatArrayType ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEfloatArrayType ( SgExpression *pExpression )\n");
     }
  
   SgType *pType = getType(pExpression);
   SgClassType *pClassType = isSgClassType( pType );
   if ( ( pClassType != NULL ) && !strcmp ( ( pClassType->get_name() ).str(), "floatArray" ) )
     bretValue = TRUE; 

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEfloatArrayType (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEintArrayType
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEintArrayType ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEintArrayType ( SgExpression *pExpression )\n");
     }

   SgType *pType = getType(pExpression);
   SgClassType *pClassType = isSgClassType( pType );
   if ( ( pClassType != NULL ) && !strcmp ( ( pClassType->get_name() ).str(), "intArray" ) )
     bretValue = TRUE;

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEintArrayType (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEIndexingType
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEIndexingType ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEIndexingType ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEIndexType ( pExpression ) || \
               isROSERangeType ( pExpression );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEIndexingType (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEIndexType
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEIndexType ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEIndexType ( SgExpression *pExpression )\n");
     }

   SgType *pType = getType(pExpression);
   SgClassType *pClassType = isSgClassType( pType );
   if ( ( pClassType != NULL ) && !strcmp ( ( pClassType->get_name() ).str(), "Index" ) )
     bretValue = TRUE;   

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEIndexType (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSERangeType
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSERangeType ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSERangeType ( SgExpression *pExpression )\n");
     }

   SgType *pType = getType(pExpression);
   SgClassType *pClassType = isSgClassType( pType );
   if ( ( pClassType != NULL ) && !strcmp ( ( pClassType->get_name() ).str(), "Range" ) )
     bretValue = TRUE;

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSERangeType (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEInternal_IndexType
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEInternal_IndexType ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface:: isROSEInternal_IndexType( SgExpression *pExpression )\n");
     }

   SgType *pType = getType(pExpression);
   SgClassType *pClassType = isSgClassType( pType );
   if ( ( pClassType != NULL ) && \
	( !strcmp ( ( pClassType->get_name() ).str(), "Internal_Index" ) \
	  || !strcmp ( ( pClassType->get_name() ).str(), "Index" ) \
	  || !strcmp ( ( pClassType->get_name() ).str(), "Range" ) ) )
     bretValue = TRUE;


   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface:: isROSEInternal_IndexType(  SgExpression *pExpression)\n");
     }

   return bretValue;
}

//-------------------------------------------------------------------------------------------------
// Function   : isROSEType
// Arguments  : SgType *pType
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEType ( SgType *pType )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pType != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEType ( SgType *pType )\n");
     }

   bretValue = isROSEArrayType ( pType ) || \
               isROSEIndexingType ( pType );
 
   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEType (  SgType *pType)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayType
// Arguments  : SgType *pType
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayType ( SgType *pType )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pType != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayType ( SgType *pType )\n");
     }

   bretValue = isROSERealArrayType ( pType ) || \
               isROSEintArrayType ( pType );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayType (  SgType *pType)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSERealArrayType
// Arguments  : SgType *pType
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSERealArrayType ( SgType *pType )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pType != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSERealArrayType ( SgType *pType )\n");
     }

    bretValue = isROSEdoubleArrayType ( pType ) || \
                isROSEfloatArrayType ( pType );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSERealArrayType (  SgType *pType)\n");
     }

   return bretValue;
}
     
//-------------------------------------------------------------------------------------------------
// Function   : isROSEdoubleArrayType
// Arguments  : SgType *pType
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEdoubleArrayType ( SgType *pType )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pType != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEdoubleArrayType ( SgType *pType )\n");
     }

   SgType *pBaseType = getType(pType);
   SgClassType *pClassType = isSgClassType( pBaseType );
   if ( (pClassType != NULL ) && !strcmp ( ( pClassType->get_name() ).str(), "doubleArray" ) )
     bretValue = TRUE;

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEdoubleArrayType (  SgType *pType)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEfloatArrayType
// Arguments  : SgType *pType
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEfloatArrayType ( SgType *pType )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pType != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEfloatArrayType ( SgType *pType )\n");
     }
  
   SgType *pBaseType = getType(pType);
   SgClassType *pClassType = isSgClassType( pBaseType );
   if ( ( pClassType != NULL ) && !strcmp ( ( pClassType->get_name() ).str(), "floatArray" ) )
     bretValue = TRUE; 

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEfloatArrayType (  SgType *pType)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEintArrayType
// Arguments  : SgType *pType
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEintArrayType ( SgType *pType )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pType != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEintArrayType ( SgType *pType )\n");
     }

   SgType *pBaseType = getType(pType);
   SgClassType *pClassType = isSgClassType( pBaseType );
   if ( ( pClassType != NULL ) && !strcmp ( ( pClassType->get_name() ).str(), "intArray" ) )
     bretValue = TRUE;

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEintArrayType (  SgType *pType)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEIndexingType
// Arguments  : SgType *pType
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEIndexingType ( SgType *pType )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pType != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEIndexingType ( SgType *pType )\n");
     }

   bretValue = isROSEIndexType ( pType ) || \
               isROSERangeType ( pType );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEIndexingType (  SgType *pType)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEIndexType
// Arguments  : SgType *pType
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEIndexType ( SgType *pType )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pType != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEIndexType ( SgType *pType )\n");
     }

   SgType *pBaseType = getType(pType);
   SgClassType *pClassType = isSgClassType( pBaseType );
   if ( ( pClassType != NULL ) && !strcmp ( ( pClassType->get_name() ).str(), "Index" ) )
     bretValue = TRUE;   

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEIndexType (  SgType *pType)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSERangeType
// Arguments  : SgType *pType
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSERangeType ( SgType *pType )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pType != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSERangeType ( SgType *pType )\n");
     }

   SgType *pBaseType = getType(pType);
   SgClassType *pClassType = isSgClassType( pBaseType );
   if ( ( pClassType != NULL ) && !strcmp ( ( pClassType->get_name() ).str(), "Range" ) )
     bretValue = TRUE;

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSERangeType (  SgType *pType)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEInternal_IndexType
// Arguments  : SgType *pType
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEInternal_IndexType ( SgType *pType )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pType != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface:: isROSEInternal_IndexType( SgType *pType )\n");
     }

   SgType *pBaseType = getType(pType);
   SgClassType *pClassType = isSgClassType( pBaseType );
   if ( ( pClassType != NULL ) && \
	( !strcmp ( ( pClassType->get_name() ).str(), "Internal_Index" ) \
	  || !strcmp ( ( pClassType->get_name() ).str(), "Index" ) \
	  || !strcmp ( ( pClassType->get_name() ).str(), "Range" ) ) )
     bretValue = TRUE;


   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface:: isROSEInternal_IndexType(  SgType *pType)\n");
     }

   return bretValue;
}


#if 0
//-------------------------------------------------------------------------------------------------
// Function   : isROSEArgumentList
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArgumentList ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArgumentList ( SgExpression *pExpression )\n");
     }

   

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArgumentList (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
#endif     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEStatement
// Arguments  : SgStatement *pStatement
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEStatement ( SgStatement *pStatement )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pStatement != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEStatement ( SgStatement *pStatement )\n");
     }

   bretValue = isROSEVariableDeclaration ( pStatement ) || \
               isROSEStatementBlock ( pStatement )      || \
               isROSEC_Statement ( pStatement )         || \
               isROSEArrayStatement ( pStatement );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEStatement (  SgStatement *pStatement)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEVariableDeclaration
// Arguments  : SgStatement *pStatement
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEVariableDeclaration ( SgStatement *pStatement  )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pStatement != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEVariableDeclaration ( SgStatement *pStatement )\n");
     }

   if( pStatement->variant() == VAR_DECL_STMT )
     {
       bretValue = isROSEC_VariableDeclaration ( pStatement ) || \
                   isROSEArrayVariableDeclaration ( pStatement ) || \
                   isROSEIndexingVariableDeclaration ( pStatement );
     }

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEVariableDeclaration (  SgStatement *pStatement)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEC_VariableDeclaration
// Arguments  : SgStatement *pStatement
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEC_VariableDeclaration ( SgStatement *pStatement )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pStatement != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEC_VariableDeclaration ( SgStatement *pStatement )\n");
     }

   bretValue = ( pStatement->variant() == VAR_DECL_STMT) && !( isROSEArrayVariableDeclaration ( pStatement ) || \
               isROSEIndexingVariableDeclaration ( pStatement ) );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEC_VariableDeclaration ( SgStatement *pStatement )\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayVariableDeclaration
// Arguments  : SgStatement *pStatement
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayVariableDeclaration ( SgStatement *pStatement )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pStatement != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayVariableDeclaration ( SgStatement *pStatement )\n");
     }

   if( pStatement->variant() == VAR_DECL_STMT )
     {
       SgVariableDeclaration *variableDeclaration = isSgVariableDeclaration( pStatement ); 
       SgInitializedNameList & variableList = variableDeclaration->get_variables();
       for (SgInitializedNameList::iterator i = variableList.begin(); i != variableList.end(); i++)
	 bretValue = isArrayClassType( (*i).get_type() ); // never mind the repeated init.!
     }

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayVariableDeclaration ( SgStatement *pStatement )\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEIndexingVariableDeclaration
// Arguments  : SgStatement *pStatement
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface:: isROSEIndexingVariableDeclaration( SgStatement *pStatement )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pStatement != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEIndexingVariableDeclaration ( SgStatement *pStatement )\n");
     }

   if( pStatement->variant() == VAR_DECL_STMT )
     {
       SgVariableDeclaration *variableDeclaration = isSgVariableDeclaration( pStatement ); 
       SgInitializedNameList & variableList = variableDeclaration->get_variables();
       for (SgInitializedNameList::iterator i = variableList.begin(); i != variableList.end(); i++)
	 bretValue = isArraySectionType( (*i).get_type() ); // never mind the repeated init.!
     }   

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEIndexingVariableDeclaration ( SgStatement *pStatement )\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEStatementBlock
// Arguments  : SgStatement *pStatement
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEStatementBlock ( SgStatement *pStatement )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pStatement != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEStatementBlock ( SgStatement *pStatement )\n");
     }

   bretValue = ( pStatement->variant() == BASIC_BLOCK_STMT )?TRUE:FALSE;

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEStatementBlock (  SgStatement *pStatement)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEC_Statement
// Arguments  : SgStatement *pStatement
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEC_Statement ( SgStatement *pStatement )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pStatement != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEC_Statement ( SgStatement *pStatement )\n");
     }

   bretValue = ! ( isROSEVariableDeclaration( pStatement ) || isROSEStatementBlock( pStatement ) || \
               isROSEArrayStatement( pStatement ) );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEC_Statement (  SgStatement *pStatement)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayStatement
// Arguments  : SgStatement *pStatement
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayStatement ( SgStatement *pStatement )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pStatement != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayStatement ( SgStatement *pStatement )\n");
     }

   bretValue = isROSEExpressionStatement ( pStatement ) || \
               isROSEWhereStatement ( pStatement )      || \
               isROSEElseWhereStatement ( pStatement )  || \
               isROSEDoWhileStatement ( pStatement )         || \
               isROSEWhileStatement ( pStatement )      || \
               isROSEForStatement ( pStatement )        || \
               isROSEIfStatement ( pStatement )         || \
               isROSEReturnStatement ( pStatement );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayStatement (  SgStatement *pStatement)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEExpressionStatement
// Arguments  : SgStatement *pStatement
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEExpressionStatement ( SgStatement *pStatement )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pStatement != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEExpressionStatement ( SgStatement *pStatement )\n");
     }

   if ( pStatement->variant() == EXPR_STMT )
     {
       SgExprStatement *pExprStmt = isSgExprStatement( pStatement );
       SgExpression *pExpression = pExprStmt->get_the_expr_i();
       SgFunctionCallExp *pFunctionCallExp = isSgFunctionCallExp( pExpression );
       if ( ( pFunctionCallExp != NULL ) && ( isROSEArrayExpression( pFunctionCallExp ) ) )
	 bretValue = TRUE;
     }

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEExpressionStatement (  SgStatement *pStatement)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEWhereStatement
// Arguments  : SgStatement *pStatement
// Comments   : currently the test for the where statement involves only a test on the initialisation
//              part of the for statement. If we find it necessary to add code to check the other 
//              parts of the for statement then we will
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEWhereStatement ( SgStatement *pStatement )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pStatement != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEWhereStatement ( SgStatement *pStatement )\n");
     }

   bretValue = isROSEWhereStatement( pStatement, "where_function");

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEWhereStatement (  SgStatement *pStatement)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEElseWhereStatement
// Arguments  : SgStatement *pStatement
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEElseWhereStatement ( SgStatement *pStatement )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pStatement != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEElseWhereStatement ( SgStatement *pStatement )\n");
     }

   bretValue = isROSEWhereStatement( pStatement, "elsewhere_function");

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEElseWhereStatement (  SgStatement *pStatement)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEWhereStatement
// Arguments  : SgStatement *pStatement
// Comments   : internal routine, NOT to be used outside
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEWhereStatement ( SgStatement *pStatement, const char *specificWhere )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pStatement != NULL );
   SgStatementPtrList::iterator statementIterator;

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface:: isROSEWhereStatement( SgStatement *pStatement, const char *specificWhere )\n");
     }

    if( pStatement->variant() == FOR_STMT )
     {
       SgForStatement *forStatement = isSgForStatement( pStatement );
       if( TRUE ) // ( forStatement->get_init_stmt() != NULL )
	 {
	   statementIterator = forStatement->get_init_stmt().begin(); // get the first component of init statement
#if USE_SAGE3
           SgStatement *pStmt = (*statementIterator);                 // get the statement
#else
           SgStatement *pStmt = (*statementIterator).irep();          // get the statement
#endif
	   if ( ( pStmt != NULL ) && ( pStmt->variant() == EXPR_STMT ) )
	     {
	       SgExprStatement *pExprStmt = isSgExprStatement( pStmt );
	       SgAssignOp *pAssignOp = isSgAssignOp ( pExprStmt->get_the_expr() ); // it has to be an assign op
	       if ( pAssignOp != NULL )
		 {
		   SgVarRefExp *pVarRefExp  = isSgVarRefExp( pAssignOp->get_lhs_operand() );
		   SgFunctionCallExp *pFunctionCallExp  = isSgFunctionCallExp( pAssignOp->get_rhs_operand() );
		   if( ( pVarRefExp != NULL ) && ( pFunctionCallExp != NULL ) )
		     {
		       SgVariableSymbol *pVarSymbol = pVarRefExp->get_symbol();
		       SgFunctionRefExp *pFnRefExp = isSgFunctionRefExp( pFunctionCallExp->get_function() );
		       if ( ( pVarSymbol != NULL ) && ( pFnRefExp != NULL ) )
			 {
			   SgFunctionSymbol *pFnSymbol = pFnRefExp->get_symbol();
			   if( ( pFnSymbol != NULL ) && (!strcmp( (pFnSymbol->get_name()).str(), specificWhere ) ) \
			       && (!strcmp( (pVarSymbol->get_name()).str(), "APP_Global_Where_Var") ) )
			     bretValue = TRUE;
			 }
		     }
		 }
	     }

	}
     }
  

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEWhereStatement (  SgStatement *pStatement, const char *specificWhere)\n");
     }

   return bretValue;
}


//-------------------------------------------------------------------------------------------------
// Function   : isROSEDoWhileStatement
// Arguments  : SgStatement *pStatement
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEDoWhileStatement ( SgStatement *pStatement )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pStatement != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEDoWhileStatement ( SgStatement *pStatement )\n");
     }

   bretValue = ( pStatement->variant() == DO_WHILE_STMT )?TRUE:FALSE;   

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEDoWhileStatement (  SgStatement *pStatement)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEWhileStatement
// Arguments  : SgStatement *pStatement
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEWhileStatement ( SgStatement *pStatement )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pStatement != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEWhileStatement ( SgStatement *pStatement )\n");
     }

   bretValue = ( pStatement->variant() == WHILE_STMT )?TRUE:FALSE;   

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEWhileStatement (  SgStatement *pStatement)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEForStatement
// Arguments  : SgStatement *pStatement
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEForStatement ( SgStatement *pStatement )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pStatement != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEForStatement ( SgStatement *pStatement )\n");
     }

   bretValue = ( pStatement->variant() == FOR_STMT ) && !( isROSEWhereStatement(pStatement) || isROSEElseWhereStatement(pStatement) );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEForStatement (  SgStatement *pStatement)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEIfStatement
// Arguments  : SgStatement *pStatement
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEIfStatement ( SgStatement *pStatement )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pStatement != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEIfStatement ( SgStatement *pStatement )\n");
     }
   
   bretValue = ( pStatement->variant() == IF_STMT )?TRUE:FALSE;

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEIfStatement (  SgStatement *pStatement)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEReturnStatement
// Arguments  : SgStatement *pStatement
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEReturnStatement ( SgStatement *pStatement )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pStatement != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEReturnStatement ( SgStatement *pStatement )\n");
     }

   if ( pStatement->variant() == RETURN_STMT )
     {
       SgReturnStmt *pReturnStmt = isSgReturnStmt( pStatement );
       SgExpression *pExpression = pReturnStmt->get_return_expr_i();
       // currently not considering returns of Index objects
       if ( ( pExpression != NULL ) && ( isArrayClassType( pExpression->get_type() ) ) )
	 bretValue = TRUE;
     }

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEReturnStatement (  SgStatement *pStatement)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEExpression
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEExpression ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEExpression ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEArrayExpression ( pExpression ) || \
               isROSEIndexExpression ( pExpression ) || \
               isROSEC_Expression ( pExpression );



   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEExpression (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEC_Expression
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEC_Expression ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEC_Expression ( SgExpression *pExpression )\n");
     }

   bretValue = !isROSEArrayExpression( pExpression );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEC_Expression (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     
#if 0
//-------------------------------------------------------------------------------------------------
// Function   : isROSEUserFunction
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEUserFunction ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEUserFunction ( SgExpression *pExpression )\n");
     }
   

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEUserFunction (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     
#endif

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayExpression
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayExpression ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayExpression ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEArrayOperator ( pExpression )          || \
               isROSEArrayOperandExpression ( pExpression ) || \
               isROSEArrayFunctionExpression ( pExpression );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayExpression (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayOperator
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayOperator ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayOperator ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEUnaryArrayOperator( pExpression ) || \
               isROSEBinaryArrayOperator( pExpression );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayOperator (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEUnaryArrayOperator
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEUnaryArrayOperator ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEUnaryArrayOperator ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEUnaryArrayOperatorMinus(pExpression)              || \
               isROSEUnaryArrayOperatorPlus(pExpression)               || \
               isROSEUnaryArrayOperatorPrefixPlusPlus(pExpression)     || \
               isROSEUnaryArrayOperatorPostfixPlusPlus(pExpression)    || \
               isROSEUnaryArrayOperatorPrefixMinusMinus(pExpression)   || \
               isROSEUnaryArrayOperatorPostfixMinusMinus(pExpression);

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEUnaryArrayOperator (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEUnaryArrayOperatorMinus
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEUnaryArrayOperatorMinus ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEUnaryArrayOperatorMinus ( SgExpression *pExpression )\n");
     }

   SgMemberFunctionRefExp *pExpr = getArrayClassMemberOperator(pExpression);

   if( pExpr != NULL )
     {
       SgFunctionDeclaration* pfunctionDeclarationStatement = getArrayMemberFunctionDeclaration(pExpr);

       if((pfunctionDeclarationStatement != NULL) && (pfunctionDeclarationStatement->isOperator()))
	 {
	   SgName sName = pfunctionDeclarationStatement->get_name();
	   if(!strcmp(sName.str(),"operator-"))
	     {
	       SgInitializedNameList &argList = pfunctionDeclarationStatement->get_args();
	       if ( argList.size() == 0 )
		 bretValue = TRUE;
	     }
	 }
     }

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEUnaryArrayOperatorMinus (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEUnaryArrayOperatorPlus
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEUnaryArrayOperatorPlus ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEUnaryArrayOperatorPlus ( SgExpression *pExpression )\n");
     }

   SgMemberFunctionRefExp *pExpr = getArrayClassMemberOperator(pExpression);

   if ( pExpr != NULL )
     {

       SgFunctionDeclaration* pfunctionDeclarationStatement = getArrayMemberFunctionDeclaration(pExpr);

       if((pfunctionDeclarationStatement != NULL) && (pfunctionDeclarationStatement->isOperator()))
	 {
	   SgName sName = pfunctionDeclarationStatement->get_name();
	   if(!strcmp(sName.str(),"operator+"))
	     {
	       SgInitializedNameList argList = pfunctionDeclarationStatement->get_args();
	       if ( argList.size() == 0 )
		 bretValue = TRUE;
	     }
	 }
     }

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEUnaryArrayOperatorPlus (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEUnaryArrayOperatorPrefixPlusPlus
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEUnaryArrayOperatorPrefixPlusPlus ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEUnaryArrayOperatorPrefixPlusPlus ( SgExpression *pExpression )\n");
     }

   SgMemberFunctionRefExp *pExpr = getArrayClassMemberOperator(pExpression);

   if ( pExpr != NULL )
     {
       SgFunctionDeclaration* pfunctionDeclarationStatement = getArrayMemberFunctionDeclaration(pExpr);

       if((pfunctionDeclarationStatement != NULL) && (pfunctionDeclarationStatement->isOperator()))
	 {
	   SgName sName = pfunctionDeclarationStatement->get_name();
	   if(!strcmp(sName.str(),"operator++"))
	     {
	       SgInitializedNameList &argList = pfunctionDeclarationStatement->get_args();
	       if ( argList.size() == 0 )
		 bretValue = TRUE;
	     }
	 }
     }

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEUnaryArrayOperatorPrefixPlusPlus (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEUnaryArrayOperatorPostfixPlusPlus
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEUnaryArrayOperatorPostfixPlusPlus ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEUnaryArrayOperatorPostfixPlusPlus ( SgExpression *pExpression )\n");
     }

   SgMemberFunctionRefExp *pExpr = getArrayClassMemberOperator(pExpression);

   if ( pExpr != NULL ) 
     {
       SgFunctionDeclaration* pfunctionDeclarationStatement = getArrayMemberFunctionDeclaration(pExpr);
       
       if((pfunctionDeclarationStatement != NULL) && (pfunctionDeclarationStatement->isOperator()))
	 {
	   SgName sName = pfunctionDeclarationStatement->get_name();
	   
	   if(!strcmp(sName.str(),"operator++"))
	     {
	       SgInitializedNameList &argList = pfunctionDeclarationStatement->get_args();
	       if ( argList.size() == 1 )  // not sure whether this suffices
		 bretValue = TRUE;
	     }
	 }
     }


   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEUnaryArrayOperatorPostfixPlusPlus (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEUnaryArrayOperatorPrefixMinusMinus
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEUnaryArrayOperatorPrefixMinusMinus ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEUnaryArrayOperatorPrefixMinusMinus ( SgExpression *pExpression )\n");
     }

   SgMemberFunctionRefExp *pExpr = getArrayClassMemberOperator(pExpression);

   if ( pExpr != NULL )  
     {

       SgFunctionDeclaration* pfunctionDeclarationStatement = getArrayMemberFunctionDeclaration(pExpr);
       
       if((pfunctionDeclarationStatement != NULL) && (pfunctionDeclarationStatement->isOperator()))
	 {
	   SgName sName = pfunctionDeclarationStatement->get_name();
	   if(!strcmp(sName.str(),"operator--"))
	     {
	       SgInitializedNameList &argList = pfunctionDeclarationStatement->get_args();
	       if ( argList.size() == 0 )
		 bretValue = TRUE;
	     }
	 }
     }


   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEUnaryArrayOperatorPrefixMinusMinus (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEUnaryArrayOperatorPostfixMinusMinus
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEUnaryArrayOperatorPostfixMinusMinus ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEUnaryArrayOperatorPostfixMinusMinus ( SgExpression *pExpression )\n");
     }

   SgMemberFunctionRefExp *pExpr = getArrayClassMemberOperator(pExpression);

   if ( pExpr != NULL ) 
     {

       SgFunctionDeclaration* pfunctionDeclarationStatement = getArrayMemberFunctionDeclaration(pExpr);
       
       if((pfunctionDeclarationStatement != NULL) && (pfunctionDeclarationStatement->isOperator()))
	 {
	   SgName sName = pfunctionDeclarationStatement->get_name();
	   
	   if(!strcmp(sName.str(),"operator--"))
	     {
	       SgInitializedNameList &argList = pfunctionDeclarationStatement->get_args();
	       if ( argList.size() == 1 )
		 bretValue = TRUE;
	     }
	 }
     }

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEUnaryArrayOperatorPostfixMinusMinus (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEUnaryArrayLogicalOperator
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEUnaryArrayLogicalOperator ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEUnaryArrayLogicalOperator ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEUnaryArrayOperatorNOT( pExpression );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEUnaryArrayLogicalOperator (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEUnaryArrayOperatorNOT
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEUnaryArrayOperatorNOT ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEUnaryArrayOperatorNOT ( SgExpression *pExpression )\n");
     }

   SgMemberFunctionRefExp *pExpr = getArrayClassMemberOperator(pExpression);

   if ( pExpr != NULL ) 
     {

       SgFunctionDeclaration* pfunctionDeclarationStatement = getArrayMemberFunctionDeclaration(pExpr);
       
       if((pfunctionDeclarationStatement != NULL) && (pfunctionDeclarationStatement->isOperator()))
	 {
	   SgName sName = pfunctionDeclarationStatement->get_name();
	   if(!strcmp(sName.str(),"operator!"))
	     {
	       SgInitializedNameList &argList = pfunctionDeclarationStatement->get_args();
	       if ( argList.size() == 0 )
		 bretValue = TRUE;
	     }
	 }
     }

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEUnaryArrayOperatorNOT (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEBinaryArrayOperator
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEBinaryArrayOperator ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEBinaryArrayOperator ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEBinaryArrayOperatorEquals ( pExpression )         || \
               isROSEBinaryArrayOperatorAdd ( pExpression )            || \
               isROSEBinaryArrayOperatorAddEquals ( pExpression )      || \
               isROSEBinaryArrayOperatorMinus ( pExpression )          || \
               isROSEBinaryArrayOperatorMinusEquals ( pExpression )    || \
               isROSEBinaryArrayOperatorMultiply ( pExpression )       || \
               isROSEBinaryArrayOperatorMultiplyEquals ( pExpression ) || \
               isROSEBinaryArrayOperatorDivide ( pExpression )         || \
               isROSEBinaryArrayOperatorDivideEquals ( pExpression )   || \
               isROSEBinaryArrayOperatorModulo ( pExpression )         || \
               isROSEBinaryArrayOperatorModuloEquals ( pExpression )   || \
               isROSEBinaryArrayTestingOperator ( pExpression )        || \
               isROSEBinaryArrayLogicalOperator ( pExpression );
               
   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEBinaryArrayOperator (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEBinaryArrayOperatorEquals
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEBinaryArrayOperatorEquals ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEBinaryArrayOperatorEquals ( SgExpression *pExpressio )\n");
     }

   bretValue = verifyArrayBinaryMemberOperator( pExpression, "operator=");

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEBinaryArrayOperatorEquals ( SgExpression *pExpression )\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEBinaryArrayOperatorAdd
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEBinaryArrayOperatorAdd ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEBinaryArrayOperatorAdd ( SgExpression *pExpression )\n");
     }

   if ( verifyArrayBinaryFriendOperator( pExpression, "operator+") )
     bretValue = verifyArgumentsForArrayBinaryFriendOperator(pExpression);

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEBinaryArrayOperatorAdd ( SgExpression *pExpression )\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEBinaryArrayOperatorAddEquals 
// Arguments  : SgExpression *pExpression
// Comments   : 
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEBinaryArrayOperatorAddEquals ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEBinaryArrayOperatorAddEquals (  SgExpression *pExpression )\n");
     }

   bretValue = verifyArrayBinaryMemberOperator( pExpression, "operator+=");

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEBinaryArrayOperatorAddEquals (   SgExpression *pExpression )\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEBinaryArrayOperatorMinus
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEBinaryArrayOperatorMinus ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEBinaryArrayOperatorMinus (  SgExpression *pExpression )\n");
     }

   if( verifyArrayBinaryFriendOperator( pExpression, "operator-"))
     bretValue = verifyArgumentsForArrayBinaryFriendOperator(pExpression);

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEBinaryArrayOperatorMinus ( SgExpression *pExpression )\n");
     }

   return bretValue;
}
     

//------------------------------------------------------------------------------------------------
// Function   : isROSEBinaryArrayOperatorMinusEquals
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEBinaryArrayOperatorMinusEquals (  SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEBinaryArrayOperatorMinusEquals (  SgExpression *pExpression )\n");
     }

   bretValue = verifyArrayBinaryMemberOperator( pExpression, "operator-=");

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEBinaryArrayOperatorMinusEquals (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEBinaryArrayOperatorMultiply
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEBinaryArrayOperatorMultiply (  SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEBinaryArrayOperatorMultiply (  SgExpression *pExpression )\n");
     }

   if( verifyArrayBinaryFriendOperator( pExpression, "operator*") )
     bretValue = verifyArgumentsForArrayBinaryFriendOperator(pExpression);

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEBinaryArrayOperatorMultiply ( SgExpression *pExpression )\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEBinaryArrayOperatorMultiplyEquals
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEBinaryArrayOperatorMultiplyEquals ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEBinaryArrayOperatorMultiplyEquals ( SgExpression *pExpression )\n");
     }

   bretValue = verifyArrayBinaryMemberOperator( pExpression, "operator*=");

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEBinaryArrayOperatorMultiplyEquals ( SgExpression *pExpression )\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEBinaryArrayOperatorDivide
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEBinaryArrayOperatorDivide (SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEBinaryArrayOperatorDivide ( SgExpression *pExpression )\n");
     }

   if ( verifyArrayBinaryFriendOperator( pExpression, "operator/") )
	 bretValue = verifyArgumentsForArrayBinaryFriendOperator(pExpression);

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEBinaryArrayOperatorDivide ( SgExpression *pExpression )\n");
     }

   return bretValue;
}
     

//------------------------------------------------------------------------------------------------
// Function   : isROSEBinaryArrayOperatorDivideEquals
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEBinaryArrayOperatorDivideEquals ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEBinaryArrayOperatorDivideEquals ( SgExpression *pExpression )\n");
     }

   bretValue = verifyArrayBinaryMemberOperator( pExpression, "operator/=");
   
   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEBinaryArrayOperatorDivideEquals ( SgExpression *pExpression )\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
//  Function   : isROSEBinaryArrayOperatorModulo
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEBinaryArrayOperatorModulo ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEBinaryArrayOperatorModulo ( SgExpression *pExpression )\n");
     }

   if( verifyArrayBinaryFriendOperator( pExpression, "operator%") )
     bretValue = verifyArgumentsForArrayBinaryFriendOperator(pExpression);

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEBinaryArrayOperatorModulo ( SgExpression *pExpression )\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEBinaryArrayOperatorModuloEquals
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEBinaryArrayOperatorModuloEquals ( SgExpression *pExpression  )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEBinaryArrayOperatorModuloEquals ( SgExpression *pExpression  )\n");
     }
   
   bretValue = verifyArrayBinaryMemberOperator( pExpression, "operator%=");
     
   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEBinaryArrayOperatorModuloEquals ( SgExpression *pExpression  )\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEBinaryArrayTestingOperator
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEBinaryArrayTestingOperator ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEBinaryArrayTestingOperator ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEBinaryArrayOperatorLT( pExpression )          || \
               isROSEBinaryArrayOperatorLTEquals( pExpression )    || \
               isROSEBinaryArrayOperatorGT( pExpression )          || \
               isROSEBinaryArrayOperatorGTEquals( pExpression )    || \
               isROSEBinaryArrayOperatorEquivalence( pExpression ) || \
               isROSEBinaryArrayOperatorNOTEquals( pExpression );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEBinaryArrayTestingOperator (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEBinaryArrayOperatorLT
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEBinaryArrayOperatorLT ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEBinaryArrayOperatorLT ( SgExpression *pExpression )\n");
     }

   if ( verifyArrayBinaryFriendOperator( pExpression, "operator<") )
	 bretValue = verifyArgumentsForArrayBinaryFriendOperator(pExpression);

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEBinaryArrayOperatorLT ( SgExpression *pExpression )\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEBinaryArrayOperatorLTEquals
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEBinaryArrayOperatorLTEquals ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEBinaryArrayOperatorLTEquals ( SgExpression *pExpression )\n");
     }

   if ( verifyArrayBinaryFriendOperator(pExpression, "operator<=") )
     bretValue = verifyArgumentsForArrayBinaryFriendOperator(pExpression);

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEBinaryArrayOperatorLTEquals (SgExpression *pExpression )\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEBinaryArrayOperatorGT
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEBinaryArrayOperatorGT (SgExpression *pExpression  )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEBinaryArrayOperatorGT ( SgExpression *pExpression )\n");
     }

   if ( verifyArrayBinaryFriendOperator(pExpression, "operator>") )
     bretValue = verifyArgumentsForArrayBinaryFriendOperator(pExpression);

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEBinaryArrayOperatorGT ( SgExpression *pExpression )\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEBinaryArrayOperatorGTEquals
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEBinaryArrayOperatorGTEquals (  SgExpression *pExpression  )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEBinaryArrayOperatorGTEquals ( SgExpression *pExpression  )\n");
     }

   if ( verifyArrayBinaryFriendOperator(pExpression, "operator>=") )
     bretValue = verifyArgumentsForArrayBinaryFriendOperator(pExpression);

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEBinaryArrayOperatorGTEquals ( SgExpression *pExpression  )\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEBinaryArrayOperatorEquivalence
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEBinaryArrayOperatorEquivalence ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEBinaryArrayOperatorEquivalence ( SgExpression *pExpression )\n");
     }

   if ( verifyArrayBinaryFriendOperator(pExpression, "operator==") )
     bretValue = verifyArgumentsForArrayBinaryFriendOperator(pExpression);

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEBinaryArrayOperatorEquivalence ( SgExpression *pExpression )\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEBinaryArrayOperatorNOTEquals
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEBinaryArrayOperatorNOTEquals ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEBinaryArrayOperatorNOTEquals (SgExpression *pExpression )\n");
     }

   if ( verifyArrayBinaryFriendOperator(pExpression, "operator!=") )
     bretValue = verifyArgumentsForArrayBinaryFriendOperator(pExpression);

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEBinaryArrayOperatorNOTEquals (SgExpression *pExpression )\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEBinaryArrayLogicalOperator
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEBinaryArrayLogicalOperator ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEBinaryArrayLogicalOperator ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEBinaryArrayOperatorLogicalAND(pExpression) || \
               isROSEBinaryArrayOperatorLogicalOR(pExpression);

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEBinaryArrayLogicalOperator (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
   

//-------------------------------------------------------------------------------------------------
// Function   : isROSEBinaryArrayOperatorLogicalAND
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEBinaryArrayOperatorLogicalAND ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEBinaryArrayOperatorLogicalAND ( SgExpression *pExpression  )\n");
     }

   if ( verifyArrayBinaryFriendOperator(pExpression, "operator&&") )
     bretValue = verifyArgumentsForArrayBinaryFriendOperator(pExpression); 

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEBinaryArrayOperatorLogicalAND ( SgExpression *pExpression )\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEBinaryArrayOperatorLogicalOR
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEBinaryArrayOperatorLogicalOR ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEBinaryArrayOperatorLogicalOR ( SgExpression *pExpression )\n");
     }

   if ( verifyArrayBinaryFriendOperator(pExpression, "operator||") )
     bretValue = verifyArgumentsForArrayBinaryFriendOperator(pExpression);

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEBinaryArrayOperatorLogicalOR ( SgExpression *pExpression )\n");
     }

   return bretValue;
}

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayOperandExpression
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayOperandExpression ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayOperandExpression ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEArrayVariableExpression( pExpression )          || \
               isROSEArrayVariableReferenceExpression( pExpression ) || \
               isROSEArrayVariablePointerExpression( pExpression )   || \
               isROSEArrayParenthesisOperator( pExpression );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayOperandExpression (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayVariableExpression
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayVariableExpression ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayVariableExpression ( SgExpression *pExpression )\n");
     }

   bretValue = isROSERealArrayVariableExpression( pExpression  )  || \
               isROSEintArrayVariableExpression( pExpression  );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayVariableExpression (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSERealArrayVariableExpression
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSERealArrayVariableExpression ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSERealArrayVariableExpression ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEdoubleArrayVariableExpression( pExpression ) || \
               isROSEfloatArrayVariableExpression( pExpression );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSERealArrayVariableExpression (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEdoubleArrayVariableExpression
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEdoubleArrayVariableExpression ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEdoubleArrayVariableExpression ( SgExpression *pExpression )\n");
     }


   switch ( pExpression->variant())
     {
     case VAR_REF: 
       {
	 SgVarRefExp *pExpr = isSgVarRefExp(pExpression);
	 if( pExpr != NULL ) 
	   {
	     SgType *pType = pExpr->get_type();
	     SgClassType *pClassType = isSgClassType( pType ); // this should be sufficient
	     if( (pClassType != NULL ) && ( !strcmp( "doubleArray",(pClassType->get_name()).str() ) ) ) 
	       bretValue = TRUE;
	   }
	 break;
       }
     default:
       break;
     }

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEdoubleArrayVariableExpression (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEfloatArrayVariableExpression
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEfloatArrayVariableExpression ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEfloatArrayVariableExpression ( SgExpression *pExpression )\n");
     }

   switch ( pExpression->variant())
     {
     case VAR_REF:
       {
	 SgVarRefExp *pExpr = isSgVarRefExp(pExpression);
	 if( pExpr != NULL )
	   {
	     SgType *pType = pExpr->get_type();
	     SgClassType *pClassType = isSgClassType( pType );
	     if( (pClassType != NULL ) && ( !strcmp( "floatArray",(pClassType->get_name()).str() ) ) )
	       bretValue = TRUE;
	       
	   }
	 break;
       }
     default:
       break;
     }

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEfloatArrayVariableExpression (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEintArrayVariableExpression
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEintArrayVariableExpression ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEintArrayVariableExpression ( SgExpression *pExpression )\n");
     }

   switch ( pExpression->variant()) 
     {
     case VAR_REF:
       {
	 SgVarRefExp *pExpr = isSgVarRefExp(pExpression);
	 if( pExpr != NULL )
	   {
	     SgType *pType = pExpr->get_type();
	     SgClassType *pClassType = isSgClassType( pType );
	     if( (pClassType != NULL ) && ( !strcmp( "intArray",(pClassType->get_name()).str() ) ) )
	       bretValue = TRUE;
	       
	   }
	 break; 
       }
     default: 
       break; 
     }

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEintArrayVariableExpression (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayVariableReferenceExpression
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayVariableReferenceExpression ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayVariableReferenceExpression ( SgExpression *pExpression )\n");
     }

      bretValue = isROSERealArrayVariableReferenceExpression( pExpression ) || \
                  isROSEintArrayVariableReferenceExpression( pExpression );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayVariableReferenceExpression (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSERealArrayVariableReferenceExpression
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSERealArrayVariableReferenceExpression ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSERealArrayVariableReferenceExpression ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEdoubleArrayVariableReferenceExpression( pExpression ) || \
               isROSEfloatArrayVariableReferenceExpression( pExpression );


   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSERealArrayVariableReferenceExpression (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEdoubleArrayVariableReferenceExpression
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEdoubleArrayVariableReferenceExpression ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEdoubleArrayVariableReferenceExpression ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEArrayVariableReferenceExpression(pExpression, "doubleArray");

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEdoubleArrayVariableReferenceExpression (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEfloatArrayVariableReferenceExpression
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEfloatArrayVariableReferenceExpression ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEfloatArrayVariableReferenceExpression ( SgExpression *pExpression )\n");
     }
   
   bretValue = isROSEArrayVariableReferenceExpression(pExpression, "floatArray");

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEfloatArrayVariableReferenceExpression (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEintArrayVariableReferenceExpression
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEintArrayVariableReferenceExpression ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEintArrayVariableReferenceExpression ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEArrayVariableReferenceExpression(pExpression, "intArray");

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEintArrayVariableReferenceExpression (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayVariableReferenceExpression
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayVariableReferenceExpression ( SgExpression *pExpression, const char *sName )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface:isROSEArrayVariableReferenceExpression ( SgExpression *pExpression, const char *sName ):\n");
     }

   switch ( pExpression->variant())
     {
     case VAR_REF: 
       {
	 SgVarRefExp *pExpr = isSgVarRefExp(pExpression);
	 if ( pExpr != NULL ) 
	   {
	     // now make sure it is a reference and then ensure it's an array reference
	     SgVariableSymbol *pSymbol = pExpr->get_symbol();
	     SgReferenceType *pRefType = isSgReferenceType( pSymbol->get_type() );
	     if( pRefType != NULL )
	       {
		 SgClassType *pClassType = isSgClassType( pRefType->get_base_type());
		 if( (pClassType != NULL ) && ( !strcmp( sName, (pClassType->get_name()).str() ) ) ) 
		   bretValue = TRUE;
	       }
	   }
	 break;
       }
     default:
       break;
     }   

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayVariableReferenceExpression ( SgExpression *pExpression, const char *sName )\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayVariablePointerExpression
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayVariablePointerExpression ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayVariablePointerExpression ( SgExpression *pExpression )\n");
     }

   bretValue = isROSERealArrayVariablePointerExpression( pExpression ) || \
               isROSEintArrayVariablePointerExpression( pExpression );
 

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayVariablePointerExpression (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSERealArrayVariablePointerExpression
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSERealArrayVariablePointerExpression ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSERealArrayVariablePointerExpression ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEdoubleArrayVariablePointerExpression( pExpression ) || \
               isROSEfloatArrayVariablePointerExpression( pExpression );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSERealArrayVariablePointerExpression (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEdoubleArrayVariablePointerExpression
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEdoubleArrayVariablePointerExpression ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEdoubleArrayVariablePointerExpression ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEArrayVariablePointerExpression(pExpression, "doubleArray");

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEdoubleArrayVariablePointerExpression (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEfloatArrayVariablePointerExpression
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEfloatArrayVariablePointerExpression ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEfloatArrayVariablePointerExpression ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEArrayVariablePointerExpression(pExpression, "floatArray");   

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEfloatArrayVariablePointerExpression (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEintArrayVariablePointerExpression
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEintArrayVariablePointerExpression ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEintArrayVariablePointerExpression ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEArrayVariablePointerExpression(pExpression, "intArray");

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEintArrayVariablePointerExpression (  SgExpression *pExpression)\n");
     }

   return bretValue;
}

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayVariablePointerExpression
// Arguments  : SgExpression *pExpression, const char *sName
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayVariablePointerExpression ( SgExpression *pExpression, const char *sName )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface:isROSEArrayVariablePointerExpression ( SgExpression *pExpression, const char *sName ):\n");
     }

   switch ( pExpression->variant())
     {
     case VAR_REF: 
       {
	 SgVarRefExp *pExpr = isSgVarRefExp(pExpression);
	 if ( pExpr != NULL ) 
	   {
	     // now make sure it is a reference and then ensure it's an array reference
	     SgVariableSymbol *pSymbol = pExpr->get_symbol();
	     // will probably have to tighten this to include member pointers too
	     SgPointerType *pType = isSgPointerType( pSymbol->get_type() );
	     if( pType != NULL )
	       {
		 SgClassType *pClassType = isSgClassType( pType->get_base_type());
		 if( (pClassType != NULL ) && ( !strcmp( sName, (pClassType->get_name()).str() ) ) ) 
		   bretValue = TRUE;
	       }
	   }
	 break;
       }
     default:
       break;
     }   

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayVariablePointerExpression ( SgExpression *pExpression, const char *sName )\n");
     }

   return bretValue;
}
          
//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayVariableDereferencedPointerExpression
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayVariableDereferencedPointerExpression ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayVariableDereferencedPointerExpression ( SgExpression *pExpression )\n");
     }

   bretValue = isROSERealArrayVariableDereferencedPointerExpression( pExpression ) || \
               isROSEintArrayVariableDereferencedPointerExpression( pExpression );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayVariableDereferencedPointerExpression ( SgExpression *pExpression )\n");
     }

   return bretValue;
}
     
          
//-------------------------------------------------------------------------------------------------
// Function   : isROSERealArrayVariableDereferencedPointerExpression
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSERealArrayVariableDereferencedPointerExpression ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSERealArrayVariableDereferencedPointerExpression ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEdoubleArrayVariableDereferencedPointerExpression( pExpression ) || \
               isROSEfloatArrayVariableDereferencedPointerExpression( pExpression );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSERealArrayVariableDereferencedPointerExpression ( SgExpression *pExpression )\n");
     }

   return bretValue;
}
     
          
//-------------------------------------------------------------------------------------------------
// Function   : isROSEdoubleArrayVariableDereferencedPointerExpression
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEdoubleArrayVariableDereferencedPointerExpression ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   // may need to add more cases as we come across wierd stuff
   switch ( pExpression->variant())
     {
     case FUNC_CALL:
       {
	 bretValue = isROSEdoubleArrayVariableDereferencedPointerExpression( ((SgFunctionCallExp *)pExpression )->get_function() );
	 // expressions like '*pA = 1.0;' get mapped to the arrow operator so array pointer dereferencing 
	 // takes place in this fashion
	 break;
       }
     case POINTST_OP:
       {
	 SgArrowExp *pExp = isSgArrowExp( pExpression );
	 if( pExp != NULL )
	   bretValue = isROSEdoubleArrayVariablePointerExpression( pExp->get_lhs_operand() );
	 break;
       }
     default:
       break;
     }

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEdoubleArrayVariableDereferencedPointerExpression ( SgExpression *pExpression )\n");
     }


   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEdoubleArrayVariableDereferencedPointerExpression ( SgExpression *pExpression )\n");
     }

   return bretValue;
}
     
          
//-------------------------------------------------------------------------------------------------
// Function   : isROSEfloatArrayVariableDereferencedPointerExpression
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEfloatArrayVariableDereferencedPointerExpression ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEfloatArrayVariableDereferencedPointerExpression ( SgExpression *pExpression )\n");
     }

   // may need to add more cases as we come across wierd stuff
   switch ( pExpression->variant())
     {
     case FUNC_CALL:
       {
	 bretValue = isROSEfloatArrayVariableDereferencedPointerExpression( ((SgFunctionCallExp *)pExpression )->get_function() );
	 // expressions like '*pA = 1.0;' get mapped to the arrow operator so array pointer dereferencing 
	 // takes place in this fashion
	 break;
       }
     case POINTST_OP:
       {
	 SgArrowExp *pExp = isSgArrowExp( pExpression );
	 if( pExp != NULL )
	   bretValue = isROSEfloatArrayVariablePointerExpression( pExp->get_lhs_operand() );
	 break;
       }
     default:
       break;
     }

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEfloatArrayVariableDereferencedPointerExpression ( SgExpression *pExpression )\n");
     }

   return bretValue;
}
     
          
//-------------------------------------------------------------------------------------------------
// Function   : isROSEintArrayVariableDereferencedPointerExpression
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEintArrayVariableDereferencedPointerExpression ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEintArrayVariableDereferencedPointerExpression ( SgExpression *pExpression )\n");
     }

   // may need to add more cases as we come across wierd stuff
   switch ( pExpression->variant())
     {
     case FUNC_CALL:
       {
	 bretValue = isROSEintArrayVariableDereferencedPointerExpression( ((SgFunctionCallExp *)pExpression )->get_function() );
	 // expressions like '*pA = 1.0;' get mapped to the arrow operator so array pointer dereferencing 
	 // takes place in this fashion
	 break;
       }
     case POINTST_OP:
       {
	 SgArrowExp *pExp = isSgArrowExp( pExpression );
	 if( pExp != NULL )
	   bretValue = isROSEintArrayVariablePointerExpression( pExp->get_lhs_operand() );
	 break;
       }
     default:
       break;
     }

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEintArrayVariableDereferencedPointerExpression ( SgExpression *pExpression )\n");
     }

   return bretValue;
}
     
//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayParenthesisOperator
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayParenthesisOperator ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayParenthesisOperator ( SgExpression *pExpression )\n");
     }

   bretValue = verifyArrayBinaryMemberOperator( pExpression, "operator()");

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayParenthesisOperator (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayFunctionExpression
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayFunctionExpression ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayFunctionExpression ( SgExpression *pExpression )\n");
     }

   bretValue = isROSEArrayCOS ( pExpression )        || \
               isROSEArraySIN ( pExpression )        || \
               isROSEArrayTAN ( pExpression )        || \
               isROSEArrayMIN ( pExpression )        || \
               isROSEArrayMAX ( pExpression )        || \
               isROSEArrayFMOD ( pExpression )       || \
               isROSEArrayMOD ( pExpression )        || \
               isROSEArrayPOW ( pExpression )        || \
               isROSEArraySIGN ( pExpression )       || \
               isROSEArrayLOG ( pExpression )        || \
               isROSEArrayLOG10 ( pExpression )      || \
               isROSEArrayEXP ( pExpression )        || \
               isROSEArraySQRT ( pExpression )       || \
               isROSEArrayFABS ( pExpression )       || \
               isROSEArrayCEIL ( pExpression )       || \
               isROSEArrayFLOOR ( pExpression )      || \
               isROSEArrayABS ( pExpression )        || \
               isROSEArrayTRANSPOSE ( pExpression )  || \
               isROSEArrayACOS ( pExpression )       || \
               isROSEArrayASIN ( pExpression )       || \
               isROSEArrayATAN ( pExpression )       || \
               isROSEArrayCOSH ( pExpression )       || \
               isROSEArraySINH ( pExpression )       || \
               isROSEArrayTANH ( pExpression )       || \
               isROSEArrayACOSH ( pExpression )      || \
               isROSEArrayASINH ( pExpression )      || \
               isROSEArrayATANH ( pExpression );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayFunctionExpression (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayCOS
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayCOS ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayCOS ( SgExpression *pExpression )\n");
     }

   if ( verifyArrayClassFriendFunction(pExpression, "cos") )
     bretValue = verifySingleArgumentForArrayFriendFunction(pExpression);

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayCOS (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArraySIN
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArraySIN ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArraySIN ( SgExpression *pExpression )\n");
     }

   if ( verifyArrayClassFriendFunction(pExpression, "sin") )
     bretValue = verifySingleArgumentForArrayFriendFunction(pExpression);

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArraySIN (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayTAN
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayTAN ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayTAN ( SgExpression *pExpression )\n");
     }


   if ( verifyArrayClassFriendFunction(pExpression, "tan") )
     bretValue = verifySingleArgumentForArrayFriendFunction(pExpression);

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayTAN (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayMIN
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayMIN ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayMIN ( SgExpression *pExpression )\n");
     }

   if ( verifyArrayClassFriendFunction(pExpression, "min") )
     bretValue = verifyArgumentsForArrayFriendFunction( pExpression );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayMIN (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayMAX
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayMAX ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayMAX ( SgExpression *pExpression )\n");
     }

   if ( verifyArrayClassFriendFunction(pExpression, "max") )
     bretValue = verifyArgumentsForArrayFriendFunction( pExpression );

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayMAX (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayFMOD
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayFMOD ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayFMOD ( SgExpression *pExpression )\n");
     }

   if ( verifyArrayClassFriendFunction(pExpression, "fmod") )
     bretValue = verifyTwoArgumentsForArrayFriendFunction(pExpression);

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayFMOD (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayMOD
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayMOD ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayMOD ( SgExpression *pExpression )\n");
     }

   if ( verifyArrayClassFriendFunction(pExpression, "mod") )
     bretValue = verifyTwoArgumentsForArrayFriendFunction(pExpression); 

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayMOD (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayPOW
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayPOW ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayPOW ( SgExpression *pExpression )\n");
     }

   if ( verifyArrayClassFriendFunction(pExpression, "pow") )
     bretValue = verifyTwoArgumentsForArrayFriendFunction(pExpression); 

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayPOW (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArraySIGN
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArraySIGN ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArraySIGN ( SgExpression *pExpression )\n");
     }

   if ( verifyArrayClassFriendFunction(pExpression, "sign") )
     bretValue = verifyTwoArgumentsForArrayFriendFunction(pExpression); 

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArraySIGN (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayLOG
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayLOG ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayLOG ( SgExpression *pExpression )\n");
     }

   if ( verifyArrayClassFriendFunction(pExpression, "log") )
     bretValue = verifySingleArgumentForArrayFriendFunction(pExpression); 

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayLOG (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayLOG10
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayLOG10 ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayLOG10 ( SgExpression *pExpression )\n");
     }


   if ( verifyArrayClassFriendFunction(pExpression, "log10") )
     bretValue = verifySingleArgumentForArrayFriendFunction(pExpression); 

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayLOG10 (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayEXP
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayEXP ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayEXP ( SgExpression *pExpression )\n");
     }

   if ( verifyArrayClassFriendFunction(pExpression, "exp") )
     bretValue = verifySingleArgumentForArrayFriendFunction(pExpression); 

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayEXP (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArraySQRT
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArraySQRT ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArraySQRT ( SgExpression *pExpression )\n");
     }

   if ( verifyArrayClassFriendFunction(pExpression, "sqrt") )
     bretValue = verifySingleArgumentForArrayFriendFunction(pExpression); 

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArraySQRT (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayFABS
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayFABS ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayFABS ( SgExpression *pExpression )\n");
     }

   if ( verifyArrayClassFriendFunction(pExpression, "fabs") )
     bretValue = verifySingleArgumentForArrayFriendFunction(pExpression); 

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayFABS (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayCEIL
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayCEIL ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayCEIL ( SgExpression *pExpression )\n");
     }

   if ( verifyArrayClassFriendFunction(pExpression, "ceil") )
     bretValue = verifySingleArgumentForArrayFriendFunction(pExpression); 

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayCEIL (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayFLOOR
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayFLOOR ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayFLOOR ( SgExpression *pExpression )\n");
     }


   if ( verifyArrayClassFriendFunction(pExpression, "floor") )
     bretValue = verifySingleArgumentForArrayFriendFunction(pExpression); 

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayFLOOR (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayABS
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayABS ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayABS ( SgExpression *pExpression )\n");
     }


   if ( verifyArrayClassFriendFunction(pExpression, "abs") )
     bretValue = verifySingleArgumentForArrayFriendFunction(pExpression); 

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayABS (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayTRANSPOSE
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayTRANSPOSE ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayTRANSPOSE ( SgExpression *pExpression )\n");
     }

   if ( verifyArrayClassFriendFunction(pExpression, "transpose") )
     bretValue = verifySingleArgumentForArrayFriendFunction(pExpression); 

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayTRANSPOSE (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayACOS
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayACOS ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayACOS ( SgExpression *pExpression )\n");
     }


   if ( verifyArrayClassFriendFunction(pExpression, "acos") )
     bretValue = verifySingleArgumentForArrayFriendFunction(pExpression); 

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayACOS (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayASIN
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayASIN ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayASIN ( SgExpression *pExpression )\n");
     }

   if ( verifyArrayClassFriendFunction(pExpression, "asin") )
     bretValue = verifySingleArgumentForArrayFriendFunction(pExpression); 

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayASIN (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayATAN
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayATAN ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayATAN ( SgExpression *pExpression )\n");
     }

   if ( verifyArrayClassFriendFunction(pExpression, "atan") )
     bretValue = verifySingleArgumentForArrayFriendFunction(pExpression); 

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayATAN (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayCOSH
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayCOSH ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayCOSH ( SgExpression *pExpression )\n");
     }

   if ( verifyArrayClassFriendFunction(pExpression, "cosh") )
     bretValue = verifySingleArgumentForArrayFriendFunction(pExpression); 

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayCOSH (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArraySINH
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArraySINH ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArraySINH ( SgExpression *pExpression )\n");
     }

   if ( verifyArrayClassFriendFunction(pExpression, "sinh") )
     bretValue = verifySingleArgumentForArrayFriendFunction(pExpression); 

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArraySINH (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayTANH
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayTANH ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayTANH ( SgExpression *pExpression )\n");
     }

   if ( verifyArrayClassFriendFunction(pExpression, "tanh") )
     bretValue = verifySingleArgumentForArrayFriendFunction(pExpression); 

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayTANH (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayACOSH
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayACOSH ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayACOSH ( SgExpression *pExpression )\n");
     }

   if ( verifyArrayClassFriendFunction(pExpression, "acosh") )
     bretValue = verifySingleArgumentForArrayFriendFunction(pExpression); 

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayACOSH (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayASINH
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayASINH ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayASINH ( SgExpression *pExpression )\n");
     }

   if ( verifyArrayClassFriendFunction(pExpression, "asinh") )
     bretValue = verifySingleArgumentForArrayFriendFunction(pExpression); 
  

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayASINH (  SgExpression *pExpression)\n");
     }

   return bretValue;
}
     

//-------------------------------------------------------------------------------------------------
// Function   : isROSEArrayATANH
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::isROSEArrayATANH ( SgExpression *pExpression )
{
   Boolean bretValue = FALSE;
   ROSE_ASSERT( pExpression != NULL );

   if ( bDebug == verbose_debug_level )
     {
	printf("Entering ArrayClassSageInterface::isROSEArrayATANH ( SgExpression *pExpression )\n");
     }

   if ( verifyArrayClassFriendFunction(pExpression, "atanh") )
     bretValue = verifySingleArgumentForArrayFriendFunction(pExpression); 

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::isROSEArrayATANH (  SgExpression *pExpression)\n");
     }

   return bretValue;
}

//-------------------------------------------------------------------------------------------------
// Function   : getArrayMemberFunctionDeclaration
// Arguments  : SgMemberFunctionRefExp *pMemberFunctionReference
// Comments   :
//-------------------------------------------------------------------------------------------------

SgFunctionDeclaration *ArrayClassSageInterface::getArrayMemberFunctionDeclaration ( SgMemberFunctionRefExp *pMemberFunctionReference )
{
  SgFunctionDeclaration *pReturnSgFunctionDeclaration = NULL;

  if ( bDebug == verbose_debug_level )
    {
	printf("Entering ArrayClassSageInterface:: getArrayMemberFunctionDeclaration( SgMemberFunctionRefExp *pMemberFunctionReference )\n");
    }

  if ( pMemberFunctionReference != NULL )
    {
      SgMemberFunctionSymbol *pMemberFunctionSymbol = pMemberFunctionReference->get_symbol();
      SgMemberFunctionDeclaration* pMemberFunctionDeclaration = pMemberFunctionSymbol->get_declaration();
      ROSE_ASSERT ( pMemberFunctionDeclaration!=NULL );
      SgClassDefinition *pClass = pMemberFunctionDeclaration->get_scope();
      const char *psClassName = getClassName(pClass);
      if ( isArrayClassType(psClassName) )
	{
	  SgFunctionDefinition *pfunctionDefinition = pMemberFunctionDeclaration->get_definition();
	  ROSE_ASSERT (pfunctionDefinition != NULL);
	  pReturnSgFunctionDeclaration = pfunctionDefinition->get_declaration();
	}
    }   

   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::getArrayMemberFunctionDeclaration ( SgMemberFunctionRefExp *pMemberFunctionReference )\n");
     }

   return pReturnSgFunctionDeclaration;
}

//-------------------------------------------------------------------------------------------------
// Function   : getArrayMemberFunctionDeclaration
// Arguments  : SgFunctionCallExp *pSgFunctionCallExp
// Comments   :
//-------------------------------------------------------------------------------------------------

SgFunctionDeclaration *ArrayClassSageInterface::getArrayMemberFunctionDeclaration ( SgFunctionCallExp *pSgFunctionCallExp )
{
  SgFunctionDeclaration *pReturnSgFunctionDeclaration = NULL;

  ROSE_ASSERT( pSgFunctionCallExp!= NULL );

  if ( bDebug == verbose_debug_level )
    {
	printf("Entering ArrayClassSageInterface:: getArrayMemberFunctionDeclaration( SgMemberFunctionRefExp *pMemberFunctionReference )\n");
    }

   SgExpression *pFunctionExp = pSgFunctionCallExp->get_function();
   ROSE_ASSERT ( pFunctionExp!= NULL );
   switch ( pFunctionExp->variant() )
     { // currently will only consider the case of a RECORD_REF & an arrow operator
     case RECORD_REF:
       { 
	 SgDotExp *pDotExp = isSgDotExp( pFunctionExp );
	 ROSE_ASSERT( pDotExp != NULL );
	 SgExpression *pRhsExpression = pDotExp->get_rhs_operand();
	 ROSE_ASSERT ( pRhsExpression != NULL );
	 SgMemberFunctionRefExp *pMemberFunctionReference = isSgMemberFunctionRefExp(pRhsExpression);
	 pReturnSgFunctionDeclaration = getArrayMemberFunctionDeclaration(pMemberFunctionReference);
	 break;
     }

     case POINTST_OP:
       {
	 SgArrowExp *pExp = isSgArrowExp( pFunctionExp );
	 ROSE_ASSERT( pExp != NULL );
	 SgExpression *pRhsExpression = pExp->get_rhs_operand();
	 ROSE_ASSERT ( pRhsExpression != NULL );
	 SgMemberFunctionRefExp *pMemberFunctionReference = isSgMemberFunctionRefExp(pRhsExpression);
	 pReturnSgFunctionDeclaration = getArrayMemberFunctionDeclaration(pMemberFunctionReference);
	 break;
       }
     default:
       break;
     }
   if ( bDebug == verbose_debug_level )
     {
	printf("Exiting ArrayClassSageInterface::getArrayMemberFunctionDeclaration ( SgMemberFunctionRefExp *pMemberFunctionReference )\n");
     }

   return pReturnSgFunctionDeclaration;
}

//-------------------------------------------------------------------------------------------------
// Function   : getArrayFriendFunctionDeclaration
// Arguments  : SgFunctionRefExp *pFunctionRefExp
// Comments   :
//-------------------------------------------------------------------------------------------------

SgFunctionDeclaration *ArrayClassSageInterface::getArrayFriendFunctionDeclaration ( SgFunctionRefExp *pFunctionRefExp )
{
  SgFunctionDeclaration *pReturnSgFunctionDeclaration = NULL;

  ROSE_ASSERT( pFunctionRefExp != NULL );

  if ( bDebug == verbose_debug_level )
    {
      printf("Entering ArrayClassSageInterface:: getArrayFriendFunctionDeclaration( SgFunctionRefExp *pFunctionRefExp )\n");
    }
  
  SgFunctionSymbol *pFunctionSymbol = pFunctionRefExp->get_symbol();
  ROSE_ASSERT ( pFunctionSymbol != NULL );
  SgFunctionDeclaration* pSgFunctionDecl = pFunctionSymbol->get_declaration();
  ROSE_ASSERT( pSgFunctionDecl!= NULL );
  SgFunctionDefinition *pfunctionDefinition = pSgFunctionDecl->get_definition();
  ROSE_ASSERT (pfunctionDefinition != NULL);
  SgFunctionDeclaration* pfunctionDeclarationStatement = pfunctionDefinition->get_declaration();
  pReturnSgFunctionDeclaration = pfunctionDeclarationStatement;

  if ( bDebug == verbose_debug_level )
    {
      printf("Exiting ArrayClassSageInterface:: getArrayFriendFunctionDeclaration( SgFunctionRefExp *pFunctionRefExp )\n");
    }

  return pReturnSgFunctionDeclaration;
}


//-------------------------------------------------------------------------------------------------
// Function   : verifyArgumentsForArrayBinaryFriendOperator
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::verifyArgumentsForArrayBinaryFriendOperator ( SgExpression *pExpression )
{
  Boolean bretValue = FALSE;

  ROSE_ASSERT( pExpression != NULL );

  if ( bDebug == verbose_debug_level )
    {
      printf("Entering ArrayClassSageInterface:: verifyArgumentsForArrayBinaryFriendOperator( SgExpression *pExpression )\n");
    }

  SgFunctionRefExp *pRefExpression = getArrayClassFriendOperator( pExpression );

  if ( pRefExpression != NULL )
    {
      SgFunctionDeclaration* pfunctionDeclaration = getArrayFriendFunctionDeclaration( pRefExpression );

      if ( pfunctionDeclaration!= NULL )
	{
	  // now proceed to verify arguments
	  SgInitializedNameList &argList = pfunctionDeclaration->get_args();
       // if( ( argList != NULL ) && ( argList.size() == 2) )
	  if( argList.size() == 2 )
	    {
	      SgInitializedNameList::iterator i = argList.begin(); 
	      bretValue = FALSE;
	      SgType *argTypes[2];
	      int j = 0;
	      while (i != argList.end()) 
		{
		  SgInitializedName argPtr = *i;
		  argTypes[j++] = getType(argPtr.get_type());  // get the basic type
		  i++;
		}
	      // now check for the exact argument types
	      // case 1: both are class types
	      if ( (argTypes[0]->variant() == T_CLASS ) && ( argTypes[1]->variant() == T_CLASS ) )
		{
		  if ( isArrayClassType(argTypes[0]) )
		    {
			  // sorry!!
		      if( !strcmp ( ( ( (SgClassType *)argTypes[0] )->get_name()).str(),( ( (SgClassType *)argTypes[1] )->get_name()).str() ) )
			bretValue = TRUE;
		    }
		}
	      // case 2: one is a class type, the other isn't
	      if ( (argTypes[0]->variant() == T_CLASS ) && ( argTypes[1]->variant() != T_CLASS ) )	      
		{
		  // not a sufficiently concrete test, will modify later
		  if ( isArrayClassType(argTypes[0]) )
		    bretValue = TRUE;
		}
	      // case 3: one is a class type, the other isn't
	      if ( (argTypes[0]->variant() != T_CLASS ) && ( argTypes[1]->variant() == T_CLASS ) )
		{// not a sufficiently concrete test, will modify later
		  if ( isArrayClassType(argTypes[1]) )
		    bretValue = TRUE;
		}	       
	    }
	}
    }

  if ( bDebug == verbose_debug_level ) 
    {
      printf("Exiting ArrayClassSageInterface:: verifyArgumentsForArrayBinaryFriendOperator( SgExpression *pExpression )\n");
    }

  return bretValue; 
}


//-------------------------------------------------------------------------------------------------
// Function   : verifyTwoArgumentsForArrayFriendFunction
// Arguments  : SgFunctionCallExp *pSgFunctionCallExp
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::verifyTwoArgumentsForArrayFriendFunction ( SgExpression *pExpression )
{ 
  Boolean bretValue = FALSE;

  ROSE_ASSERT( pExpression != NULL );
 
  if ( bDebug == verbose_debug_level )
    {
      printf("Entering ArrayClassSageInterface:: verifyTwoArgumentsForArrayFriendFunction( SgExpression *pExpression )\n");
    }

  SgFunctionRefExp *pRefExpression = getArrayClassFriendFunction( pExpression );

  if ( pRefExpression != NULL ) 
    {
      SgFunctionDeclaration* pfunctionDeclaration = getArrayFriendFunctionDeclaration( pRefExpression );

      if ( pfunctionDeclaration!= NULL ) 
	{
	  // now proceed to verify arguments
	  SgInitializedNameList &argList = pfunctionDeclaration->get_args();
       // if( ( argList != NULL ) && ( argList.size() == 2) )
	  if( argList.size() == 2 )
	    {
	      SgInitializedNameList ::iterator i = argList.begin(); 
	      bretValue = FALSE;
	      SgType *argTypes[2];
	      int j = 0;
	      while (i != argList.end()) 
		{
		  SgInitializedName argPtr = *i;
		  argTypes[j++] = getType(argPtr.get_type());  // get the basic type
		  i++;
		}
	      // now check for the exact argument types
	      // case 1: both are class types
	      if ( (argTypes[0]->variant() == T_CLASS ) && ( argTypes[1]->variant() == T_CLASS ) )
		{
		  if ( isArrayClassType(argTypes[0]) )
		    {
		      // sorry!!
		      if( !strcmp ( ( ( (SgClassType *)argTypes[0] )->get_name()).str(),( ( (SgClassType *)argTypes[1] )->get_name()).str() ) )
			bretValue = TRUE;
		    }
		}
	      // case 2: one is a class type, the other isn't
	      if ( (argTypes[0]->variant() == T_CLASS ) && ( argTypes[1]->variant() != T_CLASS ) )	      
		{
		  // not a sufficiently concrete test, will modify later
		  if ( isArrayClassType(argTypes[0]) )
		    bretValue = TRUE;
		}
	      // case 3: one is a class type, the other isn't
	      if ( (argTypes[0]->variant() != T_CLASS ) && ( argTypes[1]->variant() == T_CLASS ) )
		{// not a sufficiently concrete test, will modify later
		  if ( isArrayClassType(argTypes[1]) )
		    bretValue = TRUE;
		}	       
	    }
	}
    }
 
  if ( bDebug == verbose_debug_level )
    {
      printf("Exiting ArrayClassSageInterface:: verifyTwoArgumentsForArrayFriendFunction(SgExpression *pExpression )\n");
    }

  return bretValue;
}

//-------------------------------------------------------------------------------------------------
// Function   : verifySingleArgumentForArrayFriendFunction
// Arguments  : SgExpression *pExpression
// Comments   :
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::verifySingleArgumentForArrayFriendFunction ( SgExpression *pExpression )
{
  Boolean bretValue = FALSE; 

  ROSE_ASSERT( pExpression != NULL );

  if ( bDebug == verbose_debug_level )
    {
      printf("Entering ArrayClassSageInterface:: verifySingleArgumentForArrayFriendFunction( SgExpression *pExpression )\n");
    }

  SgFunctionRefExp *pRefExpression = getArrayClassFriendFunction( pExpression );

  if ( pRefExpression != NULL )
    {
      SgFunctionDeclaration* pfunctionDeclaration = getArrayFriendFunctionDeclaration( pRefExpression );
 
      if ( pfunctionDeclaration!= NULL )
	{
	  // now proceed to verify arguments
	  SgInitializedNameList &argList = pfunctionDeclaration->get_args();
       // if( ( argList != NULL ) && ( argList.size() == 1) )
	  if( argList.size() == 1 )
	    {
	      SgInitializedNameList::iterator i = argList.begin(); 
	      SgType *argType;
	      SgInitializedName argPtr = *i;
	      argType = getType(argPtr.get_type());  // get the basic type
	      if  ( (argType != NULL ) && (argType->variant() == T_CLASS ) && ( isArrayClassType(argType) ) )
		bretValue = TRUE;
	    }
	}
    }

  if ( bDebug == verbose_debug_level )
    {
      printf("Exiting ArrayClassSageInterface:: verifySingleArgumentForArrayFriendFunction( SgExpression *pExpression )\n");
    }

  return bretValue;
}

//-------------------------------------------------------------------------------------------------
// Function   : verifyArgumentsForArrayFriendFunction
// Arguments  : SgExpression *pExpression
// Comments   : simply checks whether atleast one argument to the function is an array class type
//-------------------------------------------------------------------------------------------------

Boolean ArrayClassSageInterface::verifyArgumentsForArrayFriendFunction ( SgExpression *pExpression )
{ 
  Boolean bretValue = FALSE;

  ROSE_ASSERT( pExpression != NULL );
 
  if ( bDebug == verbose_debug_level )
    {
      printf("Entering ArrayClassSageInterface:: verifyArgumentsForArrayFriendFunction( SgExpression *pExpression )\n");
    }

  SgFunctionRefExp *pRefExpression = getArrayClassFriendFunction( pExpression );

  if ( pRefExpression != NULL ) 
    {
      SgFunctionDeclaration* pfunctionDeclaration = getArrayFriendFunctionDeclaration( pRefExpression );

      if ( pfunctionDeclaration!= NULL ) 
	{
	  // now proceed to verify arguments
	  SgInitializedNameList &argList = pfunctionDeclaration->get_args();
       // if( ( argList != NULL ) && ( argList.size() > 0 ) )
	  if( argList.size() > 0 )
	    {
	      SgInitializedNameList ::iterator i = argList.begin(); 
	      while (i != argList.end()) 
		{
		  SgInitializedName argPtr = *i;
		  SgType *pType = getType(argPtr.get_type());  // get the basic type
		  if( (pType != NULL ) && ( isArrayClassType( pType ) ) )
		    {
		      bretValue = TRUE;
		      break;
		    }
		  i++;
		}
	    }
	}
    }
 
  if ( bDebug == verbose_debug_level )
    {
      printf("Exiting ArrayClassSageInterface:: verifyArgumentsForArrayFriendFunction(SgExpression *pExpression )\n");
    }

  return bretValue;
}

//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
// code that's NEW
//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------

SgMemberFunctionRefExp *ArrayClassSageInterface::getArrayClassMemberOperator( SgExpression *pSgExpression)
{
  SgMemberFunctionRefExp *pReturnExp = NULL;
  
  if( bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::getArrayClassMemberOperator( SgExpression *pExpression) \n" );

  ROSE_ASSERT( pSgExpression!= NULL);
  // this function is by no means assumed to be complete, additions will be made from time
  // to time as we learn more.
  // we only consider the cases SgFunctionCallExp, SgFunctionRefExp
  // SgFunctionCallExp may in turn deal with SgDotExp, SgArrowExp, SgFunctionRefExp

  switch ( pSgExpression->variant() )
    {

    case VAR_REF: 
      {
	SgVarRefExp* varRefExp = isSgVarRefExp( pSgExpression );
	ROSE_ASSERT (varRefExp != NULL);
	if ( bDebug == verbose_debug_level)
	  cout << "encountered a variable reference expression" << endl;
	// still have to put in code
	break;
      }

    case MEMBER_FUNCTION_REF:
      {
	SgMemberFunctionRefExp *pMemberFunctionReference = isSgMemberFunctionRefExp(pSgExpression);
	ROSE_ASSERT ( pMemberFunctionReference != NULL );

	if ( isArrayClassMemberOperator( pMemberFunctionReference ) )
	  pReturnExp = pMemberFunctionReference;

	break;
      }

    case FUNC_CALL: 
      {
	SgFunctionCallExp *pFunctionCallExp = isSgFunctionCallExp(pSgExpression);
	ROSE_ASSERT ( pFunctionCallExp != NULL );

	SgExpression *pFunctionExp = pFunctionCallExp->get_function();
	ROSE_ASSERT ( pFunctionExp!= NULL );
	pReturnExp = getArrayClassMemberOperator( pFunctionExp );   // recursive call
	break;
      }
 
    case POINTST_OP:    // am assuming this is the arrow operator, though we'll have to make sure
      {
	SgArrowExp *pArrowExp = isSgArrowExp(pSgExpression);
	ROSE_ASSERT ( pArrowExp != NULL );
	pReturnExp = getArrayClassMemberOperator( pArrowExp->get_rhs_operand() );
	break;
      }

    case RECORD_REF:
      {
	SgDotExp *pDotExp = isSgDotExp(pSgExpression);
	ROSE_ASSERT ( pDotExp != NULL );
        pReturnExp = getArrayClassMemberOperator( pDotExp->get_rhs_operand() );
	break;
      }

  case CONSTRUCTOR_INIT: 
    {// in this case we will access the argument list of the constructor, ensure
      // that it has only one argument and that that is a function call exp, in which case we
      // will continue to process it, we also ensure that it is a constructor of an array class
      // we do this since we are only interested in getting a array class member operator, in
      // particular we are searching for the paranthesis operator which gets hidden within
      // a constructor initializer
      SgConstructorInitializer* constructorInitializer = isSgConstructorInitializer( pSgExpression );
      ROSE_ASSERT (constructorInitializer != NULL);
      // member function declarations as well as class declarations are not usually available
      // and hence cannot be used to gain any information
      //      SgClassDeclaration *pDecl = constructorInitializer->get_class_decl();

      SgExprListExp *pArgList = constructorInitializer->get_args();
      if (  pArgList != NULL)
	{
	  SgExpressionPtrList expressionPtrList = pArgList->get_expressions();
	  if ( expressionPtrList.size() == 1 )
	    {
           // SgExpressionPtrList::iterator i = expressionPtrList.begin();
#if USE_SAGE3
           // Sage3 uses STL and this simplifies the syntax for using lists
	      pReturnExp = getArrayClassMemberOperator( *(expressionPtrList.begin()) );  
#else
	      SgExpressionPtr expressionPtr = *( expressionPtrList.begin() );
	      pReturnExp = getArrayClassMemberOperator( expressionPtr.operator->() );  
#endif
	    }
	}
      break;
    }

    default: 
      {
        break;
      }
    }
 
  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::getArrayClassMemberOperator \n" );

  return pReturnExp;
}

SgFunctionRefExp *ArrayClassSageInterface::getArrayClassFriendOperator( SgExpression *pExpression )
{
  SgFunctionRefExp * pReturnExp = NULL;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::getArrayClassFriendOperator( SgExpression *pExpression) \n" );

  ROSE_ASSERT( pExpression!= NULL );
  

  // this function is by no means assumed to be complete, additions will be made from time
  // to time as we learn more.
  // we only consider the cases SgFunctionCallExp, SgFunctionRefExp
  // SgFunctionCallExp may in turn deal with SgDotExp, SgArrowExp, SgFunctionRefExp

  switch ( pExpression->variant() )
    {
    case FUNC_CALL: 
      {
	SgFunctionCallExp *pFunctionCallExp = isSgFunctionCallExp(pExpression);
	ROSE_ASSERT ( pFunctionCallExp != NULL );

	SgExpression *pFunctionExp = pFunctionCallExp->get_function();
	ROSE_ASSERT ( pFunctionExp!= NULL );
	pReturnExp = getArrayClassFriendOperator( pFunctionExp );   // recursive call
	break;
      }
    case FUNCTION_REF:
      {
	SgFunctionRefExp *pFunctionRefExp = isSgFunctionRefExp(pExpression);
	ROSE_ASSERT ( pFunctionRefExp != NULL );
      
        if( isArrayClassFriendOperator( pFunctionRefExp->get_symbol() ) )
	  pReturnExp = pFunctionRefExp;
	break;
      }
    default:
      {
	break;
      }
    }

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::getArrayClassFriendOperator( SgExpression *pExpression) \n" );

  return pReturnExp;
}

SgFunctionRefExp *ArrayClassSageInterface::getArrayClassFriendFunction( SgExpression *pExpression)
{
  SgFunctionRefExp *pReturnExp = NULL;
  
  if(bDebug == verbose_debug_level)
    printf( "Entering ArrayClassSageInterface::getArrayClassFriendFunction( SgExpression *pExpression) \n" );

  ROSE_ASSERT( pExpression!= NULL);

  // this function is by no means assumed to be complete, additions will be made from time
  // to time as we learn more.
  // we only consider the cases SgFunctionCallExp, SgFunctionRefExp
  // SgFunctionCallExp may in turn deal with SgDotExp, SgArrowExp, SgFunctionRefExp

  switch ( pExpression->variant() )
    {
    case FUNC_CALL: 
      {
	SgFunctionCallExp *pFunctionCallExp = isSgFunctionCallExp(pExpression);
	ROSE_ASSERT ( pFunctionCallExp != NULL );

	SgExpression *pFunctionExp = pFunctionCallExp->get_function();
	ROSE_ASSERT ( pFunctionExp!= NULL );
	pReturnExp = getArrayClassFriendFunction( pFunctionExp );   // recursive call
	break;
      }
    case FUNCTION_REF:
      {
	SgFunctionRefExp *pFunctionRefExp = isSgFunctionRefExp(pExpression);
	ROSE_ASSERT ( pFunctionRefExp != NULL );
      
        if ( isArrayClassFriendFunction( pFunctionRefExp->get_symbol() ) )
	  pReturnExp = pFunctionRefExp;
	break;
      }
    default:
      {
	break;
      }
    }

  if(bDebug == verbose_debug_level)
    printf( "Exiting ArrayClassSageInterface::getArrayClassFriendFunction( SgExpression *pExpression) \n" );

  return pReturnExp;
}

Boolean ArrayClassSageInterface::verifyArrayBinaryFriendOperator( SgExpression *pExpression, const char *sOperatorName)
{
  Boolean bretValue = FALSE;

  if( (pExpression != NULL ) && ( sOperatorName != NULL) )
    {

      SgFunctionRefExp *pRefExp = getArrayClassFriendOperator( pExpression );
      
      if( pRefExp != NULL )
	{
	  SgFunctionDeclaration* pfunctionDeclarationStatement = getArrayFriendFunctionDeclaration( pRefExp );
	  
	  if ( (pfunctionDeclarationStatement != NULL) && \
	       (pfunctionDeclarationStatement->isOperator()) && (pfunctionDeclarationStatement->isFriend() ) )
	    {
	      // now we know we have a friend function which is also an operator
	      SgName sName = pfunctionDeclarationStatement->get_name();
	      if( !strcmp ( sName.str(), sOperatorName ) )  // verifying the name
		bretValue = TRUE;
		  
	    }
	}      
    }

  return bretValue;
}


Boolean ArrayClassSageInterface::verifyArrayBinaryMemberOperator( SgExpression *pExpression, const char *sOperatorName)
{
  Boolean bretValue = FALSE;

  if( (pExpression != NULL ) && ( sOperatorName != NULL) )
    {

      SgMemberFunctionRefExp *pExpr = getArrayClassMemberOperator( pExpression );

      if( pExpr != NULL )
	{
	  SgFunctionDeclaration* pfunctionDeclarationStatement = getArrayMemberFunctionDeclaration( pExpr );
	  
	  if((pfunctionDeclarationStatement != NULL) && (pfunctionDeclarationStatement->isOperator()))
	    {
	      SgName sName = pfunctionDeclarationStatement->get_name();
	      if(!strcmp(sName.str(), sOperatorName))
		bretValue = TRUE;
	    }
	}
    }

  return bretValue;
}

Boolean ArrayClassSageInterface::verifyArrayClassFriendFunction( SgExpression *pExpression, const char *sOperatorName) 
{
  Boolean bretValue = FALSE;

  if( (pExpression != NULL ) && ( sOperatorName != NULL) )
    {
      SgFunctionRefExp *pExpr = getArrayClassFriendFunction( pExpression );
      if( pExpr != NULL )
	{
	  SgFunctionDeclaration* pfunctionDeclarationStatement = getArrayFriendFunctionDeclaration( pExpr );
      
	  if ( (pfunctionDeclarationStatement != NULL) && \
	       (!pfunctionDeclarationStatement->isOperator()) && (pfunctionDeclarationStatement->isFriend() ) )
	    {
	      // now we know we have a friend function which is also an operator 
	      SgName sName = pfunctionDeclarationStatement->get_name();
	      if( !strcmp ( sName.str(), sOperatorName ) )  // verifying the name 
		bretValue = TRUE; 
	    }
	}
    }

  return bretValue; 
}

//----------------------------------------------------------------------------------------------------------------------------------------

// the getVariant function will return a ROSE_GrammarVariant given an expression. the function does not return a type variant as 
// i'm not sure whether such an issue will come up in the case of an expression.  a seperate getVariant function is provided for
// types and for statements.

//----------------------------------------------------------------------------------------------------------------------------------------

ROSE_GrammarVariants ArrayClassSageInterface::getVariant( SgExpression *pExpression)
{
  ROSE_GrammarVariants returnVariant = ROSE_UNKNOWN_GRAMMAR;

  if( pExpression != NULL )
    {
      switch ( pExpression->variant())
	{
	case UNARY_EXPRESSION:
	  break;
	case BINARY_EXPRESSION:
	  break;
#ifndef USE_SAGE3
	case ASSIGNMENT_EXPRESSION:
	  break;
#endif
	case EXPRESSION_ROOT:
	  break;
	case EXPR_LIST:  
	  {
	    if( isROSEIndexArgumentList( pExpression ) )
	      {
		returnVariant = ROSE_IndexArgumentListTag;
		if( bDebug >= normal_debug_level ) 
		  cout << endl << "Return Variant is "<< returnVariant << endl;
	      }

	    break;  
	  }
	case VAR_REF:   
	  {
	    for ( int i = 0; i < n_arrayVariableInstances; i++ )
	      {
		if( ( *( arrayVariableInstances[i].functionPointer) )(pExpression) )
		  {
		    returnVariant = arrayVariableInstances[i].variant;
		    if( bDebug >= normal_debug_level ) 
		      cout << endl << "Return Variant is "<< returnVariant << endl;
		    break;
		  }
	      }
	    // we return a variant for a C expression if everything else fails - hopefully 
	    // a good idea or else a headache!
	    if ( returnVariant == ROSE_UNKNOWN_GRAMMAR )
	      returnVariant = ROSE_C_ExpressionTag;
	    break; 
	  }
	case CLASSNAME_REF:  
	  { // maybe need to add code here
	    break; 
	  }
	case FUNCTION_REF:  
	  {
	    for ( int i = 0; i < n_arrayFriendFunctions; i++ )
	      {
		if( ( *( arrayFriendFunctions[i].functionPointer) )(pExpression) )
		  {
		    returnVariant = arrayFriendFunctions[i].variant;
		    if( bDebug >= normal_debug_level ) 
		      cout << endl << "Return Variant is "<< returnVariant << endl;
		    break;
		  }
	      }
	    // if we haven't found it as yet
	    if ( returnVariant == ROSE_UNKNOWN_GRAMMAR )
	      {
		for ( int i = 0; i < n_arrayFriendOperators; i++ )
		  {
		    if( ( *( arrayFriendOperators[i].functionPointer) )(pExpression) )
		      {
			returnVariant = arrayFriendOperators[i].variant;
			if( bDebug >= normal_debug_level ) 
			  cout << endl << "Return Variant is "<< returnVariant << endl;
			break;
		      }
		  }
		// if we haven't found it as yet, see if it's an index expression
		if ( returnVariant == ROSE_UNKNOWN_GRAMMAR )
		  {
		    for ( int i = 0; i < n_indexExpressions; i++ )
		      {
			if( ( *( indexExpressions[i].functionPointer) )(pExpression) )
			  {
			    returnVariant = indexExpressions[i].variant;
			    if( bDebug >= normal_debug_level ) 
			      cout << endl << "Return Variant is "<< returnVariant << endl;
			    break;
			  }
		      }
		  }
	      }
	    // we know it's a function call and can't identify it as one of our own
	    // so we'll return it as a user function 
	    if ( returnVariant == ROSE_UNKNOWN_GRAMMAR )
	      returnVariant = ROSE_C_ExpressionTag;
	    break;
	  }
	case MEMBER_FUNCTION_REF:
	  {
	    for ( int i = 0; i < n_arrayMemberOperators; i++ )
	      {
		if( ( *( arrayMemberOperators[i].functionPointer) )(pExpression) )
		  {
		    returnVariant = arrayMemberOperators[i].variant;
		    if( bDebug >= normal_debug_level ) 
		      cout << endl << "Return Variant is "<< returnVariant << endl;
		    break;
		  }
	      }
	    // if we haven't found it as yet, see if it's an index expression
	    if ( returnVariant == ROSE_UNKNOWN_GRAMMAR )
	      {
		for ( int i = 0; i < n_indexExpressions; i++ )
		  {
		    if( ( *( indexExpressions[i].functionPointer) )(pExpression) )
		      {
			returnVariant = indexExpressions[i].variant;
			if( bDebug >= normal_debug_level ) 
			  cout << endl << "Return Variant is "<< returnVariant << endl;
			break;
		      }
		  }
	      }

	    // we know it's a function call and can't identify it as one of our own
	    // so we'll return it as a user function 
	    if ( returnVariant == ROSE_UNKNOWN_GRAMMAR )
	      returnVariant = ROSE_C_ExpressionTag;

	    break;
	  }
	case BOOL_VAL: 
	case SHORT_VAL: 
	case CHAR_VAL: 
	case UNSIGNED_CHAR_VAL: 
	case WCHAR_VAL: 
	case STRING_VAL: 
	case UNSIGNED_SHORT_VAL: 
	case ENUM_VAL: 
	case INT_VAL:  
	case UNSIGNED_INT_VAL: 
	case LONG_INT_VAL: 
	case LONG_LONG_INT_VAL: 
	case UNSIGNED_LONG_LONG_INT_VAL: 
	case UNSIGNED_LONG_INT_VAL: 
	case FLOAT_VAL: 
	case DOUBLE_VAL: 
	case LONG_DOUBLE_VAL: 
	  {
	    returnVariant = ROSE_C_ExpressionTag;
	    break; 
	  }
	case FUNC_CALL: 
	  {
	    // just extract the rhs and see what it's variant is
	    SgExpression *pExpr = ((SgFunctionCallExp *) pExpression)->get_function();

         // Added by DQ to debug SAGE 3
            ROSE_ASSERT(pExpr != NULL);
	    returnVariant = getVariant(pExpr);
	    break; 
	  }
	case POINTST_OP:
	  {
	    // first we'll see if it is a dereferenced pointer expression
	    for ( int i = 0; i < n_arrayDereferencedVariables; i++ )
	      {
		if( ( *( arrayDereferencedVariables[i].functionPointer) )(pExpression) )
		  {
		    returnVariant = arrayDereferencedVariables[i].variant;
		    if( bDebug >= normal_debug_level ) 
		      cout << endl << "Return Variant is "<< returnVariant << endl;
		    break;
		  }
	      }
	    // if it's not a dereferenced pointer see what else it could be based on rhs
	    if ( returnVariant == ROSE_UNKNOWN_GRAMMAR )
	      {
		SgExpression *pExpr = ((SgArrowExp *) pExpression)->get_rhs_operand();
		returnVariant = getVariant(pExpr);	    
	      }
	    break;
	  }
	case RECORD_REF: 
	  {
	    SgExpression *pExpr = ((SgDotExp *) pExpression)->get_rhs_operand();
	    returnVariant = getVariant(pExpr);	    
	    break; 
	  }
	case DOTSTAR_OP:        // am not going to handle these 2 cases other than as C expressions
	case ARROWSTAR_OP: 
	case EQ_OP: 
	case LT_OP: 
	case GT_OP: 
	case NE_OP: 
	case LE_OP: 
	case GE_OP: 
	case ADD_OP: 
	case SUBT_OP: 
	case MULT_OP: 
	case DIV_OP: 
	case INTEGER_DIV_OP: 
	case MOD_OP: 
	case AND_OP: 
	case OR_OP: 
	case BITXOR_OP:
	case BITAND_OP: 
	case BITOR_OP: 
	case COMMA_OP: 
	case LSHIFT_OP: 
	case RSHIFT_OP: 
	case UNARY_MINUS_OP: 
	case UNARY_ADD_OP: 
	case SIZEOF_OP: 
	case TYPEID_OP: 
	case NOT_OP: 
	case DEREF_OP: 
	case ADDRESS_OP: 
	case MINUSMINUS_OP: 
	case PLUSPLUS_OP: 
#ifndef USE_SAGE3
	case ABSTRACT: 
#endif
	case BIT_COMPLEMENT_OP: 
	  {	  
	    returnVariant = ROSE_C_ExpressionTag;
	    break;
	  }
	case EXPR_CONDITIONAL: 
	  break; 
#ifndef USE_SAGE3
	case CLASSINIT_OP: 
	  break; 
	case DYNAMICCAST_OP: 
	  break; 
#endif
	case CAST_OP: 
	case ARRAY_OP: 
	case NEW_OP: 
	case DELETE_OP: 
	case THIS_NODE: 
	  {
	    returnVariant = ROSE_C_ExpressionTag;
	    break; 
	  }
	case SCOPE_OP: 
	  break; 
	case ASSIGN_OP: 
	case PLUS_ASSIGN_OP: 
	case MINUS_ASSIGN_OP: 
	case AND_ASSIGN_OP: 
	case IOR_ASSIGN_OP: 
	case MULT_ASSIGN_OP: 
	case DIV_ASSIGN_OP: 
	case MOD_ASSIGN_OP: 
	case XOR_ASSIGN_OP: 
	case LSHIFT_ASSIGN_OP: 
	case RSHIFT_ASSIGN_OP: 
	  {
	    returnVariant = ROSE_C_ExpressionTag;
	    break; 
	  }
#ifndef USE_SAGE3
	case FORDECL_OP: 
	  cerr << "FORDECL_OP not implemented" << endl; 
	  break; 
#endif
	case TYPE_REF: 
	  cerr << "TYPE_REF not implemented" << endl; 
	  break; 
#ifndef USE_SAGE3
	case VECTOR_CONST: 
	  cerr << "VECTOR_CONST not implemented" << endl; 
	  break; 
#endif
	case EXPR_INIT: 
	  cerr << "EXPR_INIT not implemented" << endl; 
	  break; 
	case AGGREGATE_INIT: 
	  cerr << "AGGREGATE_INIT not implemented" << endl; 
	  break; 
	case CONSTRUCTOR_INIT:  
	  { // have to add code here which goes into the constructor initializer to see what's there
	    // in this case we will access the argument list of the constructor, ensure
	    // that it has only one argument and that that is a function call exp, in which case we
	    // will continue to process it, we also ensure that it is a constructor of an array class
	    // we do this since we are only interested in getting a array class member operator, in
	    // particular we are searching for the paranthesis operator which gets hidden within
	    // a constructor initializer
	    SgConstructorInitializer* constructorInitializer = isSgConstructorInitializer( pExpression );
	    ROSE_ASSERT (constructorInitializer != NULL);
	    if (bDebug >= normal_debug_level)
	      showSgExpression( cout, constructorInitializer, "Constructor info.", 0);
	    // discovered that declarations are not usually available and so cannot 
	    // rely on discovering whether this is an array class constructor by name.
	    SgExprListExp *pArgList = constructorInitializer->get_args();
	    if (  pArgList != NULL)
	      {
		SgExpressionPtrList expressionPtrList = pArgList->get_expressions();
		if ( expressionPtrList.size() == 1 )
		  {
                 // SgExpressionPtrList::iterator i = expressionPtrList.begin();
#if USE_SAGE3
                 // Sage3 uses STL and this simplifies the syntax for using lists
		    SgMemberFunctionRefExp *pReturnExp = getArrayClassMemberOperator( *(expressionPtrList.begin()) );
#else
		    SgExpressionPtr expressionPtr = *(expressionPtrList.begin());
		    SgMemberFunctionRefExp *pReturnExp = getArrayClassMemberOperator( expressionPtr.operator->() );  
#endif
		    if( pReturnExp != NULL )
		      returnVariant = getVariant( pReturnExp );
		  }
	      }	    // in the argument list 
	    break; 
	  }
	case ASSIGN_INIT:
	  {
	    break;
	  }
	case THROW_OP:
	  {
	    returnVariant = ROSE_C_ExpressionTag;
	    break;
	  }
	default:
	  {
	    returnVariant = ROSE_UNKNOWN_GRAMMAR;
	    break;
	  }
	}
    }
  return returnVariant;
}


ROSE_GrammarVariants ArrayClassSageInterface::getVariant( SgStatement *pStatement)
{
  ROSE_GrammarVariants returnVariant = ROSE_UNKNOWN_GRAMMAR;

  if( pStatement != NULL ) 
    {
      for ( int i = 0; i < n_statementFunctions; i++ )
	{
	  if( ( *( statementFunctions[i].functionPointer) )( pStatement ) )
	    {
	      returnVariant = statementFunctions[i].variant;
	      if( bDebug >= normal_debug_level ) 
		cout << endl << "Return Variant is "<< returnVariant << endl;
	      break;
	    }
	}
      // we know it's got to be a C Statement atleast!
      if ( returnVariant == ROSE_UNKNOWN_GRAMMAR )
	returnVariant = ROSE_C_StatementTag;
    }
  return returnVariant;
}

ROSE_GrammarVariants ArrayClassSageInterface::getVariant( SgType *pType )
{
  ROSE_GrammarVariants returnVariant = ROSE_UNKNOWN_GRAMMAR;

  if( pType != NULL )
    {
      for ( int i = 0; i < n_typeFunctions; i++ )
	{
	  if( ( *( typeFunctions[i].functionPointer ) )( pType ) )
	    {
	      returnVariant = typeFunctions[i].variant;
	      if( bDebug >= normal_debug_level )
		cout << endl << "Return Variant is "<< returnVariant << endl;
	      break;
	    }
	}
    }
  return returnVariant;
}





