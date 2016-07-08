// Treat config.h separately from other include files
#ifdef HAVE_CONFIG_H
// This avoids requiring the user to use config.h and follows 
// the automake manual request that we use <> instead of ""
#include <config.h>
#endif

#include "arrayPreprocessor.h"

using namespace rose;

// *****************************************************************
//      Interface function for Array Assignment Statement Query
// *****************************************************************

// ScalarIndexingStatementQuerySynthesizedAttributeType
SynthesizedAttributeBaseClassType
ScalarIndexingStatementTransformation::transformation
   ( const ArrayStatementQueryInheritedAttributeType & X, SgNode* astNode )
   {
  // This function returns the string representing the array statement transformation
     ROSE_ASSERT (isSgExprStatement(astNode) != NULL);

     ScalarIndexingStatementQueryInheritedAttributeType scalarIndexingStatementQueryInheritedData(X,astNode);

  // Declare the list to place transformation options into from each scope as we traverse backward
  // (up) the AST to the Global scope.
  // list<int> transformationOptions; // inherited attribute
     list<int> & transformationOptions = scalarIndexingStatementQueryInheritedData.getTransformationOptions();

     printf ("ScalarIndexingStatementQueryInheritedData.arrayStatementDimension = %d \n",
          scalarIndexingStatementQueryInheritedData.arrayStatementDimension);

  // We have to use a special function here instead of the Query mechanism since the Query mechanism
  // searches down through he AST (parents traverse their children) and we want to traverse up
  // through the AST (and not the whole AST) from the child to the parent stopping at the root of
  // the AST (the global scope: SgGlobal)
     printf ("############### CALLING TRANSFORMATION OPTION QUERY FROM ASSIGNMENT TRANSFORMATION ############ \n");

  // Find all the hints specified as enum values constructor parameters for declarations of
  // variables of type "TransformationAssertion".  The current scope and all parent scopes are
  // searched back to the global scope. (Maybe this should return a value rather than modify a
  // reference parameter???)
     TransformationSupport::getTransformationOptions ( astNode, transformationOptions, "TransformationAssertion");

#if 1
  // List the different transformation options that are specified in the user's code
  // list<TransformationAssertion::TransformationOption>::iterator transformationOptionListIterator;
     list<int>::iterator transformationOptionListIterator;
     for (transformationOptionListIterator  = transformationOptions.begin();
          transformationOptionListIterator != transformationOptions.end();
          transformationOptionListIterator++)
        {
       // display each value
       // TransformationAssertion::TransformationOption i = *transformationOptionListIterator;
          int i = *transformationOptionListIterator;
          printf ("     Value in transformation option = %d name = %s \n",i,TransformationAssertion::getOptionString(i));
        }
#endif

  // If there are any index object in use then we can't do the transformation (so make sure there are none)
     ROSE_ASSERT (NameQuery::getVariableNamesWithTypeNameQuery (astNode,"Internal_Index").size() == 0);

     printf ("######################### END OF INTERNALINDEX NAME QUERY ######################## \n");

     printf ("################# START OF ARRAY STATEMENT DIMENSION QUERY ################ \n");

  // The dimension of the array statement must be computed on the way down (in the traversal of the AST).
  // This query gets the list of integer associated with the dimension of each array operand (array
  // operands using the doubleArray::operator() member function).
     list<int> operandDimensionList =
          NumberQuery::getNumberOfArgumentsToParenthesisOperatorQuery (astNode, "doubleArray" );
     ROSE_ASSERT (operandDimensionList.size() >= 0);

#if 1
     printf ("operandDimensionList.size() = %d \n",operandDimensionList.size());
     printf ("operandDimensionList = \n%s\n",StringUtility::listToString(operandDimensionList).c_str());
#endif

  // Now use STL to build a list of unique names
     operandDimensionList.unique();
#if 0
     printf ("Unique Names: operandDimensionList = \n%s\n",StringUtility::listToString(operandDimensionList).c_str());
#endif

  // If there is no dimension computed for the query then it means that there were no operator()
  // used in which case we have to assume 6D array operations
     int dimensionOfArrayStatement = (operandDimensionList.size() == 0) ? 6 : *(operandDimensionList.begin());
     printf ("Array statement is %d dimensional \n",dimensionOfArrayStatement);

  // Make sure that it has the default value before we change it (error checking)
     printf ("ScalarIndexingStatementQueryInheritedData.arrayStatementDimension = %d \n",
          scalarIndexingStatementQueryInheritedData.arrayStatementDimension);
     ROSE_ASSERT (scalarIndexingStatementQueryInheritedData.arrayStatementDimension == -1);

  // Modify the inherited attribute using the array statement dimension data
     scalarIndexingStatementQueryInheritedData.arrayStatementDimensionDefined = TRUE;
     scalarIndexingStatementQueryInheritedData.arrayStatementDimension        = dimensionOfArrayStatement;

#if 0
     printf ("ScalarIndexingStatementQueryInheritedData.arrayStatementDimension = %d \n",
          scalarIndexingStatementQueryInheritedData.arrayStatementDimension);
     printf ("Exiting as part of testing (after search for array statement dimension) ... \n");
     ROSE_ABORT();
#endif

     ScalarIndexingStatementTransformation transformation;

  // Build a return value for this function
     ScalarIndexingStatementQuerySynthesizedAttributeType returnScalarIndexingStatementTransformation(astNode);

  // We need these until we have a new interface which will allow us to skip them (they are not used)
  // ScalarIndexingStatementQueryAccumulatorType        accumulatorValue;

  // WARNING: It is a design problem that we have the dimension set in two locations

  // To to implement this transformation withouth a database mechanism
  // Set the dimension of the array statement in the array operand database
     transformation.accumulatorValue.operandDataBase.setDimension(dimensionOfArrayStatement);
     ROSE_ASSERT (transformation.accumulatorValue.operandDataBase.getDimension() > 0);

  // Setup the data base with the options specified by the use and extracted from the current scope
  // of the application code.
     transformation.accumulatorValue.operandDataBase.setUserOptimizationAssertions
          (scalarIndexingStatementQueryInheritedData.transformationOptions);
     ROSE_ASSERT ( transformation.accumulatorValue.operandDataBase.transformationOption > 
                   ArrayTransformationSupport::UnknownIndexingAccess );

  // Make sure the data base has been setup properly
     ROSE_ASSERT ( transformation.accumulatorValue.operandDataBase.transformationOption >
          ArrayTransformationSupport::UnknownIndexingAccess );
     ROSE_ASSERT ( transformation.accumulatorValue.operandDataBase.dimension > -1 );

  // We must call this preorder because we modify the inherited attribute on the way down into the
  // AST so that the assembly will have the correct array statment dimension on the way back up (the
  // assembly of attributes always happens postorder).
     returnScalarIndexingStatementTransformation =
          transformation.traverse ( astNode, scalarIndexingStatementQueryInheritedData );

  // printf ("accumulatorValue.operandDataBase.displayString() = %s \n",accumulatorValue.operandDataBase.displayString().c_str());
  // ROSE_ASSERT (accumulatorValue.operandDataBase.size() > 0);

  // Copy the operand data base out of the accumulator attribute and into the ScalarIndexingStatementTransformation object
  // operandDataBase = transformation.accumulatorValue.operandDataBase;

  // printf ("transformation.accumulatorValue.operandDataBase.displayString() = %s \n",
  //      transformation.accumulatorValue.operandDataBase.displayString().c_str());

#if 0
  // Verify that we have saved the global and variable declarations and variable initializations
     ROSE_ASSERT (returnScalarIndexingStatementTransformation.globalDeclarationStrings.isEmpty()      == false);
     ROSE_ASSERT (returnScalarIndexingStatementTransformation.variableDeclarationStrings.isEmpty()    == false);
     ROSE_ASSERT (returnScalarIndexingStatementTransformation.variableInitializationStrings.isEmpty() == false);
#endif

  // Verify that we have a valid string
     ROSE_ASSERT (returnScalarIndexingStatementTransformation.isEmpty() == false);

     printf ("######################### END OF SCALAR INDEXING TRANSFORMATION QUERY ######################## \n");

     printf ("returnScalarIndexingStatementTransformation.getTransformationSourceCode().c_str() = \n%s \n",
          returnScalarIndexingStatementTransformation.getWorkSpace().c_str());

#if 0
  // NOTE: I think we need to get the variable declarations out of returnArrayStatementTransformation
     list<string> stringList = returnArrayStatementTransformation.getVariableDeclarationStrings();
     ROSE_ASSERT (stringList.size() > 0);
     for (list<string>::iterator i = stringList.begin(); i != stringList.end(); i++)
          printf ("Base of ScalarIndexingStatementTransformation::transformation(): getVariableDeclarationStrings() = %s \n",(*i).c_str());
#endif

#if 0
     printf ("Exiting as part of testing (after SCALAR INDEXING TRANSFORMATION QUERY) ... \n");
     ROSE_ABORT();
#endif

     return returnScalarIndexingStatementTransformation;
   }

// ********************************************************************************************
//                             Destructors and Constructors
// ********************************************************************************************

ScalarIndexingStatementTransformation::~ScalarIndexingStatementTransformation()
   {
  // Nothing to do here!
     delete &accumulatorValue;
   }

ScalarIndexingStatementTransformation::ScalarIndexingStatementTransformation()
   :
  // Initialize the accumulator attribute
     accumulatorValue( *(new ScalarIndexingStatementQueryAccumulatorType()) ),
  // Initialize to false
     firstOperandAlreadyProcessed(false)
   {
  // Nothing to do here!
     printf ("Inside of body of constructor ScalarIndexingStatementTransformation \n");
   }

#if 0
ScalarIndexingStatementTransformation::ScalarIndexingStatementTransformation()
   : // Initialize Base class
        AST_Rewrite::RewriteTreeTraversal<
             ScalarIndexingStatementQueryInheritedAttributeType,
             ScalarIndexingStatementQuerySynthesizedAttributeType>( *(new SgProject()) ),
     // Initialize the accumulator attribute
     accumulatorValue( *(new ScalarIndexingStatementQueryAccumulatorType()) )
   {
  // Nothing to do here!
     printf ("Inside of body of constructor ScalarIndexingStatementTransformation \n");
   }
#endif

bool
ScalarIndexingStatementTransformation::targetForTransformation ( SgNode* astNode )
   {
     ROSE_ASSERT (astNode != NULL);

  // If we are to apply the scalar indexing transformation then we need only check for use of the
  // operator() taking an integer.
     bool returnValue = false;

  // This query only returns function names as strings so it is not sufficient for distigishing 
  // between the operaotr(Index) and operator(int). First we get this working.
     printf ("Generating a list of function expressions in ScalarIndexingStatementTransformation::targetForTransformation(SgNode*) \n");
     list<NameQuery::FunctionReferenceNameType> functionList = NameQuery::getFunctionReferenceNamesQuery(astNode);

     if (functionList.size() > 0)
        {
       // search for the string "operator()"
          printf ("We have a valid nonzero length list of function names, no search for \"operator()\", not implemented! \n");

          list<NameQuery::FunctionReferenceNameType>::iterator i;
          for (i = functionList.begin(); i != functionList.end(); i++)
             {
               string functionName = (*i).getFunctionName();
               if (functionName == "operator()")
                  {
                 // Need to look at the argument types (looking for integer arguements)
                    list<string> argumentTypeNameList = (*i).getArgumentTypeNames();
                    list<string>::iterator j;
                    bool isScalarIndexingOperator = (functionList.size() > 0) ? true : false;
                    for (j = argumentTypeNameList.begin(); j != argumentTypeNameList.end(); j++)
                       {
                         string argumentTypeName = *j;
                         if (argumentTypeName != "int")
                              isScalarIndexingOperator = false;
                       }

                    if (isScalarIndexingOperator == true)
                       {
                      // check return type
                         string returnTypeName = (*i).getReturnTypeName();
                         if ( returnTypeName == "double &" )
                            {
                           // Found a scalar indexing operator
                              returnValue = true;
                            }
                       }
                  }
             }
        }

//  ROSE_ASSERT (functionList.size() > 0);

     printf ("ScalarIndexingStatementTransformation::targetForTransformation(): returning %s \n",
          (returnValue) ? "true" : "false");

//   ROSE_ABORT();

     return returnValue;
   }











