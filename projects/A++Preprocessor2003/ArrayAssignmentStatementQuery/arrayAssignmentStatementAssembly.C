// Treat config.h separately from other include files
#ifdef HAVE_CONFIG_H
// This avoids requiring the user to use config.h and follows 
// the automake manual request that we use <> instead of ""
#include <config.h>
#endif

#include "arrayPreprocessor.h"

// ********************************************************************************************
// Support for Query to build string representing the transformation of an expression statement
// ********************************************************************************************

// Rename this function to: localArrayAssignmentStatementQueryFunction
ArrayAssignmentStatementQueryInheritedAttributeType
ArrayAssignmentStatementTransformation::
evaluateRewriteInheritedAttribute (
   SgNode* astNode,
   ArrayAssignmentStatementQueryInheritedAttributeType arrayAssignmentStatementQueryInheritedData )
   {
  // This function returns a string specific to the current astNode (assembly of strings is a separate function)

     ROSE_ASSERT (astNode != NULL);

#if 0
     printf ("##### TOP of evaluateRewriteInheritedAttribute() (sage_class_name = %s) \n",astNode->sage_class_name());
#endif

  // Build the inherited attribute (using the source position information (required for use of
  // rewrite mechanism)).
     ArrayAssignmentStatementQueryInheritedAttributeType
          returnAttribute(arrayAssignmentStatementQueryInheritedData,astNode);

#if 0
     returnAttribute.display("Called from ArrayAssignmentStatementTransformation::evaluateInheritedAttribute()");
#endif

  // Make sure the data base has been setup properly
     ROSE_ASSERT ( accumulatorValue.operandDataBase.transformationOption > ArrayTransformationSupport::UnknownIndexingAccess );
     ROSE_ASSERT ( accumulatorValue.operandDataBase.dimension > -1 );

  // Default value for synthesized attribute we will use as a return value
  // Need to handle all unary and binary operators and variables (but not much else)
     switch (astNode->variantT())
        {
          case V_SgFunctionCallExp: // tag for SgFunctionCallExp class
             {
            // Error checking: Verify that we have a SgFunctionCallExp object
               SgFunctionCallExp* functionCallExpression = isSgFunctionCallExp(astNode);
               ROSE_ASSERT (functionCallExpression != NULL);

               string operatorName = TransformationSupport::getFunctionName ( functionCallExpression );
               ROSE_ASSERT (operatorName.length() > 0);

            // printf ("overloaded operator is of type = %s \n",functionTypeName.c_str());

               string functionTypeName = TransformationSupport::getFunctionTypeName(functionCallExpression);

               if ( (functionTypeName != "doubleArray") && 
                    (functionTypeName != "floatArray")  &&
                    (functionTypeName != "intArray") )
                  {
                 // Use this query to handle only A++ function call expressions
                 // printf ("Break out of overloaded operator processing since type = %s is not to be processed \n",functionTypeName.c_str());
                    break;
                  }
                 else
                  {
                 // printf ("Processing overloaded operator of type = %s \n",functionTypeName.c_str());
                  }

               ROSE_ASSERT ( (functionTypeName == "doubleArray") || 
                             (functionTypeName ==  "floatArray") ||
                             (functionTypeName ==    "intArray") );

            // printf ("CASE FUNC_CALL: Overloaded operator = %s \n",operatorName);

               TransformationSupport::operatorCodeType operatorCodeVariant =
                    TransformationSupport::classifyOverloadedOperator (operatorName.c_str());
            // printf ("CASE FUNC_CALL: operatorCodeVariant = %d \n",operatorCodeVariant);

               ROSE_ASSERT (operatorName.length() > 0);

            // Moved from local generation function to assembly function

            // Separating this case into additional cases makes up to some 
            // extent for using a more specific higher level grammar.
               switch (operatorCodeVariant)
                  {
                    case TransformationSupport::PARENTHESIS_OPERATOR_CODE:
                       {
                         ROSE_ASSERT (operatorName == "operator()");

                      // Record that the array operand is index with InternalIndex objects this info
                      // is used later in the assembly phase to defer the specification of return
                      // string.
                         arrayAssignmentStatementQueryInheritedData.setIsIndexedArrayOperand(TRUE);
#if 0
                         printf ("Exiting after finding FUNC_CALL operator() local attribute generator ... \n");
                         ROSE_ABORT();
#endif
                         break;
                       }

                    default:
                       {
                      // printf ("default in switch found in variant = %d \n",operatorCodeVariant);
                       }
                  }
             }

          default:
             {
#if 0
               printf ("DEFAULT LOCAL ATTRIBUTE CREATED: default case in evaluateRewriteInheritedAttribute() (sage_class_name = %s \n",
                    astNode->sage_class_name());
#endif
               break;
             }
        }


#if 0
     printf ("##### BOTTOM of evaluateRewriteInheritedAttribute (astNode = %s) \n",astNode->sage_class_name());
#endif

  // arrayAssignmentStatementQueryInheritedData.display("BOTTOM of localArrayAssignmentStatementQueryFunction");

  // return arrayAssignmentStatementQueryInheritedData;
     return returnAttribute;
   }


// Supporting function (not one of the global functions required for the Query interface)
ArrayAssignmentStatementQuerySynthesizedAttributeType
expressionStatementTransformation (
   SgExprStatement* astNode,
   const ArrayAssignmentStatementQueryInheritedAttributeType   & arrayAssignmentStatementQueryInheritedData,
   const ArrayAssignmentStatementQuerySynthesizedAttributeType & innerLoopTransformation,
   OperandDataBaseType                                         & operandDataBase )
   {
  // Since we input the operandDataBase we could skip the initial AST queries and query the data base instead.
  // (the operandDataBase has more information than the inital queries represent).

  // declaraton of return value
     ArrayAssignmentStatementQuerySynthesizedAttributeType returnSynthesizedAttribute(astNode);

     string returnString;

  // printf ("In expressionStatementTransformation(): operandDataBase.displayString() = %s \n",operandDataBase.displayString().c_str());

  // Make sure the data base has been setup properly
     ROSE_ASSERT ( operandDataBase.transformationOption > ArrayTransformationSupport::UnknownIndexingAccess );
     ROSE_ASSERT ( operandDataBase.dimension > -1 );

#if 0
     printf ("##### In expressionStatementTransformation at sage_class_name() = %s ##### \n",
          astNode->sage_class_name());
  // printf ("innerLoopTransformation.getTransformationSourceCode() = %s \n",
  //      innerLoopTransformation.getTransformation().getWorkSpace().c_str());
#endif

  // Verify that we have a valid string (but only in the workspace)
     ROSE_ASSERT (innerLoopTransformation.isEmpty() == true);
     ROSE_ASSERT (innerLoopTransformation.getWorkSpace().c_str() != NULL);
     ROSE_ASSERT (innerLoopTransformation.getWorkSpace() != "");

  // printf ("######################### START OF VARIABLE TYPE NAME QUERY ######################## \n");

     list<string> operandNameStringList = NameQuery::getVariableNamesWithTypeNameQuery (astNode, "doubleArray" );

  // printf ("Print out the list of variable names: \n%s \n",StringUtility::listToString(operandNameStringList).c_str());

  // Now use STL to build a list of unique names
     list<string> uniqueOperandNameStringList = operandNameStringList;
     uniqueOperandNameStringList.unique();

  // printf ("Print out the list of unique variable names: \n%s \n",StringUtility::listToString(uniqueOperandNameStringList).c_str());

  // printf ("######################### START OF VARIABLE NAME QUERY ######################## \n");

  // Specify optimization assertions defined by user (specified by the user in his application code)
     operandDataBase.setUserOptimizationAssertions(arrayAssignmentStatementQueryInheritedData.transformationOptions);

  // This query gets the indexing code for each operand used in the expression statement
  // (but for now we make them all the same, later we will support the optimization of indexing of individual array operands)
  // Query the operand data base to find out the global properties of the indexing in the array statement
  // int globalIndexingProperties = ArrayTransformationSupport::UniformSizeUniformStride;
  // int globalIndexingProperties = operandDataBase.globalIndexingProperties();

  // printf ("globalIndexingProperties = %d \n",globalIndexingProperties);

#if 0
  // Testing Code Only
  // build the operandIndexingCodeList to have the same number of elements as in the operandNameStringList
     list<int> operandIndexingCodeList;
     list<string>::iterator listStringElementIterator;
     for (listStringElementIterator = operandNameStringList.begin(); 
          listStringElementIterator != operandNameStringList.end();
          listStringElementIterator++)
        {
       // assume 0 as a value for the indexCode (for testing only)
       // int testingValueForIndexCode = ArrayTransformationSupport::UniformSizeUniformStride;
          int testingValueForIndexCode = globalIndexingProperties;
          operandIndexingCodeList.push_back(testingValueForIndexCode);
        }

     ROSE_ASSERT (operandIndexingCodeList.size() >= 0);

  // printf ("Print out the indexing code for each operand: \n%s \n",StringUtility::listToString(operandIndexingCodeList).c_str());

  // Now use STL to build a list of unique codes (cooresponding to the operands)
     list<int> uniqueOperandIndexingCodeList = operandIndexingCodeList;
     uniqueOperandIndexingCodeList.unique();

  // printf ("Print out the unique indexing codes over all operands: \n%s \n",StringUtility::listToString(uniqueOperandIndexingCodeList).c_str());

  // for now assume that all operands have the same indexing
     bool sameIndexingForAllOperands = TRUE;
     int operandIndexingCode         = 0x00000000;    // assume code is 0 for each operand
#endif

#if 0
     printf ("Exiting as part of testing (after search for operand indexing codes) ... \n");
     ROSE_ABORT();
#endif

  // printf ("######################### END OF ALL PRELIMINARY QUERIES ######################## \n");


  // Template string into which we substitute the variableData and loopNestString strings
     string transformationTemplateString =
"\
// Automatically Introduced Transformation (via preprocessor built by ROSE)\n\
$LOOP_NEST \n\
\n";

  // copy the template to the return string (and then edit the return string
     returnString = transformationTemplateString;

  // #############################
  //     COMPUTE THE LOOP NEST
  // #############################

  // Get the dimensionality of the array statement
     ROSE_ASSERT (arrayAssignmentStatementQueryInheritedData.arrayStatementDimensionDefined == TRUE);
     int dimensionOfArrayStatement = arrayAssignmentStatementQueryInheritedData.arrayStatementDimension;

  // call function to build loop nest
     string loopNestString =
          ArrayTransformationSupport::buildLoopNest ( operandDataBase, dimensionOfArrayStatement );

     if (innerLoopTransformation.getLoopDependence() == TRUE)
        {
       // If a loop dependence was found in the assembly then generate a special form of the
       // transformation to preserve the array semantics. This can be optimized later to block the
       // the computation so that we avoid poor use of cache.  Add an additional loop nest to the
       // loopNestString so that we can transform A = A+A into:
       //      _T.redim(lhs); for (...) {_T = A+A;} for (...) {A = _T};

       // Insert "T.redim(<lhs>);" at front of loopNestString
          string lhsArrayOperandName = operandDataBase.arrayOperandList[0].arrayVariableName;
          loopNestString = string("_T.redim(") + lhsArrayOperandName + string(");\n") + loopNestString;

          string temporaryString = innerLoopTransformation.getLoopDependenceLhs();
       // Use the same indexing with the temporary as is used with the lhs (copy lhs and change the
       // variable name (add to data base so that we generate the correct supporting declarations
       // and initializations)). Since _T is the same size as A we don't have to change the
       // subscript macro.
          temporaryString = SgNode::copyEdit (temporaryString,lhsArrayOperandName,"_T");

       // ***************************************************
       // Setup loop bodies for loop dependent transformation
       // ***************************************************

       // First loop body
          string firstLoopBody = innerLoopTransformation.getLoopDependenceRhs();
          firstLoopBody = temporaryString + firstLoopBody;
          firstLoopBody = SgNode::copyEdit (firstLoopBody,"$ASSIGNMENT_OPERATOR"," = ");

       // Second loop body
          string secondLoopBody = innerLoopTransformation.getLoopDependenceLhs();
          secondLoopBody        = secondLoopBody + string(" = ") + temporaryString;

       // Mark the first "$INNER_LOOP" substring so that we can differentiate between the first and second loops
          loopNestString  = SgNode::copyEdit (loopNestString,"$INNER_LOOP","$FIRST_INNER_LOOP");
          loopNestString += ArrayTransformationSupport::buildLoopNest ( operandDataBase, dimensionOfArrayStatement );
          loopNestString  = SgNode::copyEdit (loopNestString,"$INNER_LOOP","$SECOND_INNER_LOOP");

          loopNestString  = SgNode::copyEdit (loopNestString,"$FIRST_INNER_LOOP",firstLoopBody);
          loopNestString  = SgNode::copyEdit (loopNestString,"$SECOND_INNER_LOOP",secondLoopBody);

#if 0
          printf ("firstLoopBody  = %s \n",firstLoopBody.c_str());
          printf ("secondLoopBody = %s \n",secondLoopBody.c_str());
          printf ("loopNestString = \n%s \n",loopNestString.c_str());
#endif

       // Add "T" to operand data base for loop dependent array statement
       // operandDataBase.setVariableName( "_T" );
          ArrayOperandDataBase temporaryArrayOperand = operandDataBase.arrayOperandList[0];
          temporaryArrayOperand.arrayVariableName = "_T";
          operandDataBase.arrayOperandList.push_back(temporaryArrayOperand);

       // *******************************************************
       // add declaration for T (the the local and global scopes)
       // *******************************************************

       // This effects the declaration which appears in the global scop of the final generated code
       // (preprocessor output).
//        returnSynthesizedAttribute.variableDeclarationStrings.addSourceCodeString("doubleArray _T;");

       // This effects the declaration of _T for the code that generates the AST fragement that we
       // insert.  We have to insert this into the global declarations since the mechanism for
       // automatically adding global declarations does not see a reference to _T because it only
       // looks at the original array statement which does not include _T.
//        returnSynthesizedAttribute.globalDeclarationStrings.addSourceCodeString("doubleArray _T;");

#if 0
          printf ("All global declarations = \n%s\n",
               StringUtility::listToString(returnSynthesizedAttribute.getGlobalDeclarationStrings()).c_str());
          printf ("All variable declarations = \n%s\n",
               StringUtility::listToString(returnSynthesizedAttribute.getVariableDeclarationStrings()).c_str());
          printf ("All variable initializations = \n%s\n",
               StringUtility::listToString(returnSynthesizedAttribute.getVariableInitializationStrings()).c_str());
#endif

#if 0
          printf ("Exiting as part of support for loop dependence int expressionStatementTransformation() ... \n");
          ROSE_ABORT();
#endif
        }
       else
        {
       // This is case when no loop dependence is detected at compile time
       // Substitute the new transformation into the final returnString in place of "$INNER_LOOP"
       // string tempString = innerLoopTransformation.transformationStrings.getWorkSpace();
          string tempString = innerLoopTransformation.getWorkSpace();

          loopNestString = SgNode::copyEdit (loopNestString,"$INNER_LOOP",tempString);
        }

  // printf ("Before generation of global declarations: loopNestString = \n%s\n",loopNestString.c_str());

  // #######################################
  //     COMPUTE THE PREAMPLE DECLARATIONS
  // #######################################

  // A string to hold the #include directives required for the compilation 
  // of the intermediate file used to generate the AST fragments.
  // This prefix string is inserted ahead of global declarations and is 
  // specific to the A++P++Preprocessor.  simpleA++.h is a header file that
  // is similar to A++.h but containing only the public interface.  At some 
  // point a preprocessor could be used to automatically generate such a file.

  // We don't have to include "#include \"transformationMacros.h\" " in the
  // string below since it is included in the global declarations string
     string preambleDeclarations = "\
// Include simplified A++.h header file for intermediate generation of AST fragments \n\
#include \"simpleA++.h\" \n\n\
// Defining ROSE_MACROS_APPEAR_AS_FUNCTIONS forces macros to be defined in transformationMacros.h as \n\n\
// functions so that they will not be expanded except in the final placement in the application code. \n\n\
#define ROSE_MACROS_APPEAR_AS_FUNCTIONS \n\n\
// It is best to close off any string representing a transformation which includes \n\
// comments of preprocessor directives with a C++ declaration statement (avoids \n\
// comments from being attached to internal declaration markers \n\
int provideDeclarationForAttachmentOfPreprocessorDirectivesAndComments; \n\n\
";

  // printf ("preambleDeclarations = \n%s \n",preambleDeclarations.c_str());

#if 0
     AST_Rewrite::AST_FragmentString preambleFragmentString =
          AST_Rewrite::AST_FragmentString(preambleDeclarations,
                                          arrayAssignmentStatementQueryInheritedData,
                                          AST_Rewrite::Preamble,
                                       // AST_Rewrite::BeforeCurrentPosition);
                                       // AST_Rewrite::TopOfScope);
                                          AST_Rewrite::PreamblePositionInScope);
     returnSynthesizedAttribute.addSourceCodeString(preambleFragmentString);
#else
     returnSynthesizedAttribute.insert(astNode,
                                       preambleDeclarations,
                                       ArrayAssignmentStatementQuerySynthesizedAttributeType::Preamble,
                                       ArrayAssignmentStatementQuerySynthesizedAttributeType::PreamblePositionInScope);
#endif

#if 0
     printf ("Exiting as part of testing (after computation of Preamble string) ... \n");
     ROSE_ABORT();
#endif

  // #######################################
  //     COMPUTE THE GLOBAL DECLARATIONS
  // #######################################

  // A string to hold the variables required for this transformation
  // Build the variable data into the array statment transformation
     string globalDeclarations =
          ArrayTransformationSupport::buildOperandSpecificGlobalDeclarations
             ( arrayAssignmentStatementQueryInheritedData, operandDataBase );

  // printf ("globalDeclarations = \n%s \n",globalDeclarations.c_str());

#if 0
     AST_Rewrite::AST_FragmentString globalDeclarationFragmentString =
          AST_Rewrite::AST_FragmentString(globalDeclarations,
                                          arrayAssignmentStatementQueryInheritedData,
                                          AST_Rewrite::GlobalScope,
                                          AST_Rewrite::BeforeCurrentPosition);
     returnSynthesizedAttribute.addSourceCodeString(globalDeclarationFragmentString);
#else
     returnSynthesizedAttribute.insert(astNode,
                                       globalDeclarations,
                                       ArrayAssignmentStatementQuerySynthesizedAttributeType::GlobalScope,
                                       ArrayAssignmentStatementQuerySynthesizedAttributeType::BeforeCurrentPosition);
#endif

#if 0
     printf ("Exiting as part of testing (after computation of globalDeclaration string) ... \n");
     ROSE_ABORT();
#endif

  // ###############################################
  //     COMPUTE THE LOCAL VARIABLE DECLARATIONS
  // ###############################################

  // A string to hold the variables required for this transformation
  // Build the variable data into the array statment transformation
     string variableDeclarations =
          ArrayTransformationSupport::buildOperandSpecificVariableDeclarations
             ( arrayAssignmentStatementQueryInheritedData, operandDataBase );

  // printf ("variableDeclarations = \n%s \n",variableDeclarations.c_str());

#if 0
     AST_Rewrite::AST_FragmentString variableDeclarationFragmentString =
          AST_Rewrite::AST_FragmentString(variableDeclarations,
                                          arrayAssignmentStatementQueryInheritedData,
                                          AST_Rewrite::LocalScope,
                                          AST_Rewrite::TopOfScope);
     returnSynthesizedAttribute.addSourceCodeString(variableDeclarationFragmentString);
#else
     returnSynthesizedAttribute.insert(astNode,
                                       variableDeclarations,
                                       ArrayAssignmentStatementQuerySynthesizedAttributeType::LocalScope,
                                       ArrayAssignmentStatementQuerySynthesizedAttributeType::TopOfScope);
#endif

#if 0
     printf ("Exiting as part of testing (after computation of variableDeclaration string) ... \n");
     ROSE_ABORT();
#endif

  // ##################################################
  //     COMPUTE THE LOCAL VARIABLE INITIALIZATIONS
  // ##################################################

  // Variable initialization must occur imediately before the transformations using the variables
     string variableInitialization;
     variableInitialization = ArrayTransformationSupport::buildOperandSpecificVariableInitialization
                       ( arrayAssignmentStatementQueryInheritedData, operandDataBase);

#if 0
     AST_Rewrite::AST_FragmentString variableInitializationFragmentString =
          AST_Rewrite::AST_FragmentString(variableInitialization,
                                          arrayAssignmentStatementQueryInheritedData,
                                          AST_Rewrite::LocalScope,
                                          AST_Rewrite::BeforeCurrentPosition);
     returnSynthesizedAttribute.addSourceCodeString(variableInitializationFragmentString);
#else
     returnSynthesizedAttribute.insert(astNode,
                                       variableInitialization,
                                       ArrayAssignmentStatementQuerySynthesizedAttributeType::LocalScope,
                                       ArrayAssignmentStatementQuerySynthesizedAttributeType::BeforeCurrentPosition);
#endif

#if 0
     printf ("Exiting as part of testing (after computation of variableInitialization string) ... \n");
     ROSE_ABORT();
#endif

  // ############################################################
  // Assemble computed strings into place within the returnString
  // ############################################################

  // Edit into place the substring containing the declaration of all pointers and operands specific
  // subscript variables
     returnString = SgNode::copyEdit (returnString,"$VARIABLE_DECLARATIONS",variableDeclarations);
     ROSE_ASSERT (returnString.c_str() != NULL);

     returnString = SgNode::copyEdit (returnString,"$VARIABLE_INITIALIZATION",variableInitialization);
     ROSE_ASSERT (returnString.c_str() != NULL);

  // Substitute the new loop nest into the final returnString in place of "$LOOP_NEST"
     returnString = SgNode::copyEdit (returnString,"$LOOP_NEST",loopNestString);

  // printf ("returnString = \n%s \n",returnString.c_str());

#if 0
     AST_Rewrite::AST_FragmentString mainTransformationFragmentString =
          AST_Rewrite::AST_FragmentString(returnString,
                                          arrayAssignmentStatementQueryInheritedData,
                                          AST_Rewrite::LocalScope,
                                          AST_Rewrite::ReplaceCurrentPosition);
     returnSynthesizedAttribute.addSourceCodeString(mainTransformationFragmentString);
#else
     returnSynthesizedAttribute.insert(astNode,
                                       returnString,
                                       ArrayAssignmentStatementQuerySynthesizedAttributeType::LocalScope,
                                       ArrayAssignmentStatementQuerySynthesizedAttributeType::ReplaceCurrentPosition);
#endif

  // Reset the workspace to "" in the synthesized attribute (part of general cleanup, but not required)
     returnSynthesizedAttribute.setWorkSpace("");

#if 0
  // printf ("returnString = \n%s \n",returnString.c_str());
     printf ("Exiting as part of testing (at base of expressionStatementTransformation) ... \n");
     ROSE_ABORT();
#endif

     return returnSynthesizedAttribute;
   }


// Rename this function to: arrayAssignmentStatementQueryAssemblyFunction
ArrayAssignmentStatementQuerySynthesizedAttributeType
ArrayAssignmentStatementTransformation::
evaluateRewriteSynthesizedAttribute (
   SgNode* astNode,
   ArrayAssignmentStatementQueryInheritedAttributeType arrayAssignmentStatementQueryInheritedData,
   SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
  // This function assembles the elements of the input list (a list of char*) to form the output (a single char*)

#if 0
     printf ("\n$$$$$ TOP of evaluateRewriteSynthesizedAttribute (astNode = %s) (synthesizedAttributeList.size() = %d) \n",
          astNode->sage_class_name(),synthesizedAttributeList.size());
#endif

  // Build the return value for this function
     ArrayAssignmentStatementQuerySynthesizedAttributeType returnSynthesizedAttribute(astNode);

  // This is done in the base class
  // Store the astNode pointer in the synthesized attribute
  // returnSynthesizedAttribute.associated_AST_Node = astNode;

  // Iterator used within several error checking loops (not sure we should declare it here!)
     vector<ArrayAssignmentStatementQuerySynthesizedAttributeType>::iterator i;

  // Make a reference to the global operand database
     OperandDataBaseType & operandDataBase = accumulatorValue.operandDataBase;

  // Make sure the data base has been setup properly
     ROSE_ASSERT ( operandDataBase.transformationOption > ArrayTransformationSupport::UnknownIndexingAccess );
     ROSE_ASSERT ( operandDataBase.dimension > -1 );

  // Build up a return string
     string returnString;

  // Default setting for offsetString
  // string offsetString = "index1,index2,index3,index4,index5,index6";
     string offsetString = "_1,_2,_3,_4,_5,_6";

  // Figure out the dimensionality of the statement globally
     int maxNumberOfIndexOffsets = 6; // default value for A++/P++ arrays
     ROSE_ASSERT (arrayAssignmentStatementQueryInheritedData.arrayStatementDimensionDefined == TRUE);
     if (arrayAssignmentStatementQueryInheritedData.arrayStatementDimensionDefined == TRUE)
        {
       // The the globally computed array dimension from the arrayAssignmentStatementQueryInheritedData
          maxNumberOfIndexOffsets = arrayAssignmentStatementQueryInheritedData.arrayStatementDimension;

       // reset the offsetString since the maxNumberOfIndexOffsets is less than the A++/P++ MAX_ARRAY_DIMENSION
          offsetString = "";
          int n = 0;
          char countString[128];
          for (n = 0; n < maxNumberOfIndexOffsets; n++)
             {
            // Add the different expressions represented by the indexing strings together
               if ( n > 0 )
                    offsetString += ",";
               sprintf (countString,"%d",n+1);
            // offsetString += "index" + string(countString);
               offsetString += "_" + string(countString);
             }
        }
       else
        {
       // Then we want the minimum of all the dimensions accesses (or is it the maximum?)
        }

  // printf ("offsetString = %s \n",offsetString.c_str());

  // printf ("Exiting asa test at the top of arrayAssignmentStatementQueryAssemblyFunction \n");
  // ROSE_ABORT();

#if 0
  // Print out the elements of the list
     printf ("############################################################# \n");
     printf ("              Print out synthesizedAttributeList              \n");
     printf ("############################################################# \n");
     
     int counter = 0;
     for (i = synthesizedAttributeList.begin(); i != synthesizedAttributeList.end(); i++)
        {
          ROSE_ASSERT ((*i).associated_AST_Node != NULL);
          printf ("Element %d of SynthesizedAttributeListElements: variant = %d (%s) \nworkspace = %s \n",
               counter,(*i).associated_AST_Node->variantT(),(*i).associated_AST_Node->sage_class_name(),(*i).getWorkSpace().c_str());
          (*i).display("Called from ArrayAssignmentStatementTransformation::evaluateSynthesizedAttribute()");
          counter++;
        }
  // printf ("operandDataBase.displayString() = %s \n",operandDataBase.displayString().c_str());
#endif

     string operatorString;

  // Need to handle all unary and binary operators and variables (but not much else)
     switch (astNode->variant())
        {
       // Need to handle prefix and postfix unary operators
          case UNARY_MINUS_OP:
          case UNARY_ADD_OP:
          case NOT_OP:
          case DEREF_OP:
          case ADDRESS_OP:
          case MINUSMINUS_OP:
          case PLUSPLUS_OP:
          case UNARY_EXPRESSION:
             {
            // printf ("Found a UNARY expression (not implemented!) \n");
               SgUnaryOp*   unaryOperator  = isSgUnaryOp(astNode);
               ROSE_ASSERT (unaryOperator != NULL);

               returnString = " $UNARY_OP$ ";

               operatorString = TransformationSupport::buildOperatorString (astNode);

            // This is example of where it is useful to have enums generated from non-terminals within ROSETTA
               string operandString = synthesizedAttributeList[SgUnaryOp_operand_i].getWorkSpace();

            // This does not handle prefix and postfix operators properly
            // sprintf (returnString,"%s %s",operatorString,operandString);
               returnString = operatorString + operandString;
               break;
             }

       // Handle binary operators
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
          case EQ_OP:
          case LT_OP:
          case GT_OP:
          case NE_OP:
          case LE_OP:
          case GE_OP:
          case ASSIGN_OP:
          case BINARY_EXPRESSION:
             {
            // printf ("Found a BINARY OPERATOR expression \n");

               operatorString = TransformationSupport::buildOperatorString (astNode);

            // Handle the special assembly of binary operators
               SgBinaryOp*  binaryOperator = isSgBinaryOp(astNode);
               ROSE_ASSERT (binaryOperator != NULL);

               returnString = synthesizedAttributeList[SgBinaryOp_lhs_operand_i].getWorkSpace() +
                              operatorString +
                              synthesizedAttributeList[SgBinaryOp_rhs_operand_i].getWorkSpace();
               break;
             }

          case EXPR_STMT:
             {
            // The assembly associated with the SgExprStatement is what 
            // triggers the generation of the transformation string
               SgExprStatement* expressionStatement = isSgExprStatement(astNode);
               ROSE_ASSERT (expressionStatement != NULL);

               ArrayAssignmentStatementQuerySynthesizedAttributeType innerLoopTransformation =
                    synthesizedAttributeList[SgExprStatement_expression_root];

            // Make sure that the indexing computation macro has been edited away
            // ROSE_ASSERT ( innerLoopTransformation.getTransformation().getWorkSpace().find("INDEXING_SUBSCRIPT_COMPUTATION") == RoseString::npos );


            // Verify that we have a valid string
            // ROSE_ASSERT (innerLoopTransformation.isEmpty() == false);
               ROSE_ASSERT (innerLoopTransformation.isEmpty() == true);
               ROSE_ASSERT (innerLoopTransformation.getWorkSpace().c_str() != NULL);

#if 0
               printf ("Exiting as a test before call to expressionStatementTransformation() (workspace=%s) \n",
                    innerLoopTransformation.getWorkSpace().c_str());
               ROSE_ABORT();
#endif

#if 1
            // Call another global support
               returnSynthesizedAttribute +=
                    expressionStatementTransformation (
                         expressionStatement,
                         arrayAssignmentStatementQueryInheritedData,
                         innerLoopTransformation,
                         operandDataBase );

            // Verify that we have a valid string
               ROSE_ASSERT (returnSynthesizedAttribute.isEmpty() == false);
#endif

#if 0
               returnSynthesizedAttribute.display("Called from EXPR_STMT case!");
               printf ("Exiting as a test after call to expressionStatementTransformation() (workspace=%s) \n",
                    innerLoopTransformation.getWorkSpace().c_str());
               ROSE_ABORT();
#endif

            // Verify that we have saved the variable declarations
            // ROSE_ASSERT (returnSynthesizedAttribute.getVariableDeclaration().isEmpty() == false);
               break;
             }

       // I don't know if we need this! Though it might be helpful to assemble list containing
       // multiple Expressions as "arg1 $OPERATOR arg2" instead of the default "arg1 arg2"
       // which is more complex to parse and separate just to stick and operator between them!
          case EXPR_LIST:
             {
            // Most overloaded function accept their lhs and rhs within the parameter list
            // (e.g. friend T operator+(T lhs,T rhs) or T T::operator=(T rhs) for a type of class T)

            // To figure out how to handle the the expression list we need to figure out
            // if it is an expression list from an overloaded function from A++/P++.
            // There could be three approaches to this:
            //      1) Use an inherited attribute.
            //         But this will NOT work since then nesting of the query would not resolve 
            //         which expression lists should be implemented with the context that they
            //         are associated with a function call (parameter list).
            //      2) Use a second query.
            //         This will not work since any additional expression lists in the subtree would 
            //         be treated the same way. And not the correct way!
            //      3) Check the parent of the expression list to see if it is a part of an overloaded function.
            //         This is the only robust mechanism we can see to know the context of the 
            //         transformation associated with an expression list.

               SgExprListExp* exprListExp = isSgExprListExp( astNode );
               ROSE_ASSERT (exprListExp != NULL);

            // Get the parent and check if it is an overloaded operator
               SgExpression* parentExpression = isSgExpression(exprListExp->get_parent());
               ROSE_ASSERT (parentExpression != NULL);

               printf ("In EXPR_LIST switch based on parentExpression = %s \n",parentExpression->sage_class_name());

#if 0
               SgFunctionCallExp* functionCallExpression = isSgFunctionCallExp(parentExpression);
               if (functionCallExpression != NULL)
                  {
                    printf ("This is a valid SgFunctionCallExp object (variant = %d) \n",parentExpression->variant());
                  }
#endif

               switch ( parentExpression->variant() )
                  {
                    case FUNC_CALL:
                       {
                         SgFunctionCallExp* functionCallExpression = isSgFunctionCallExp(parentExpression);
                         ROSE_ASSERT (functionCallExpression != NULL);

                      // printf ("Parent is a FUNC_CALL case \n");

                      // Get the name of the parent function
                         string operatorName = TransformationSupport::getFunctionName ( functionCallExpression );
                         ROSE_ASSERT (operatorName.c_str() != NULL);

                      // Get the value of the offsets (start the search from the functionCallExp)
                         SgExprListExp* exprListExp = functionCallExpression->get_args();
                         ROSE_ASSERT (exprListExp != NULL);

                         SgExpressionPtrList & expressionPtrList = exprListExp->get_expressions();
                         int numberOfParameters = expressionPtrList.size();

                      // Classify the function name (it could be an overloaded function)
                         TransformationSupport::operatorCodeType operatorCodeVariant =
                              TransformationSupport::classifyOverloadedOperator
                                   (operatorName.c_str(),numberOfParameters);
#if 0
                         printf ("Parent operatorName = %s numberOfParameters = %d operatorCodeVariant = %d \n",
                              operatorName.c_str(),numberOfParameters,operatorCodeVariant);
#endif
                         switch (operatorCodeVariant)
                            {
                           // normal function call (not an overloaded function) (used as the default value)
                              case TransformationSupport::FUNCTION_CALL_OPERATOR_CODE:
#if 0
                                   printf ("Found a normal function as a parent: operatorName = %s numberOfParameters = %d operatorCodeVariant = %d \n",
                                        operatorName.c_str(),numberOfParameters,operatorCodeVariant);
#endif
                                   ROSE_ASSERT (synthesizedAttributeList.size() == 1);
                                   returnString = synthesizedAttributeList[0].getWorkSpace();
                                   unsigned int n;
                                   for (n = 1; n < synthesizedAttributeList.size(); n++)
                                      {
                                     // concatenate the synthesized attributes
                                        returnString = returnString + string(",") + 
                                                       synthesizedAttributeList[n].getWorkSpace();
                                      }
                                   returnString = string("$FUNCTION_OPERATOR(") + returnString + string(")");
                                // printf ("Adding \"()\": returnString = %s \n",returnString.c_str());
                                   break;

                              case TransformationSupport::PARENTHESIS_OPERATOR_CODE:
                                // returnString = " --operator()() not implemented-- ";
                                // printf ("Found an operator()() (operator code = %d)  \n",operatorCodeVariant);
                                   returnString = "";
                                   for (i = synthesizedAttributeList.begin(); i != synthesizedAttributeList.end(); i++)
                                      {
                                     // concatenate the synthesized attributes
                                        returnString = returnString + (*i).getWorkSpace();
                                      }
                                   break;

                              case TransformationSupport::ASSIGN_OPERATOR_CODE:
                                   ROSE_ASSERT (synthesizedAttributeList.size() == 1);
                                   returnString = "$ASSIGNMENT_OPERATOR " + synthesizedAttributeList[0].getWorkSpace();

                                // reset the workspace in the sysnthesized attribute
                                // printf ("Before reseting the work space: returnSynthesizedAttribute.getWorkSpace() = %s \n",returnSynthesizedAttribute.getWorkSpace().c_str());
                                // returnSynthesizedAttribute.setWorkSpace("");
                                // returnSynthesizedAttribute.skipConcatination = true;
                                   break;

                           // Overloaded binary operators: These are implemented a friend functions taking 2
                           // parameters instead of member functions taking only 1 parameter, as a result they
                           // have to be handled differently.
                              case TransformationSupport::ADD_OPERATOR_CODE:
                              case TransformationSupport::SUBT_OPERATOR_CODE:
                              case TransformationSupport::MULT_OPERATOR_CODE:
                              case TransformationSupport::DIV_OPERATOR_CODE:
                              case TransformationSupport::INTEGER_DIV_OPERATOR_CODE:
                              case TransformationSupport::MOD_OPERATOR_CODE:
                              case TransformationSupport::AND_OPERATOR_CODE:
                              case TransformationSupport::OR_OPERATOR_CODE:
                              case TransformationSupport::BITXOR_OPERATOR_CODE:
                              case TransformationSupport::BITAND_OPERATOR_CODE:
                              case TransformationSupport::BITOR_OPERATOR_CODE:
                              case TransformationSupport::EQ_OPERATOR_CODE:
                              case TransformationSupport::LT_OPERATOR_CODE:
                              case TransformationSupport::GT_OPERATOR_CODE:
                              case TransformationSupport::NE_OPERATOR_CODE:
                              case TransformationSupport::LE_OPERATOR_CODE:
                              case TransformationSupport::GE_OPERATOR_CODE:
                              case TransformationSupport::PLUS_ASSIGN_OPERATOR_CODE:
                              case TransformationSupport::MINUS_ASSIGN_OPERATOR_CODE:
                              case TransformationSupport::AND_ASSIGN_OPERATOR_CODE:
                              case TransformationSupport::IOR_ASSIGN_OPERATOR_CODE:
                              case TransformationSupport::MULT_ASSIGN_OPERATOR_CODE:
                              case TransformationSupport::DIV_ASSIGN_OPERATOR_CODE:
                              case TransformationSupport::MOD_ASSIGN_OPERATOR_CODE:
                              case TransformationSupport::XOR_ASSIGN_OPERATOR_CODE:
                                 {
                                // printf ("Found an OVERLOADED BINARY OPERATOR expression \n");
                                   SgExpression* expression = isSgExpression(astNode);
                                   ROSE_ASSERT (expression != NULL);

                                   returnString =
                                        synthesizedAttributeList[0].getWorkSpace() +
                                        "$BINARY_OPERATOR" +
                                        synthesizedAttributeList[1].getWorkSpace();
                                   break;
                                 }

                           // unary operators:
                              case TransformationSupport::NOT_OPERATOR_CODE:
                              case TransformationSupport::DEREFERENCE_OPERATOR_CODE:
                              case TransformationSupport::ADDRESS_OPERATOR_CODE:
                              case TransformationSupport::LSHIFT_OPERATOR_CODE:
                              case TransformationSupport::RSHIFT_OPERATOR_CODE:
                              case TransformationSupport::LSHIFT_ASSIGN_OPERATOR_CODE:
                              case TransformationSupport::RSHIFT_ASSIGN_OPERATOR_CODE:
                              case TransformationSupport::PREFIX_PLUSPLUS_OPERATOR_CODE:
                              case TransformationSupport::POSTFIX_PLUSPLUS_OPERATOR_CODE:
                              case TransformationSupport::PREFIX_MINUSMINUS_OPERATOR_CODE:
                              case TransformationSupport::POSTFIX_MINUSMINUS_OPERATOR_CODE:
                                 {
                                // Assemble the string for "!A" instead of "A!" which is what is currently generated!
                                // assemble the name of the function name + variable refernece to be the string (instead of default assembly!) 

                                // printf ("Found an OVERLOADED UNARY OPERATOR expression \n");

                                   printf ("ERROR: Still need to associated operator with unary expression \n");
                                   ROSE_ABORT();

                                   returnString = synthesizedAttributeList[0].getWorkSpace();
                                   break;
                                 }

                              default:
                                   printf ("default in switch found in variant = %d (arglist parent function name not implemented) \n",operatorCodeVariant);
                                   ROSE_ABORT();
                            }

                         printf ("EXPR_LIST switch, Parent is a FUNC_CALL: returnString = %s \n",returnString.c_str());

                         break;
                       }

                    case CONSTRUCTOR_INIT:
                       {
                      // This case is important for the identification of scalars used within
                      // indexing operators (not the same thing as scalar indexing)
                      // printf ("Parent of SgExprListExp is SgContructorInitializer \n");

#if 0
                      // ROSE_ASSERT (synthesizedAttributeList.size() == 1);

                      // error checking
                         if (ROSE_DEBUG > -1)
                            {
                           // Check the parent's parent to make sure it came from a A++/P++
                           // array::operator() membr function.
                              SgExpression* grandparentExpression = parentExpression->get_parent();
                              ROSE_ASSERT (grandparentExpression != NULL);
                              printf ("grandparentExpression = %s \n",grandparentExpression->sage_class_name());

                              SgExpression* greatgrandparentExpression = grandparentExpression->get_parent();
                              ROSE_ASSERT (greatgrandparentExpression != NULL);

                              printf ("greatgrandparentExpression = %s \n",greatgrandparentExpression->sage_class_name());

                              SgFunctionCallExp* functionCallExpression = isSgFunctionCallExp(greatgrandparentExpression);
                              ROSE_ASSERT (functionCallExpression != NULL);

                           // Get the name of the parent function
                              string operatorName = ArrayTransformationSupport::getFunctionName ( functionCallExpression );
                              ROSE_ASSERT (operatorName.c_str() != NULL);

                           // Classify the function name (it could be an overloaded function)
                              TransformationSupport::operatorCodeType operatorCodeVariant =
                                   TransformationSupport::classifyOverloadedOperator (operatorName.c_str());
                           // printf ("operatorCodeVariant = %d \n",operatorCodeVariant);

                              ROSE_ASSERT (operatorCodeVariant == ArrayTransformationSupport::PARENTHESIS_OPERATOR_CODE);
                            }
                         ROSE_ASSERT (synthesizedAttributeList.size() == 1);
#endif
                         ROSE_ASSERT (synthesizedAttributeList.size() > 0);
                         string integerExpressionString = synthesizedAttributeList[0].getWorkSpace();

                      // retrieve the variable name from the data base (so that we can add the associated index object names)
                      // printf ("returnSynthesizedAttribute.arrayOperandList.size() = %d \n",
                      //     returnSynthesizedAttribute.arrayOperandList.size());
                      // printf ("operandDataBase.size() = %d \n",operandDataBase.size());
                         ROSE_ASSERT (operandDataBase.arrayOperandList.size() > 0);
                         int indexOfLastOperand = operandDataBase.size() - 1;
                      // printf ("index of last operand = %d \n",indexOfLastOperand);
                         string arrayVariableName = operandDataBase.arrayOperandList[indexOfLastOperand].arrayVariableName;
                      // string arrayVariableName = "XYZ";
                      // printf ("arrayVariableName = %s integerExpressionString = %s \n",arrayVariableName.c_str(),integerExpressionString.c_str());

                      // Record the name of the Index object used
                      // returnSynthesizedAttribute.addIntegerExpressionIndexInformation(arrayVariableName,integerExpressionString);
                         operandDataBase.addIntegerExpressionIndexInformation(arrayVariableName,integerExpressionString);

                      // This handles the case of "A(I,2)" where the "2" is promoted to a
                      // InternalIndex though a call to the InternalIndex constructor which takes a
                      // single integer.
                         returnString = integerExpressionString;

#if 0
                         printf ("Exiting as a test of this case (SgContructorInitializer) \n");
                         ROSE_ABORT();
#endif
                         break;
                       }

                    default:
                       {
                      // Not really sure how to handle it since it depends upon what the parent is!

                      // printf ("parentExpression = %s \n",parentExpression->sage_class_name());

                         SgFunctionCallExp* functionCallExpression = isSgFunctionCallExp(parentExpression);
                         ROSE_ASSERT (functionCallExpression == NULL);

                         printf ("Parent of SgExprListExp is not a SgFunctionCallExp (need to allow for these cases explicitly) \n");
                         ROSE_ABORT();
                       }
                  }

            // printf ("Found case of EXPR_LIST (SgExprListExp) in assembly (returnString = %s) \n",returnString.c_str());
            // ROSE_ABORT();

               break;
             }

          case FUNC_CALL: // tag for SgFunctionCallExp class
             {
            // Error checking: Verify that we have a SgFunctionCallExp object
               SgFunctionCallExp* functionCallExpression = isSgFunctionCallExp(astNode);
               ROSE_ASSERT (functionCallExpression != NULL);

            // char* operatorName = memberFunctionSymbol->get_name().str();
            // char* operatorName = ArrayTransformationSupport::getFunctionName ( functionCallExpression );
               string operatorName = TransformationSupport::getFunctionName ( functionCallExpression );
               ROSE_ASSERT (operatorName.c_str() != NULL);

               string functionTypeName = TransformationSupport::getFunctionTypeName(functionCallExpression);

            // SgType* functionType    = functionCallExpression->get_type();
            // string functionTypeName = TransformationSupport::getTypeName (functionType);

            // printf ("overloaded operator is of type = %s \n",functionTypeName.c_str());

               if ( (functionTypeName != "doubleArray") && (functionTypeName != "floatArray") && (functionTypeName != "intArray") )
                  {
                 // Use this query to handle only A++ function call expressions
                 // printf ("Break out of overloaded operator processing since type = %s is not to be processed \n",functionTypeName.c_str());
                    break;
                  }
                 else
                  {
                 // printf ("Processing overloaded operator of type = %s \n",functionTypeName.c_str());
                  }

               ROSE_ASSERT ( (functionTypeName == "doubleArray") || 
                             (functionTypeName == "floatArray")  ||
                             (functionTypeName == "intArray") );

            // printf ("CASE FUNC_CALL: Overloaded operator = %s \n",operatorName.c_str());

            // Get the number of parameters to this function
               SgExprListExp* exprListExp = functionCallExpression->get_args();
               ROSE_ASSERT (exprListExp != NULL);

               SgExpressionPtrList & expressionPtrList = exprListExp->get_expressions();
               int numberOfParameters = expressionPtrList.size();

               TransformationSupport::operatorCodeType operatorCodeVariant =
                    TransformationSupport::classifyOverloadedOperator (operatorName.c_str(),numberOfParameters);
            // printf ("CASE FUNC_CALL: numberOfParameters = %d operatorCodeVariant = %d \n",
            //      numberOfParameters,operatorCodeVariant);

               ROSE_ASSERT (operatorName.length() > 0);

            // Separating this case into additional cases makes up to some 
            // extent for using a more specific higher level grammar.
               switch (operatorCodeVariant)
                  {
                 // normal function call (not an overloaded function) (used as the default value)
                    case TransformationSupport::FUNCTION_CALL_OPERATOR_CODE:
                       {
#if 0
                         printf ("Found a normal function: operatorName = %s numberOfParameters = %d operatorCodeVariant = %d \n",
                              operatorName.c_str(),numberOfParameters,operatorCodeVariant);
                         printf ("function name = %s synthesizedAttributeList.size() = %d \n",
                              operatorName.c_str(),synthesizedAttributeList.size());
#endif
                      // ROSE_ASSERT (synthesizedAttributeList.size() == 1);
                         string parameterListString = synthesizedAttributeList[0].getWorkSpace();
                         returnString = operatorName + string("(") + parameterListString + string(")");
                      // printf ("CASE FUNC_CALL: Adding \"()\": returnString = %s \n",returnString.c_str());
                         break;
                       }

                    case TransformationSupport::ASSIGN_OPERATOR_CODE:
                       {
                      // The overloaded assignment operator has only the rhs in the argument list
                      // (the lhs is index using the value SgFunctionCallExp_function in the
                      // synthesizedAttributeList)
                      // printf ("Found an OVERLOADED ASSIGNMENT OPERATOR expression: T::operator=(const T & X) \n");

                         operatorString = TransformationSupport::buildOperatorString (astNode);

                         string lhsString = synthesizedAttributeList[SgFunctionCallExp_function].getWorkSpace();
                         string rhsString = synthesizedAttributeList[SgFunctionCallExp_args].getWorkSpace();

#if 0
                         printf ("lhsString      = %s \n",lhsString.c_str());
                         printf ("operatorString = %s \n",operatorString.c_str());
                         printf ("rhsString      = %s \n",rhsString.c_str());
#endif

                      // string lhsOperandName = operandDataBase.arrayOperandList[0].arrayVariableName;
                         vector<ArrayOperandDataBase>::iterator lhs = operandDataBase.arrayOperandList.begin();
                         vector<ArrayOperandDataBase>::iterator rhs = lhs;
                         rhs++;
                         while ( rhs != operandDataBase.arrayOperandList.end() )
                            {
                           // look at the operands on the rhs for a match with the one on the lhs
                              if ( (*lhs).arrayVariableName == (*rhs).arrayVariableName )
                                 {
                                // A loop dependence has been identified
                                // printf ("A loop dependence has been identified (*lhs) = %s (*rhs) = %s \n",
                                //      (*lhs).arrayVariableName.c_str(),(*rhs).arrayVariableName.c_str());

                                // Mark the synthesized attribute to record 
                                // the loop dependence within this statement
                                   returnSynthesizedAttribute.setLoopDependence(TRUE);
                                   returnSynthesizedAttribute.setLoopDependenceLhs(lhsString);
                                   returnSynthesizedAttribute.setLoopDependenceRhs(rhsString);
                                 }

                              rhs++;
                            }

                      // Assemble the final return string
                         returnString = lhsString + rhsString;

                      // Edit the correct operator into place
                         returnString = SgNode::copyEdit(returnString,"$ASSIGNMENT_OPERATOR",operatorString);

#if 0
                         printf ("Exiting after handling overloaded binary assignment operator=(): returnString = %s \n",returnString.c_str());
                         ROSE_ABORT();
#endif
                      // returnSynthesizedAttribute.setTransformationSourceCode(returnString);
                         break;
                       }

                 // Overloaded binary operators: These are implemented a friend functions taking 2
                 // parameters instead of member functions taking only 1 parameter, as a result they
                 // have to be handled differently.
                    case TransformationSupport::ADD_OPERATOR_CODE:
                    case TransformationSupport::SUBT_OPERATOR_CODE:
                    case TransformationSupport::MULT_OPERATOR_CODE:
                    case TransformationSupport::DIV_OPERATOR_CODE:
                    case TransformationSupport::INTEGER_DIV_OPERATOR_CODE:
                    case TransformationSupport::MOD_OPERATOR_CODE:
                    case TransformationSupport::AND_OPERATOR_CODE:
                    case TransformationSupport::OR_OPERATOR_CODE:
                    case TransformationSupport::BITXOR_OPERATOR_CODE:
                    case TransformationSupport::BITAND_OPERATOR_CODE:
                    case TransformationSupport::BITOR_OPERATOR_CODE:
                    case TransformationSupport::EQ_OPERATOR_CODE:
                    case TransformationSupport::LT_OPERATOR_CODE:
                    case TransformationSupport::GT_OPERATOR_CODE:
                    case TransformationSupport::NE_OPERATOR_CODE:
                    case TransformationSupport::LE_OPERATOR_CODE:
                    case TransformationSupport::GE_OPERATOR_CODE:
                    case TransformationSupport::PLUS_ASSIGN_OPERATOR_CODE:
                    case TransformationSupport::MINUS_ASSIGN_OPERATOR_CODE:
                    case TransformationSupport::AND_ASSIGN_OPERATOR_CODE:
                    case TransformationSupport::IOR_ASSIGN_OPERATOR_CODE:
                    case TransformationSupport::MULT_ASSIGN_OPERATOR_CODE:
                    case TransformationSupport::DIV_ASSIGN_OPERATOR_CODE:
                    case TransformationSupport::MOD_ASSIGN_OPERATOR_CODE:
                    case TransformationSupport::XOR_ASSIGN_OPERATOR_CODE:
                       {
                      // Binary overloaded operators have all their arguments in the argument list
                      // printf ("Found an OVERLOADED BINARY OPERATOR expression \n");

                         SgExpression* expression = isSgExpression(astNode);
                         ROSE_ASSERT (expression != NULL);

                         SgExpression* parentExpression = isSgExpression(expression->get_parent());
                         int parent_precedence = (parentExpression) ? parentExpression->precedence() : 0;
                         int our_precedence    = expression->precedence();
                         int skip_parens       = ((parent_precedence > 0) && (our_precedence > parent_precedence));

                         operatorString = TransformationSupport::buildOperatorString (astNode);

                         string expressionString =
                              synthesizedAttributeList[SgFunctionCallExp_args].getWorkSpace();
                         returnString = SgNode::copyEdit(expressionString,"$BINARY_OPERATOR",operatorString);
                         if (!skip_parens)
                            {
                              returnString = "(" + returnString + ")";
                            }

                      // printf ("CASE FUNC_CALL (overloaded operator): Adding \"()\": returnString = %s \n",returnString.c_str());

#if 0
                         printf ("Exiting after handling general overloaded binary operator: returnString = %s \n",returnString.c_str());
                         ROSE_ABORT();
#endif

                      // returnSynthesizedAttribute.setTransformationSourceCode(returnString);

                      // returnString = buffer;
                         break;
                       }

                 // unary operators:
                    case TransformationSupport::NOT_OPERATOR_CODE:
                    case TransformationSupport::DEREFERENCE_OPERATOR_CODE:
                    case TransformationSupport::ADDRESS_OPERATOR_CODE:
                    case TransformationSupport::LSHIFT_OPERATOR_CODE:
                    case TransformationSupport::RSHIFT_OPERATOR_CODE:
                    case TransformationSupport::LSHIFT_ASSIGN_OPERATOR_CODE:
                    case TransformationSupport::RSHIFT_ASSIGN_OPERATOR_CODE:
                    case TransformationSupport::PREFIX_PLUSPLUS_OPERATOR_CODE:
                    case TransformationSupport::POSTFIX_PLUSPLUS_OPERATOR_CODE:
                    case TransformationSupport::PREFIX_MINUSMINUS_OPERATOR_CODE:
                    case TransformationSupport::POSTFIX_MINUSMINUS_OPERATOR_CODE:
                       {
                      // Assemble the string for "!A" instead of "A!" which is what is currently generated!
                      // assemble the name of the function name + variable refernece to be the string (instead of default assembly!) 

                      // printf ("Found an OVERLOADED UNARY OPERATOR expression \n");

                         string functionString   = synthesizedAttributeList[SgFunctionCallExp_function].getWorkSpace();
                         string expressionString = synthesizedAttributeList[SgFunctionCallExp_args].getWorkSpace();
                         returnString = functionString + expressionString;

#if 1
                         printf ("Exiting after handling general overloaded unary operator: returnString = %s \n",returnString.c_str());
                         ROSE_ABORT();
#endif

                      // returnSynthesizedAttribute.setTransformationSourceCode(returnString);
                         break;
                       }

                    case TransformationSupport::PARENTHESIS_OPERATOR_CODE:
                       {
                         ROSE_ASSERT (operatorName == "operator()");
                      // printf ("Indexing of InternalIndex objects in not implemented yet! \n");

                      // Now get the operands out and search for the offsets in the index objects

                      // We only want to pass on the transformationOptions as inherited attributes
                      // to the indexOffsetQuery
                      // list<int> & transformationOptionList = arrayAssignmentStatementQueryInheritedData.getTransformationOptions();

                      // string offsetString;
                         string indexOffsetString[6]; // = {NULL,NULL,NULL,NULL,NULL,NULL};

                      // retrieve the variable name from the data base (so that we can add the associated index object names)
                      // printf ("WARNING (WHICH OPERAND TO SELECT): operandDataBase.size() = %d \n",operandDataBase.size());
                      // ROSE_ASSERT (operandDataBase.size() == 1);
                      // string arrayVariableName = returnSynthesizedAttribute.arrayOperandList[0].arrayVariableName;
                         int lastOperandInDataBase = operandDataBase.size()-1;
                         ArrayOperandDataBase & arrayOperandDB = operandDataBase.arrayOperandList[lastOperandInDataBase];
                      // string arrayVariableName =
                      //      operandDataBase.arrayOperandList[operandDataBase.size()-1].arrayVariableName;
                         string arrayVariableName = arrayOperandDB.arrayVariableName;

                         string arrayDataPointerNameSubstring = string("_") + arrayVariableName;

                      // printf ("***** WARNING: Need to get identifier from the database using the ArrayOperandDataBase::generateIdentifierString() function \n");

                         if (expressionPtrList.size() == 0)
                            {
                           // Case of A() (index object with no offset integer expression) Nothing to do here (I think???)
                              printf ("Special case of Indexing with no offset! exiting ... \n");
                              ROSE_ABORT();

                              returnString = "";
                           // returnSynthesizedAttribute.setTransformationSourceCode("");
                            }
                           else
                            {
                           // Get the value of the offsets (start the search from the functionCallExp)
                              SgExprListExp* exprListExp = functionCallExpression->get_args();
                              ROSE_ASSERT (exprListExp != NULL);

                              SgExpressionPtrList & expressionPtrList = exprListExp->get_expressions();
                              SgExpressionPtrList::iterator i = expressionPtrList.begin();

                           // Case of indexing objects used within operator()
                              int counter = 0;
                              while (i != expressionPtrList.end())
                                 {
                                // printf ("Looking for the offset on #%d of %d (total) \n",counter,expressionPtrList.size());

                                // Build up the name of the final index variable (or at least give
                                // it a unique number by dimension)
                                   string counterString  = StringUtility::numberToString(counter+1);

                                // Call another transformation mechanism to generate string for the index
                                // expression (since we don't have an unparser mechanism in ROSE yet)
                                   indexOffsetString [counter] = IndexOffsetQuery::transformation ( *i );

                                   ROSE_ASSERT (indexOffsetString[counter].c_str() != NULL);
                                // printf ("indexOffsetString [%d] = %s \n",counter,indexOffsetString[counter].c_str());

                                // Accumulate a list of all the InternalIndex, Index, and Range objects
                                   list<string> indexNameList =
                                        NameQuery::getVariableNamesWithTypeNameQuery ( *i, "Index" );
                                   list<string> rangeNameList =
                                        NameQuery::getVariableNamesWithTypeNameQuery ( *i, "Range" );
                                   list<string> internalIndexNameList =
                                        NameQuery::getVariableNamesWithTypeNameQuery ( *i, "InternalIndex" );

                                // We have to sort the lists before we can call the merge function
                                   indexNameList.sort();
                                   rangeNameList.sort();
                                   internalIndexNameList.sort();

                                // Now merge the lists (to build a single list)
                                   indexNameList.merge(rangeNameList);
                                   indexNameList.merge(internalIndexNameList);

                                // Make sure that we only have one InternalIndex object used in the index expression
                                // printf ("indexNameList.size() = %d \n",indexNameList.size());
                                // printf ("indexNameList = %s \n",StringUtility::listToString(indexNameList).c_str());

                                   if (indexNameList.size() == 0)
                                      {
                                     // case of scalar indexing
                                     // The final transformations will use index variables of the form
                                     // "index1" through "index6"
                                        string indexVariableString = indexOffsetString[counter];

                                     // printf ("indexVariableString = %s \n",indexVariableString.c_str());

                                        indexOffsetString[counter] =
                                             SgNode::copyEdit(indexOffsetString[counter],"$INDEX_OBJECT",indexVariableString);

                                     // Record the name of the Index object used
                                        operandDataBase.addIndexInformation(arrayVariableName,"SCALAR");
                                      }
                                     else
                                      {
                                        ROSE_ASSERT (indexNameList.size() == 1);
                                        string indexVariableName = *(indexNameList.begin());

                                     // Accumulate a string from concatination of the index variable names
                                     // (to be use to build a unique identifier for the data pointer)
                                        arrayDataPointerNameSubstring += string("_") + indexVariableName;

                                     // The final transformations will use index variables of the form
                                     // "index1" through "index6"
                                     // string indexVariableString = string("index") + counterString;
                                        string indexVariableString = string("_") + counterString;

                                     // printf ("indexVariableString = %s \n",indexVariableString.c_str());

                                        indexOffsetString[counter] =
                                             SgNode::copyEdit(indexOffsetString[counter],"$INDEX_OBJECT",indexVariableString);

                                     // printf ("indexOffsetString[%d] = %s \n",counter,indexOffsetString[counter].c_str());

#if 0
                                        printf ("Before addIndexInformation(): arrayVariableName = %s indexVariableName = %s operandDataBase.displayString() = %s \n",
                                             arrayVariableName.c_str(),
                                             indexVariableName.c_str(),
                                             operandDataBase.displayString().c_str());
#endif
                                     // Record the name of the Index object used
                                        operandDataBase.addIndexInformation(arrayVariableName,indexVariableName);

                                     // printf ("After addIndexInformation(): operandDataBase.displayString() = %s \n",operandDataBase.displayString().c_str());
                                      }

#if 0
                                   printf ("Exiting after finding an InternalIndex in array::operator()(InternalIndex) ... \n");
                                   ROSE_ABORT();
#endif

                                   i++;
                                   counter++;
                                 }

                           // Need to fix subscript computation if counter > 1 (so make this case an error)
                              ROSE_ASSERT (counter >= 0);
                              ROSE_ASSERT (counter <= maxNumberOfIndexOffsets);

                           // arrayAssignmentStatementQueryInheritedData.display("FUNC_CALL case in localArrayAssignmentStatementQueryFunction()");

                           // The inherited attribute needs to be modified with the computed array
                           // dimension and then used in subsequent queries.  But we can't modify the
                           // inherited attribute by desing so we have to use an other inherited
                           // attribute as with the correct array dimension.  This is not implemented yet
                           // in the array assignement statement transformation.
#if 0
                              printf ("counter = %d maxNumberOfIndexOffsets = %d (setting maxNumberOfIndexOffsets = counter) \n",
                                   counter,maxNumberOfIndexOffsets);
#endif
                           // This code allows the transformation to be continued, but with out the
                           // correct handling of the offsets (all six values are built)
                           // maxNumberOfIndexOffsets = counter;
                              ROSE_ASSERT (counter <= maxNumberOfIndexOffsets);

                           // This is the more strict requirement that has to be asserted in A++/P++
                              ROSE_ASSERT (counter == maxNumberOfIndexOffsets);

                           // reset the offsetString to use the new list of index expressions
                              int n = 0;
                              offsetString = "";
                              for (n = 0; n < maxNumberOfIndexOffsets; n++)
                                 {
                                // Add the different expressions represented by the indexing strings together
                                   if ( n > 0 )
                                        offsetString += ",";

                                   ROSE_ASSERT (indexOffsetString[n].c_str() != NULL);
                                   offsetString += indexOffsetString[n];
                                 }
                            }

                      // Declare the string which will be modified
                         string arrayReferenceString =
                              synthesizedAttributeList[SgFunctionCallExp_function].getWorkSpace();
                      // Modify the $OFFSET in for example:
                      // $IDENTIFIER_STRING_pointer[SC$IDENTIFIER_STRING($OFFSET)]
                         returnString = SgNode::copyEdit(arrayReferenceString,"$OFFSET",offsetString);

                      // Modify the $IDENTIFIER_STRING in for example:
                      // $IDENTIFIER_STRING_pointer[SC_$IDENTIFIER_STRING($OFFSET)]
                         returnString = SgNode::copyEdit(returnString,"$IDENTIFIER_STRING",arrayDataPointerNameSubstring);

                      // Optimize the case of uniform or unit indexing to generate a single subscript macro definition
                         if ( (arrayOperandDB.indexingAccessCode == ArrayTransformationSupport::UniformSizeUnitStride) ||
                              (arrayOperandDB.indexingAccessCode == ArrayTransformationSupport::UniformSizeUniformStride) )
                              returnString = SgNode::copyEdit (returnString,"$MACRO_NAME_SUBSTRING","");
                           else
                              returnString = SgNode::copyEdit (returnString,"$MACRO_NAME_SUBSTRING",arrayDataPointerNameSubstring);
                      // printf ("In FUNC_CALL operator() case: operandDataBase.displayString() = %s \n",
                      //      operandDataBase.displayString().c_str());

#if 0
                         printf ("Exiting after finding FUNC_CALL operator() returnString = %s ... \n",returnString.c_str());
                         ROSE_ABORT();
#endif
                         break;
                       }

                    default:
                         printf ("default in switch found in variant = %d (NAME NOT IMPLEMENTED) \n",operatorCodeVariant);
                         ROSE_ABORT();
                  }

            // printf ("Base of FUNC_CALL case: returnString   = %s \n",returnString.c_str());
#if 0
               returnString = substituteSubstriptIndexingMacro (
                                   arrayAssignmentStatementQueryInheritedData,
                                   offsetString,
                                   returnString );
#endif
#if 0
            // printf ("Base of FUNC_CALL case: AFTER substituteSubstriptIndexingMacro(): returnString    = %s \n",
            //      returnString.c_str());

            // printf ("Base of FUNC_CALL case: returnString = %s \n",returnString.c_str());
#endif
               break;
             }

       // These cases have been moved from the local function
       // Values
          case ValueExpTag:
          case BOOL_VAL:
          case STRING_VAL:
          case SHORT_VAL:
          case CHAR_VAL:
          case UNSIGNED_CHAR_VAL:
          case WCHAR_VAL:
          case UNSIGNED_SHORT_VAL:
          case ENUM_VAL:
          case UNSIGNED_INT_VAL:
          case LONG_INT_VAL:
          case LONG_LONG_INT_VAL:
          case UNSIGNED_LONG_LONG_INT_VAL:
          case UNSIGNED_LONG_INT_VAL:
          case INT_VAL:
          case FLOAT_VAL:
          case DOUBLE_VAL:
          case LONG_DOUBLE_VAL:
             {
            // Call the unparser to generate a string for any value (shortest possible way to generate string)
               returnString = astNode->unparseToString();
            // printf ("returnString (type value) = %s \n",returnString.c_str());
               break;
             }

       // Added VAR_REF case (moved from the local function)
          case VAR_REF:
             {
            // A VAR_REF has to output a string (the variable name)
            // printf ("Found a variable reference expression \n");

            // Since we are at a leaf in the traversal of the AST this attribute list should a size of 0.
               ROSE_ASSERT (synthesizedAttributeList.size() == 0);

               SgVarRefExp* varRefExp = isSgVarRefExp(astNode);
               ROSE_ASSERT (varRefExp != NULL);
               SgVariableSymbol* variableSymbol = varRefExp->get_symbol();
               ROSE_ASSERT (variableSymbol != NULL);
               SgInitializedName* initializedName = variableSymbol->get_declaration();
               ROSE_ASSERT (initializedName != NULL);
               SgName variableName = initializedName->get_name();

            // copy the string to avoid corruption of the AST's version of the string
            // returnString = rose::stringDuplicate(variableName.str());
            // printf ("returnString (variable name) = %s \n",returnString);

               string buffer;
               string indexOffsetString;

            // Now compute the offset to the index objects (form a special query for this???)


            // printf ("Exiting after finding the Index objects in the operand ... \n");
            // ROSE_ABORT();

               SgType* type = variableSymbol->get_type();
               ROSE_ASSERT (type != NULL);

            // SgClassType* classType = isSgClassType( type );
            // ROSE_ASSERT (classType != NULL);
            // char* typeName = classType->get_name().str();

               string typeName = TransformationSupport::getTypeName(type);
               ROSE_ASSERT (typeName.c_str() != NULL);

            // printf ("In VAR_REF case: typeName = %s \n",typeName.c_str());

            // Recognize only these types at present
               if ( typeName == "intArray" || typeName == "floatArray" || typeName == "doubleArray" )
                  {
                 // Only define the variable name if we are using an object of array type

                 // printf("Handle case of A++ array object \n");

                 // Use a macro to make this easier to read
                    string subscriptString;

                 // Handle the selection of subscript macro at a higher level of the AST
                 // subscriptString = "$INDEXING_SUBSCRIPT_COMPUTATION";

                 // printf("Do the string manipulation \n");

                 // Copy the string from the SgName object to a string object
                    string variableNameString = variableName.str();

                 // printf ("variableNameString = %s \n",variableNameString.c_str());

                 // Setup an intry in the synthesized attribute data base for this variable any
                 // future results from analysis could be place there at this point as well
                 // record the name in the synthesized attribute
                 // returnSynthesizedAttribute.setVariableName(variableNameString);
                    ROSE_ASSERT ( operandDataBase.transformationOption > ArrayTransformationSupport::UnknownIndexingAccess );
                    ArrayOperandDataBase arrayOperandDB = operandDataBase.setVariableName(variableNameString);

                 // We could have specified in the inherited attribute that this array variable was
                 // index and if so leave the value of $IDENTIFIER_STRING to be modified later in
                 // the assembly of the operator() and if not do the string replacement on
                 // $IDENTIFIER_STRING here (right now).

                    returnString = string("$IDENTIFIER_STRING") + string("_pointer[SC") + 
                                   string("$MACRO_NAME_SUBSTRING") + string("(") + 
                                   string("$OFFSET") + string(")]");

                 // The inherited attribute mechanism is not yet implimented
                    if (arrayAssignmentStatementQueryInheritedData.getIsIndexedArrayOperand() == FALSE)
                       {
                      // do the substitution of $OFFSET here since it our last chance
                      // (offsetString is the list of index values "index1,index2,...,indexn")
                         returnString = SgNode::copyEdit(returnString,"$OFFSET",offsetString);

                         string operandIdentifier = arrayOperandDB.generateIdentifierString();
                      // do the substitution of $IDENTIFIER_STRING here since it our last chance
                      // if variable name is "A", generate: A_pointer[SC_A(index1,...)]
                      // returnString = SgNode::copyEdit (returnString,"$IDENTIFIER_STRING",variableNameString);
                         ROSE_ASSERT ( arrayOperandDB.indexingAccessCode > 
                                       ArrayTransformationSupport::UnknownIndexingAccess);

                      // Edit into place the name of the data pointer
                         returnString = SgNode::copyEdit (returnString,"$IDENTIFIER_STRING",operandIdentifier);

                      // Optimize the case of uniform or unit indexing to generate a single subscript macro definition
                         if ( (arrayOperandDB.indexingAccessCode == ArrayTransformationSupport::UniformSizeUnitStride) ||
                              (arrayOperandDB.indexingAccessCode == ArrayTransformationSupport::UniformSizeUniformStride) )
                              returnString = SgNode::copyEdit (returnString,"$MACRO_NAME_SUBSTRING","");
                           else
                              returnString = SgNode::copyEdit (returnString,"$MACRO_NAME_SUBSTRING",operandIdentifier);
                       }
                  }
                 else
                  {
                 // printf("Not an A++ array object \n");

                 // variable is of some other type wo just output the variable name as an unparser would
                 // returnString = rose::stringDuplicate(variableName.str());
                    returnString = variableName.str();
                  }

            // printf ("In VAR_REF case: returnString = %s \n",returnString.c_str());

#if 1
               printf ("After building the transformation for VAR_REF (returnString = %s) (variableName = %s) ... \n",returnString.c_str(),variableName.str());
//             ROSE_ABORT();
#endif
               break;
             }

       // Removed VAR_REF case from below
          case RECORD_REF:  // SgDotExp
       // case ValueExpTag:
          default:
             {
            // printf ("DEFAULT ASSEMBLY: default case in arrayAssignmentStatementQueryAssemblyFunction() (sage_class_name = %s \n",astNode->sage_class_name());
            // The default is to concatinate all the strings together (in the order in which they appear)
               for (i = synthesizedAttributeList.begin(); i != synthesizedAttributeList.end(); i++)
                  {
                 // printf ("In the DEFAULT CASE: SynthesizedAttributeListElements: attributeList = %s \n",(*i).getWorkSpace().c_str());
                 // Default policy for workspace concatination
                    returnString += (*i).getWorkSpace();
                  }
               break;
             }
        } // End of main switch statement

     returnSynthesizedAttribute.setWorkSpace(returnString);

#if 0
     printf ("$$$$$ BOTTOM of evaluateRewriteSynthesizedAttribute (astNode = %s) \n",astNode->sage_class_name());
  // printf ("      BOTTOM: operandDataBase = \n%s \n",operandDataBase.displayString().c_str());
     printf ("      BOTTOM: returnString = \n%s \n",returnString.c_str());
#endif

  // Error checking
  // ROSE_ASSERT (returnSynthesizedAttribute.associated_AST_Node != NULL);
  // printf ("At BASE of evaluateRewriteSynthesizedAttribute all destructors being called for local stack variables \n");
     return returnSynthesizedAttribute;
   }









