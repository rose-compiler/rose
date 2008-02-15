#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rose.h"

#include "ROSE_Transformations.h"

// include "transformation_3.h"

extern struct 
   { 
     ROSE_GrammarVariants variant; 
     char *name;
   } arrayGrammarTerminalNames[143];

#define FILE_LEVEL_COPY_DEBUG_VALUE 1

OperandDataBase::~OperandDataBase ()
   {
     numberOfDimensions = -1;
     variableName = "DESTRUCTOR_STRING";
   }

int
OperandDataBase::getStatementDimension()
   {
  // This function gets the dimension of the local operand if it can be determined from the 
  // number of operands in the indexing or the MAX_ARRAY_DIMENSION of the operand.
  // Later we will have the operands that are not indexed look at the local declaration if it
  // can be found; so that we can see the number of arguements that are used in its 
  // initialization (i.e. in the constructor).

     int localNumberOfDimensions = -1;

     ROSE_ASSERT (transformableOperand != NULL);
     ROSE_ASSERT (transformableOperand->getRoseSubTree() != NULL);
     if (transformableOperand->getRoseSubTree()->getVariant() == ROSE_C_ExpressionTag)
        {
       // We can't get the number of dimensions from a scalar ROSE_C_Expression
          localNumberOfDimensions = 0;
        }
       else
        {
          ROSE_ArrayOperandExpression* expr = (ROSE_ArrayOperandExpression*) transformableOperand->getRoseSubTree();
          ROSE_ASSERT (expr != NULL);

#if 1
          if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
               printf ("@@@@@@@@@@@@@@@ In OperandDataBase::getStatementDimension() arrayGrammarTerminalNames[%d].name = %s \n",
                    expr->getVariant(), arrayGrammarTerminalNames[expr->getVariant()].name);
#endif

          localNumberOfDimensions = expr->getNumberOfDimensions();
        }

     ROSE_ASSERT (localNumberOfDimensions >= 0);
     return localNumberOfDimensions;
   }


SgName
OperandDataBase::transformationVariableSuffix()
   {
  // This function builds the string used to represent the name of the pointer to the array data  
  // ROSE_ArrayOperandExpression* expr = (ROSE_ArrayOperandExpression*) transformableOperand->getRoseSubTree();
  // ROSE_ASSERT (expr != NULL);

     return ROSE_ArrayOperandExpression::transformationVariableSuffix();
   }

SgName OperandDataBase::getVariableNamePointer()
   {
  // This function builds the string used to represent the name of the pointer to the array data  
#if 1
     return ROSE::concatenate(getVariableName(),transformationVariableSuffix());
#else
     char* originalVariableName = new char[256];
     strcpy(originalVariableName,getVariableName());

     ROSE_ArrayOperandExpression* expr = (ROSE_ArrayOperandExpression*) transformableOperand->getRoseSubTree();
     strcat(originalVariableName,expr->transformationVariableSuffix());
     return originalVariableName;
#endif
   }

OperandDataBase::OperandDataBase ( ROSE_TransformableOperandExpression* transformableOperandExpression )
   {
     numberOfDimensions = 1;
     variableName = "default_name_from_OperandDataBase_constructor";
     transformableOperand = transformableOperandExpression;

  // error debugging variable
  // int ROSE_COPY_DEBUG = FILE_LEVEL_COPY_DEBUG_VALUE;
     const int ROSE_COPY_DEBUG = ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) ) ? TRUE : FALSE;

     char* notImplemented = "in constructor OperandDataBase(ROSE_TransformableOperandExpression*) NOT implemented";
     char* implemented    = "in constructor OperandDataBase(ROSE_TransformableOperandExpression*) IS implemented";

  // ROSE_Expression* expr = transformableOperand->getRoseExpression();
     ROSE_ArrayOperandExpression* expr = (ROSE_ArrayOperandExpression*) transformableOperand->getRoseSubTree();

#if 1
     if ( (ROSE_DEBUG > 0) || (ROSE::verbose == TRUE) )
          printf ("########## In constructor for OperandDataBase: arrayGrammarTerminalNames[%d].name = %s \n",
               expr->getVariant(), arrayGrammarTerminalNames[expr->getVariant()].name);
#endif

     switch (expr->getVariant())
        {
          case ROSE_C_ExpressionTag:
	     {
               if (ROSE_COPY_DEBUG > 0) cout << "ROSE_C_Expression " << notImplemented << endl;
               ROSE_C_Expression* rose_C_Expression = (ROSE_C_Expression*) expr;
            // No name if appropriate for a ROSE_C_Expression because there is no array variable
               variableName = "default_C_ExpressionName";
               break;
             }
          case ROSE_ArrayOperandExpressionTag: 
	     {
               if (ROSE_COPY_DEBUG > 0) cout << "ROSE_ArrayOperandExpressionTag " << notImplemented << endl;
               break;
             }
          case ROSE_ArrayVariableExpressionTag: 
	     {
               if (ROSE_COPY_DEBUG > 0) cout << "ROSE_ArrayVariableExpressionTag " << notImplemented << endl;
               break;
             }
          case ROSE_RealArrayVariableExpressionTag: 
	     {
               if (ROSE_COPY_DEBUG > 0) cout << "ROSE_RealArrayVariableExpressionTag " << notImplemented << endl;
               break;
             }
          case ROSE_doubleArrayVariableExpressionTag:
	     {
               if (ROSE_COPY_DEBUG > 0) cout << "ROSE_doubleArrayVariableExpressionTag " << notImplemented << endl;
               ROSE_doubleArrayVariableExpression* doubleArrayVariableExpression = 
                    (ROSE_doubleArrayVariableExpression*) expr;
               variableName = doubleArrayVariableExpression->getOperandName();
               break;
             }
          case ROSE_floatArrayVariableExpressionTag: 
	     {
               if (ROSE_COPY_DEBUG > 0) cout << "ROSE_floatArrayVariableExpressionTag " << notImplemented << endl;
               ROSE_floatArrayVariableExpression* floatArrayVariableExpression = 
                    (ROSE_floatArrayVariableExpression*) expr;
               variableName = floatArrayVariableExpression->getOperandName();
               break;
             }
          case ROSE_intArrayVariableExpressionTag: 
	     {
               if (ROSE_COPY_DEBUG > 0) cout << "ROSE_intArrayVariableExpressionTag " << notImplemented << endl;
               ROSE_intArrayVariableExpression* intArrayVariableExpression = 
                    (ROSE_intArrayVariableExpression*) expr;
               variableName = intArrayVariableExpression->getOperandName();
               break;
             }
          case ROSE_ArrayVariableReferenceExpressionTag: 
	     {
               if (ROSE_COPY_DEBUG > 0) cout << "ROSE_ArrayVariableReferenceExpressionTag " << notImplemented << endl;
               break;
             }
          case ROSE_RealArrayVariableReferenceExpressionTag: 
	     {
               if (ROSE_COPY_DEBUG > 0) cout << "ROSE_RealArrayVariableReferenceExpressionTag " << notImplemented << endl;
               break;
             }
          case ROSE_doubleArrayVariableReferenceExpressionTag: 
	     {
               if (ROSE_COPY_DEBUG > 0) cout << "ROSE_doubleArrayVariableReferenceExpressionTag " << notImplemented << endl;
               break;
             }
          case ROSE_floatArrayVariableReferenceExpressionTag: 
	     {
               if (ROSE_COPY_DEBUG > 0) cout << "ROSE_floatArrayVariableReferenceExpressionTag " << notImplemented << endl;
               break;
             }
          case ROSE_intArrayVariableReferenceExpressionTag: 
	     {
               if (ROSE_COPY_DEBUG > 0) cout << "ROSE_intArrayVariableReferenceExpressionTag " << notImplemented << endl;
               break;
             }
          case ROSE_ArrayVariablePointerExpressionTag: 
	     {
               if (ROSE_COPY_DEBUG > 0) cout << "ROSE_ArrayVariablePointerExpressionTag " << notImplemented << endl;
               break;
             }
          case ROSE_RealArrayVariablePointerExpressionTag: 
	     {
               if (ROSE_COPY_DEBUG > 0) cout << "ROSE_RealArrayVariablePointerExpressionTag " << notImplemented << endl;
               break;
             }
          case ROSE_doubleArrayVariablePointerExpressionTag: 
	     {
               if (ROSE_COPY_DEBUG > 0) cout << "ROSE_doubleArrayVariablePointerExpressionTag " << notImplemented << endl;
               break;
             }
          case ROSE_floatArrayVariablePointerExpressionTag: 
	     {
               if (ROSE_COPY_DEBUG > 0) cout << "ROSE_floatArrayVariablePointerExpressionTag " << notImplemented << endl;
               break;
             }
          case ROSE_intArrayVariablePointerExpressionTag: 
	     {
               if (ROSE_COPY_DEBUG > 0) cout << "ROSE_intArrayVariablePointerExpressionTag " << notImplemented << endl;
               break;
             }
          case ROSE_ArrayVariableDereferencedPointerExpressionTag: 
	     {
               if (ROSE_COPY_DEBUG > 0) cout << "ROSE_ArrayVariableDereferencedPointerExpressionTag " << notImplemented << endl;
               break;
             }
          case ROSE_RealArrayVariableDereferencedPointerExpressionTag: 
	     {
               if (ROSE_COPY_DEBUG > 0) cout << "ROSE_RealArrayVariableDereferencedPointerExpressionTag " << notImplemented << endl;
               break;
             }
          case ROSE_doubleArrayVariableDereferencedPointerExpressionTag: 
	     {
               if (ROSE_COPY_DEBUG > 0) cout << "ROSE_doubleArrayVariableDereferencedPointerExpressionTag " << notImplemented << endl;
               break;
             }
          case ROSE_floatArrayVariableDereferencedPointerExpressionTag: 
	     {
               if (ROSE_COPY_DEBUG > 0) cout << "ROSE_floatArrayVariableDereferencedPointerExpressionTag " << notImplemented << endl;
               break;
             }
          case ROSE_intArrayVariableDereferencedPointerExpressionTag: 
	     {
               if (ROSE_COPY_DEBUG > 0) cout << "ROSE_intArrayVariableDereferencedPointerExpressionTag " << notImplemented << endl;
               break;
             }
          case ROSE_ArrayParenthesisOperatorTag: 
	     {
               if (ROSE_COPY_DEBUG > 0) cout << "ROSE_ArrayParenthesisOperatorTag " << notImplemented << endl;
               ROSE_ArrayParenthesisOperator* arrayParenthesisOperator = 
                    (ROSE_ArrayParenthesisOperator*) expr;
               variableName = arrayParenthesisOperator->getOperandName();
               break;
             }
          default:
             cout << "ERROR: operandDataBase.C, constructor(ROSE_TransformableOperandExpression*)--default reached" << endl;
	     ROSE_ABORT();
        } // switch

   }

