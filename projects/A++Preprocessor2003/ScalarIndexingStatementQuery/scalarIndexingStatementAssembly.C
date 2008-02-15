// Treat config.h separately from other include files
#ifdef HAVE_CONFIG_H
// This avoids requiring the user to use config.h and follows 
// the automake manual request that we use <> instead of ""
#include <config.h>
#endif

// Only include file required for all A++/P++ preprocessor source code files
#include "arrayPreprocessor.h"

// ******************************************************
//             Inherited Attribute Function
// ******************************************************
ScalarIndexingStatementQueryInheritedAttributeType
ScalarIndexingStatementTransformation::
evaluateRewriteInheritedAttribute (
   SgNode* astNode,
   ScalarIndexingStatementQueryInheritedAttributeType scalarIndexingStatementQueryInheritedData )
   {
  // This function will be of more use later when we force the context to be represented and
  // communicated through the inherited attribute. See the InheritedAttributeBaseClassType for more
  // details.

     ROSE_ASSERT (astNode != NULL);

  // Build the inherited attribute (using the source position information (required for use of
  // rewrite mechanism)).
     ScalarIndexingStatementQueryInheritedAttributeType
          returnAttribute (scalarIndexingStatementQueryInheritedData,astNode);

     return returnAttribute;
   }

// ******************************************************
//             Synthesized Attribute Function
// ******************************************************
ScalarIndexingStatementQuerySynthesizedAttributeType
ScalarIndexingStatementTransformation::
evaluateRewriteSynthesizedAttribute (
   SgNode* astNode,
   ScalarIndexingStatementQueryInheritedAttributeType scalarIndexingStatementQueryInheritedData,
   SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
  // This function assembles the elements of the input list of child synthesized 
  // attributes to form the return value (a synthesized  attribute).

#if 1
     printf ("$$$$$ TOP of scalarIndexingStatementQueryAssemblyFunction (astNode = %s) (synthesizedAttributeList.size() = %d) \n",
          astNode->sage_class_name(),synthesizedAttributeList.size());
#endif

  // Error checking
     ROSE_ASSERT (astNode != NULL);

#if 0
  // Print out the elements of the list
     int i = 0;
     for (i = 0; i < synthesizedAttributeList.size(); i++)
        {
          ROSE_ASSERT (synthesizedAttributeList[i].astNode != NULL);
          printf ("Element %d of SynthesizedAttributeListElements: variant = %d (%s) \ndisplayString = %s \n",
               i,synthesizedAttributeList[i].astNode->variantT(),
               synthesizedAttributeList[i].astNode->sage_class_name(),
               synthesizedAttributeList[i].displayString().c_str());
        }

  // printf ("At TOP: operandDataBase.displayString() = %s \n",operandDataBase.displayString().c_str());
#endif

  // Build the return value for this function
     ScalarIndexingStatementQuerySynthesizedAttributeType returnSynthesizedAttribute (astNode);

  // Build up a return string (used to reset the "workspace" string in the aynthesized attribute
     string returnString;

  // temporary string used in numerous case of switch statement
     string operatorString;

  // Need to handle all relavant AST nodes required by the Scalar Indexing Transformation
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
               ROSE_ASSERT (isSgUnaryOp(astNode) != NULL);
               operatorString = TransformationSupport::buildOperatorString (astNode);

               string operandString = synthesizedAttributeList[SgUnaryOp_operand_i].getWorkSpace();

            // NOTE: This does not handle prefix and postfix operators properly
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
               ROSE_ASSERT (isSgBinaryOp(astNode) != NULL);
               operatorString = TransformationSupport::buildOperatorString (astNode);
               returnString   = synthesizedAttributeList[SgBinaryOp_lhs_operand_i].getWorkSpace() +
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

            // Template string into which we substitute the string representing the optimized statement
               string transformationTemplateString = "\
// Automatically Introduced Scalar Indexing Transformation (via preprocessor built by ROSE)\n";

               string scalarIndexingStatementString = 
                    transformationTemplateString +
                    synthesizedAttributeList[SgExprStatement_expression_root].getWorkSpace() + ";";

            // Put the returnString into the returnSynthesizedAttribute
#if 0
               AST_Rewrite::AST_FragmentString mainTransformationFragmentString =
                    AST_Rewrite::AST_FragmentString(scalarIndexingStatementString,
                                          scalarIndexingStatementQueryInheritedData,
                                          AST_Rewrite::LocalScope,
                                          AST_Rewrite::ReplaceCurrentPosition);
               returnSynthesizedAttribute.addSourceCodeString(mainTransformationFragmentString);
#else
               returnSynthesizedAttribute.insert(
                    expressionStatement,
                    scalarIndexingStatementString,
                    ScalarIndexingStatementQuerySynthesizedAttributeType::LocalScope,
                    ScalarIndexingStatementQuerySynthesizedAttributeType::ReplaceCurrentPosition);
#endif

            // Include the include file to define the A++ classes
#if 0
               AST_Rewrite::AST_FragmentString globalIncludeTransformationFragmentString =
                    AST_Rewrite::AST_FragmentString("#include <simpleA++.h> \n int rose_dummy_var;",
                                          scalarIndexingStatementQueryInheritedData,
                                          AST_Rewrite::Preamble, // instead of AST_Rewrite::GlobalScope,
                                          AST_Rewrite::TopOfScope);
               returnSynthesizedAttribute.addSourceCodeString(globalIncludeTransformationFragmentString);
#else
            // Add this to the Preamble instead of the global scope since it 
            // only effects the final processing of the generated source code 
            // and not the generation of the intermediate files (???)
#if 0
               AST_Rewrite::addSourceCodeString(
                    returnSynthesizedAttribute,
                    "#include <simpleA++.h> \n int rose_dummy_var;",
                    scalarIndexingStatementQueryInheritedData,
                    AST_Rewrite::Preamble,
                    AST_Rewrite::TopOfScope,
                    AST_Rewrite::C_Preprocessor_Declaration,
                    false);
#else
               returnSynthesizedAttribute.insert(
                    expressionStatement,
                    "#include <simpleA++.h> \n int rose_dummy_var;",
                    ScalarIndexingStatementQuerySynthesizedAttributeType::Preamble,
                    ScalarIndexingStatementQuerySynthesizedAttributeType::TopOfScope);
#endif
#endif
            // Verify that we have a valid strings to be inserted into the AST
               ROSE_ASSERT (returnSynthesizedAttribute.isEmpty() == false);
               break;
             }

          case EXPR_LIST:
             {
               SgExprListExp* exprListExp = isSgExprListExp( astNode );
               ROSE_ASSERT (exprListExp != NULL);
               returnString = exprListExp->unparseToString();
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

               string functionTypeName = TransformationSupport::getFunctionTypeName(functionCallExpression).c_str();

               printf ("overloaded operator is of type = %s \n",functionTypeName.c_str());

               if ( (functionTypeName != "doubleArray") &&
                    (functionTypeName != "floatArray")  &&
                    (functionTypeName != "intArray") )
                  {
                 // Use this query to handle only A++ function call expressions
                    printf ("Break out of overloaded operator processing since type = %s is not to be processed \n",functionTypeName.c_str());
                 // returnSynthesizedAttribute.setTransformationString("");
                    break;
                  }
                 else
                  {
                    printf ("Processing overloaded operator of type = %s \n",functionTypeName.c_str());
                  }

               ROSE_ASSERT ( (functionTypeName == "doubleArray") || 
                             (functionTypeName ==  "floatArray") ||
                             (functionTypeName ==    "intArray") );

               printf ("CASE FUNC_CALL: Overloaded operator = %s \n",operatorName.c_str());

            // Get the number of parameters to this function since it is required by the
            // TransformationSupport::classifyOverloadedOperator function.  Perhaps this function's
            // interface should be extended so that it is not so much trouble to call it. Maybe it
            // should take a SgFunctionCallExp* instead?
               SgExprListExp* exprListExp = functionCallExpression->get_args();
               ROSE_ASSERT (exprListExp != NULL);

               SgExpressionPtrList & expressionPtrList = exprListExp->get_expressions();
               int numberOfParameters = expressionPtrList.size();

               TransformationSupport::operatorCodeType operatorCodeVariant =
                    TransformationSupport::classifyOverloadedOperator (operatorName.c_str(),numberOfParameters);
            // printf ("CASE FUNC_CALL: numberOfParameters = %d operatorCodeVariant = %d \n",
            //      numberOfParameters,operatorCodeVariant);

            // Separating this case into additional cases makes up to some extent for using a more
            // specific higher level grammar (which is not implemented yet).
               switch (operatorCodeVariant)
                  {
                    case TransformationSupport::PARENTHESIS_OPERATOR_CODE:
                       {
                         ROSE_ASSERT (operatorName == "operator()");

                      // Now get the operands out and search for the offsets in the index objects

                      // Get the value of the offsets (start the search from the functionCallExp)
                      // SgExprListExp* exprListExp = functionCallExpression->get_args();
                      // ROSE_ASSERT (exprListExp != NULL);

                      // We only want to pass on the transformationOptions as inherited attributes
                      // to the indexOffsetQuery
//                       list<int> & transformationOptionList =
//                            scalarIndexingStatementQueryInheritedData.getTransformationOptions();

                      // string returnString = exprListExp->unparseToString();
                      // returnString = "VAR_NAME[SC(" + returnString + ")]";

                      // We pass the variable name through the synthesised attribute from the
                      // function name I'm not sure if this is too subtle a trick. The alternative
                      // would be to build a special string in the synthesized attribute for this
                      // specific purpose (this might at least be more clear) and then delete the
                      // string once we were finished using it (within this case)).
                         string operandIdentifier  = synthesizedAttributeList[SgFunctionCallExp_function].getWorkSpace();
                         string argumentListString = synthesizedAttributeList[SgFunctionCallExp_args].getWorkSpace();

                         printf ("operandIdentifier = %s argumentListString = %s \n",
                              operandIdentifier.c_str(),argumentListString.c_str());

#if 0
                         printf ("Exiting after assignment of operandIdentifier ... \n");
                         ROSE_ABORT();
#endif

                         string variableNameString = operandIdentifier;
                         ArrayOperandDataBase arrayOperandDB =
                              accumulatorValue.operandDataBase.setVariableName(variableNameString);

                         string dataPointerName = "_" + operandIdentifier + "_pointer";

                         bool uniformSizeUnitStride =
                              ( scalarIndexingStatementQueryInheritedData.isAssertedByUser(TransformationAssertion::StrideOneAccess) ||
                                scalarIndexingStatementQueryInheritedData.isAssertedByUser(TransformationAssertion::ConstantStrideAccess) ) &&
                              scalarIndexingStatementQueryInheritedData.isAssertedByUser(TransformationAssertion::TransformationAssertion::SameSizeArrays);

#if 0
                         printf ("Exiting ... uniformSizeUnitStride = %s \n",uniformSizeUnitStride ? "true" : "false");
                         ROSE_ABORT();
#endif

                         string macroNameSubstring;
                         if (uniformSizeUnitStride == false)
                              macroNameSubstring = string("_") + operandIdentifier;

                         string subscriptMacroName = "SC" + macroNameSubstring;

                      // This is the returnString that will be pieced together to
                      // replace the targeted source code in the user's application
                         returnString = dataPointerName + "[" + subscriptMacroName + argumentListString + "]";

                      // If certain conditions hold then we can make the indexing more efficient
                      // (i.e. generate a simpler macro).

                         stringstream tempStringStream;
                         tempStringStream << expressionPtrList.size();
                         string numberOfFunctionArguments = tempStringStream.str();
                         string dataPointerString = string("Array_Descriptor.Array_View_Pointer") + 
                                                    numberOfFunctionArguments;

                      // Build a declaration for the new variable (and put it into the gathered data)
                      // string variableDeclarationString = "double* " + operandIdentifier + "_pointer = " + operandIdentifier + ".getDataPointer();";
                         string variableDeclarationString =
                              "double* _" + operandIdentifier + "_pointer = " + 
                              operandIdentifier + "." + dataPointerString + ";";

                         string defineSubscriptMacroString   = "\n#define SC(x1) x1 \n";
                         string undefineSubscriptMacroString = "\n#undef SC ";
                         ostringstream os;
                         static int counter = 0;
                         os << counter++;
                         undefineSubscriptMacroString += string("/* Instance: #") + os.str() + " */ \n";

                         printf ("defineSubscriptMacroString   = %s \n",defineSubscriptMacroString.c_str());
//                       printf ("undefineSubscriptMacroString = %s \n",undefineSubscriptMacroString.c_str());

                         string declarationString = variableDeclarationString;
                         if (firstOperandAlreadyProcessed == false)
                            {
#if 1
                              declarationString += defineSubscriptMacroString;
#else
// Don't use this yet
                              AST_Rewrite::addSourceCodeString(
                                   returnSynthesizedAttribute,
                                   defineSubscriptMacroString,
                                   scalarIndexingStatementQueryInheritedData,
                                   AST_Rewrite::NestedLoopScope,
                                   AST_Rewrite::BeforeCurrentPosition,
                                   AST_Rewrite::C_Preprocessor_Declaration,
                                   false);
#endif
                            }

                         printf ("declarationString = %s \n",declarationString.c_str());

#if 0
                      // Declare the array variables in the intermediate file (it is 
                      // not likely clear when the user would be required to do this).
                         AST_Rewrite::addSourceCodeString(
                              returnSynthesizedAttribute,
                              string("doubleArray ") + operandIdentifier + ";",
                              scalarIndexingStatementQueryInheritedData,
                           // NOTE: FunctionScope and PreamblePositionInScope generates an error
#if 1
                              AST_Rewrite::FunctionScope,
                              AST_Rewrite::PreamblePositionInScope,
#else
                              AST_Rewrite::Preamble,
                              AST_Rewrite::TopOfScope,
#endif
                              AST_Rewrite::TransformationString,
                              false);
#else
                         returnSynthesizedAttribute.insert (
                              functionCallExpression,
                              string("doubleArray ") + operandIdentifier + ";",
                              ScalarIndexingStatementQuerySynthesizedAttributeType::Preamble,
                              ScalarIndexingStatementQuerySynthesizedAttributeType::TopOfScope);
#endif

#if 0
                      // Put the declarations before the transformation target in the user's code
                         AST_Rewrite::AST_FragmentString declarationTransformationFragmentString =
                         AST_Rewrite::AST_FragmentString(declarationString,
                                                         scalarIndexingStatementQueryInheritedData,
                                                         AST_Rewrite::LocalScope,
                                                         AST_Rewrite::BeforeCurrentPosition);
                         returnSynthesizedAttribute.addSourceCodeString(declarationTransformationFragmentString);
#else
#if 0
                         AST_Rewrite::addSourceCodeString(
                              returnSynthesizedAttribute,
                              declarationString,
                              scalarIndexingStatementQueryInheritedData,
                              AST_Rewrite::NestedLoopScope,
                              AST_Rewrite::BeforeCurrentPosition,
                              AST_Rewrite::TransformationString,
                              false);
#else
                         returnSynthesizedAttribute.insert (
                              functionCallExpression,
                              declarationString,
                              ScalarIndexingStatementQuerySynthesizedAttributeType::NestedLoopScope,
                              ScalarIndexingStatementQuerySynthesizedAttributeType::BeforeCurrentPosition);
#endif
#endif

                         printf ("undefineSubscriptMacroString = %s \n",undefineSubscriptMacroString.c_str());

                         if (firstOperandAlreadyProcessed == false)
                            {
#if 0
                           // Put the "#undef" after the transformation
                              AST_Rewrite::AST_FragmentString undefineSubscriptMacroTransformationFragmentString =
                              AST_Rewrite::AST_FragmentString(undefineSubscriptMacroString,
                                                              scalarIndexingStatementQueryInheritedData,
                                                              AST_Rewrite::LocalScope,
                                                              AST_Rewrite::AfterCurrentPosition);
                              returnSynthesizedAttribute.addSourceCodeString(undefineSubscriptMacroTransformationFragmentString);
#else
#if 0
                              AST_Rewrite::addSourceCodeString(
                                   returnSynthesizedAttribute,
                                   "#undef SC ",
                                   scalarIndexingStatementQueryInheritedData,
                                   AST_Rewrite::LocalScope,
                                   AST_Rewrite::AfterCurrentPosition,
                                   AST_Rewrite::C_Preprocessor_Declaration,
                                   false);
#else
                              returnSynthesizedAttribute.insert (
                                   functionCallExpression,
                                   "#undef SC ",
                                   ScalarIndexingStatementQuerySynthesizedAttributeType::LocalScope,
                                   ScalarIndexingStatementQuerySynthesizedAttributeType::AfterCurrentPosition);
#endif
#endif
                            }

                      // Record that the first operand has been processed so that additional 
                      // operands will not generate redundent parts of the transformation if 
                      // the size and stride are the same across all operands (a user defined hint).
                         firstOperandAlreadyProcessed = true;

#if 0
                         printf ("Exiting after processing FUNC_CALL scalar operator(): returnString = %s \n",returnString.c_str());
                         ROSE_ABORT();
#endif
                         break;
                       }

                    case TransformationSupport::FUNCTION_CALL_OPERATOR_CODE:

                 // binary operators
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
                    case TransformationSupport::ASSIGN_OPERATOR_CODE:
                    case TransformationSupport::PLUS_ASSIGN_OPERATOR_CODE:
                    case TransformationSupport::MINUS_ASSIGN_OPERATOR_CODE:
                    case TransformationSupport::AND_ASSIGN_OPERATOR_CODE:
                    case TransformationSupport::IOR_ASSIGN_OPERATOR_CODE:
                    case TransformationSupport::MULT_ASSIGN_OPERATOR_CODE:
                    case TransformationSupport::DIV_ASSIGN_OPERATOR_CODE:
                    case TransformationSupport::MOD_ASSIGN_OPERATOR_CODE:
                    case TransformationSupport::XOR_ASSIGN_OPERATOR_CODE:
                    case TransformationSupport::BRACKET_OPERATOR_CODE:

                 // unary operators
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
                         break;

                    default:
                         printf ("default in switch found in variant = %d (NAME NOT IMPLEMENTED) (Scalar indexing transformation might be misused) \n",operatorCodeVariant);
                         ROSE_ABORT();
                  }

               printf ("Base of FUNC_CALL case: returnString = %s \n",returnString.c_str());
#if 0
               ROSE_ABORT();
#endif

            // returnSynthesizedAttribute.setTransformationString(returnString);

               ROSE_ASSERT (returnString.c_str() != NULL);
               break;
             }

       // These cases have been moved from the local function values
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
               break;
             }

       // Added VAR_REF case (moved from the local function)
          case VAR_REF:
             {
            // A VAR_REF has to output a string (the variable name)
               printf ("Found a variable reference expression \n");

            // Since we are at a leaf in the traversal of the AST this attribute list should a size of 0.
               ROSE_ASSERT (synthesizedAttributeList.size() == 0);

               SgVarRefExp* varRefExp = isSgVarRefExp(astNode);
               ROSE_ASSERT (varRefExp != NULL);
               SgVariableSymbol* variableSymbol = varRefExp->get_symbol();
               ROSE_ASSERT (variableSymbol != NULL);
               SgInitializedName* initializedName = variableSymbol->get_declaration();
               ROSE_ASSERT (initializedName != NULL);
               string variableName = initializedName->get_name().str();

            // return value for this case
               returnString = variableName;
            // returnSynthesizedAttribute.setTransformationString(returnString);

#if 1
               printf ("Exiting after building the transformation for VAR_REF (returnString = %s) (variableName = %s) ... \n",returnString.c_str(),variableName.c_str());
//             ROSE_ABORT();
#endif
               break;
             }

       // Removed VAR_REF case from below
          case RECORD_REF:  // SgDotExp
             {
            // A VAR_REF has to output a string (the variable name)
               printf ("Case SgDotExp: don't take the lhs string \n");

               printf ("synthesizedAttributeList[SgDotExp_lhs_operand_i].getWorkSpace() = %s \n",
                    synthesizedAttributeList[SgDotExp_lhs_operand_i].getWorkSpace().c_str());
               printf ("synthesizedAttributeList[SgDotExp_rhs_operand_i].getWorkSpace() = %s \n",
                    synthesizedAttributeList[SgDotExp_rhs_operand_i].getWorkSpace().c_str());

               returnString = synthesizedAttributeList[SgDotExp_lhs_operand_i].getWorkSpace();

            // returnSynthesizedAttribute.setTransformationString(returnString);

#if 0
               printf ("Exiting as a test in assembly in case SgDotExp: returnSynthesizedAttribute.getTransformationString() = %s \n",
                    returnSynthesizedAttribute.getTransformationString().c_str());
               ROSE_ABORT();
#endif
               break;
             }

       // case ValueExpTag:
          default:
             {
               printf ("DEFAULT ASSEMBLY: default case in scalarIndexingStatementQueryAssemblyFunction() (sage_class_name = %s \n",
                    astNode->sage_class_name());

            // The default is to concatinate all the strings together (in the order in which they appear)

               vector<ScalarIndexingStatementQuerySynthesizedAttributeType>::iterator i;
               for (i = synthesizedAttributeList.begin(); i != synthesizedAttributeList.end(); i++)
                  {
#if 1
                    printf ("In the DEFAULT CASE: SynthesizedAttributeListElements: attributeList = %s \n",
                         (*i).getWorkSpace().c_str());
#endif
                 // Call the ScalarIndexingStatementQuerySynthesizedAttributeType::operator+= overloaded operator
                 // returnSynthesizedAttribute += *i;
                    returnString += (*i).getWorkSpace();
                  }

            // This is bizzare since we are copying redundently back and forth
            // between the synthesised attribute and the return string.
//             returnString = returnSynthesizedAttribute.getWorkSpace();

            // ROSE_ABORT();
               break;
             }
        }

#if 0
  // Now put the new string into the synthesised attribute that we will return
     if (returnString.length() > 0)
       {
          printf ("BOTTOM of scalarIndexingStatementQueryAssemblyFunction: returnString = %s \n",returnString.c_str());

       // Build a locatable string fragement for automated insertion into the AST
          AST_Rewrite::AST_FragmentString mainTransformationFragmentString =
               AST_Rewrite::AST_FragmentString(returnString,
                                               scalarIndexingStatementQueryInheritedData,
                                               AST_Rewrite::LocalScope,
                                               AST_Rewrite::ReplaceCurrentPosition);
          returnSynthesizedAttribute.addSourceCodeString(mainTransformationFragmentString);

       // printf ("BOTTOM of scalarIndexingStatementQueryAssemblyFunction: returnSynthesizedAttribute = %s \n",
       //      returnSynthesizedAttribute.getWorkSpace().c_str());
        }
#endif

     returnSynthesizedAttribute.setWorkSpace(returnString);

#if 1
     printf ("$$$$$ BOTTOM of scalarIndexingStatementQueryAssemblyFunction (astNode = %s) (returnSynthesizedAttribute.getWorkSpace() = %s) \n",
          astNode->sage_class_name(),
          returnSynthesizedAttribute.getWorkSpace().c_str());
     returnSynthesizedAttribute.display("BOTTOM of scalarIndexingStatementQueryAssemblyFunction");
//   printf ("accumulatorValue.operandDataBase.displayString() = %s \n",
//        accumulatorValue.operandDataBase.displayString().c_str());
#endif

     return returnSynthesizedAttribute;
   }









