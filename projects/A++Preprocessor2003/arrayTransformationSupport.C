// Treat config.h separately from other include files
#ifdef HAVE_CONFIG_H
// This avoids requiring the user to use config.h and follows
// the automake manual request that we use <> instead of ""
#include <config.h>
#endif

#include "arrayPreprocessor.h"

// Not certain this is still required
extern SgProject* globalProject;

ArrayTransformationSupport::IndexingAccessEnumType
ArrayTransformationSupport::interpretTransformationOptions (
     const list<int> & transformationOptionList )
   {
  // This function sets a collection of input boolean values
  // A better imlementation might use a set of enum value to index
  // a more general input array of boolean values.

  // Implementation Note: this implementation could be much simpler if the transformationOptionList
  // values where bit patterns that could be OR'ed together to form the return value of this
  // function.

  // Implementation Note: We might at some point want to return an object from this function
  // instaead of an enum. An object could contain more information (such as the dimension of the
  // array operation, etc. (which would be awkward to encode into the enum return value).

  // Implementation Note: We should share a header file between the preprocessor source code and the
  // A++ library source code so that we can use the same values (or enum type) for the different
  // transformation options.

  // declaration of function return value
     IndexingAccessEnumType returnValue = UnknownIndexingAccess;

#if 0
  // This would be a better implementation
     list<int>::const_iterator transformationOptionListIterator;
     for (transformationOptionListIterator  = transformationOptionList.begin(); 
          transformationOptionListIterator != transformationOptionList.end();
          transformationOptionListIterator++)
        {
       // display each value
          printf ("     Value in transformation option = %d \n",(*transformationOptionListIterator));
          if (*transformationOptionListIterator & SET_BIT)
             {
               returnValue |= *transformationOptionListIterator;
             }
            else
             {
               ROSE_ASSERT (*transformationOptionListIterator & UNSET_BIT);
               returnValue |= *transformationOptionListIterator;
             }
        }
#else
  // most defaults are TRUE to permit the most general cases
     bool whereStatementTransformationRequired       = TRUE;
     bool indirectAddressingTransformationRequired   = TRUE;
     bool variableStrideTransformationRequired       = TRUE;
     bool strideOneAccessTransformation              = FALSE;  // default is FALSE to permit most general case
     bool differentSizeTransformationRequired        = TRUE;
     bool differentIndexObjectTransformationRequired = TRUE;
     bool differentPartitionTransformationRequired   = TRUE;

  // list<TransformationAssertion::TransformationOption>::const_iterator transformationOptionListIterator;
     list<int>::const_iterator transformationOptionListIterator;
     for (transformationOptionListIterator  = transformationOptionList.begin(); 
          transformationOptionListIterator != transformationOptionList.end();
          transformationOptionListIterator++)
        {
       // display each value
       // TransformationAssertion::TransformationOption i = *transformationOptionListIterator;
       // int i = int(*transformationOptionListIterator);
       // printf ("     Value in transformation option = %d name = %s \n",i,TransformationAssertion::getOptionString(i));

          int listValue = int(*transformationOptionListIterator);

       // figure out what options are being specified
          switch (listValue)
             {
               case  1:
                    whereStatementTransformationRequired     = FALSE;
                    break;
               case  2:
                    whereStatementTransformationRequired     = TRUE;
                    break;
               case  3:
                    indirectAddressingTransformationRequired = FALSE;
                    break;
               case  4:
                    indirectAddressingTransformationRequired = TRUE;
                    break;
               case  5:
                    variableStrideTransformationRequired     = FALSE;
                    break;
               case  6:
                    strideOneAccessTransformation            = TRUE;
                    variableStrideTransformationRequired     = FALSE;
                    break;
               case  7:
                    variableStrideTransformationRequired     = TRUE;
                    strideOneAccessTransformation            = FALSE;
                    break;
               case  8:
                    differentSizeTransformationRequired      = FALSE;
                    break;
               case  9:
                    differentSizeTransformationRequired      = TRUE;
                    break;
               case 10:
                    differentPartitionTransformationRequired = FALSE;
                    break;
               case 11:
                    differentPartitionTransformationRequired = TRUE;
                    break;
               case 12:
                    differentIndexObjectTransformationRequired = FALSE;
                    break;
               default:
                    printf ("default reached in switch for listValue (transformationOptions) \n");
                    ROSE_ABORT();
             }
        }

  // Select different subscript mechanisms to optimize the performance (the
  // alternative would be to implement transformations that take care of all possible
  // optimizations, but this would lead to long compile times (so the selection of
  // transformation options can be seen either as increasing performance or
  // decreasing the compile time processing of the unparsed (optimized) output from
  // preprocessors built using ROSE).
     if (differentSizeTransformationRequired == TRUE)
        {
       // Transformation requires separate size variables for each operand's subscript computation
       // (this could be make more specific later to specify specific variables which require separate
       // size variables)
       // ROSE_ASSERT (strideOneAccessTransformation == FALSE);
          if (variableStrideTransformationRequired == TRUE)
             {
            // Transformation requires separate stride information for each operand
               returnValue = VariableSizeVariableStride;
               ROSE_ASSERT (strideOneAccessTransformation == FALSE);
             }
            else
             {
               if (strideOneAccessTransformation == TRUE)
                  {
                  // Transformation requires a single stride variable since it can't be assume to
                  // have the value == 1 for each operand
                     returnValue = VariableSizeUnitStride;
                  }
                 else
                  {
                 // Transformation can use the same stride variable for each operand
                    returnValue = VariableSizeUniformStride;
                  }
             }
        }
       else
        {
       // Transformation can use a single size variable for the subscript computation of each operand
          if (variableStrideTransformationRequired == TRUE)
             {
             // Transformation requires separate stride information for each operand
                returnValue = UniformSizeVariableStride;
                ROSE_ASSERT (strideOneAccessTransformation == FALSE);
             }
            else
             {
               if (strideOneAccessTransformation == TRUE)
                  {
                 // This is the fastest possible code for array objects using index objects
                 // Transformation can be optimized for stride one access of each operand
                 // This is the fastest possible code for array objects not using index objects
                    returnValue = UniformSizeUnitStride;
                  }
                 else
                  {
                  // Transformation requires a single stride variable since it can't be assume to
                  // have the value == 1 for each operand
                     returnValue = UniformSizeUniformStride;
                  }
             }
        }
#endif

#if 0
     printf ("In ArrayTransformationSupport::interpretTransformationOptions(): transformationOptionList.size() = %d returnValue = %d \n",
          transformationOptionList.size(),returnValue);
#endif

#if 0
     printf ("Exiting at base of ArrayTransformationSupport::interpretTransformationOptions() \n");
     ROSE_ABORT();
#endif

     return returnValue;
   }

ArrayTransformationSupport::IndexingAccessEnumType
ArrayTransformationSupport::typeOfIndexingAccessAcrossAllOperands ( list<int> operandCodeList, int dimension )
   {
  // The dimension parameter will be used later to permit aggrigation of optimizations for
  // dimensions that ate indexed using scalars. Not implemented yet.

     IndexingAccessEnumType returnValue = UnknownIndexingAccess;

#if 0
     switch (typeOfIndexingAccess)
        {
          case UnknownIndexingAccess:
               printf ("Error case UnknownIndexingAccess \n");
               ROSE_ABORT();
               break;

          case UniformSizeUnitStride:

          case UniformSizeUniformStride:

          case UniformSizeVariableStride:
          case VariableSizeUnitStride:
          case VariableSizeUniformStride:
          case VariableSizeVariableStride:
          case scalarIndexingAccess:
               printf ("cases of indexing access not yet supported in subscript macro construction (indexing access) \n");
               ROSE_ABORT();
               break;

          default:
               printf ("default reached in subscript macro construction: typeOfIndexingAccessAcrossAllOperands()\n");
               ROSE_ABORT();
               break;
        }

     returnValue = *(operandCodeList.begin());
#else
  // Currently the values in the list are set explicitly to all be the same
  // so we can reduce the list to this unique value and return it as the 
  // return value for this function.
     operandCodeList.sort();
     operandCodeList.unique();
     ROSE_ASSERT (operandCodeList.size() == 1);
  // returnValue = static_cast<IndexingAccessEnumType> *(operandCodeList.begin());
     returnValue = (IndexingAccessEnumType) *(operandCodeList.begin());

  // This is the value that we are testing presently
     ROSE_ASSERT (returnValue == UniformSizeUniformStride);
#endif

     return returnValue;
   }

string
ArrayTransformationSupport::buildLoopNest ( const OperandDataBaseType & operandDataBase, int dimensionOfArrayStatement )
   {
  // This function constructs the loop nest.  At the moment we don't optimize for scalar indexing
  // where the generation of "for" loops for a particular dimension could be avoided. To allow for
  // this optimization we input the operandCodeList (indexing data for each operand). At a later
  // point in the debugging this list will be used to recognize where scalar indexing is used and to
  // generate index initialization statements instead of for loop statements (e.g. "index3 = 1;").
  // To support the multiple statements that this optimization will generate, we presently include
  // "{}" explicitly for each for loop scope. So the generation of the loop nest for any dimension
  // requires the generation of the "for statement" plus an opending brace AND the generation of the
  // closing braces.

  // TODO: At some point we should try to get the indentation correct for each loop of the loop nest.

  // make sure that we have a list with something in it
  // ROSE_ASSERT (operandCodeList.size() > 0);

  // Start of loop nest string
     char* loopNestTemplate = "\
// \"for\" loop for $dimD array operations with stride \n\
";

     ROSE_ASSERT (dimensionOfArrayStatement > 0);

  // Make sure that the return string is allocated on the heap 
  // (use the rose::stringDuplicate function to force this)
     char* loopNestString = rose::stringDuplicate (loopNestTemplate);

     bool consistantAcrossAllOperands = TRUE;     // consistantIndexingAcrossAllOperands (operandCodeList);

  // bool unitStride                         = FALSE; // unitStrideIndexingAcrossAllOperands (operandCodeList);
  // IndexingAccessType typeOfIndexingAccess = typeOfIndexingAccessAcrossAllOperands (operandCodeList);
  // Test the simple case for now!
  // IndexingAccessType typeOfIndexingAccess = ArrayTransformationSupport::unitStride;

  // Generate the for statement and the opening brace
  // count down from dimensionOfArrayStatement to 1
     int i = 0;
     for (i = dimensionOfArrayStatement; i > 0; i--)
        {
       // Currently we don't treat the indexing of individual operands separately
       // IndexingAccessEnumType typeOfIndexingAccess = typeOfIndexingAccessAcrossAllOperands (operandCodeList,i);
          IndexingAccessEnumType typeOfIndexingAccess = operandDataBase.globalIndexingProperties();

          char* staticLocalLoopNestTemplate = NULL;
          if (consistantAcrossAllOperands == TRUE)
             {
            // Incrementing the "for" loop by the stride is possible if all operands have the same stride access
            // numerous other optimizations are possible if the indexing is consistant across all operands
            // Note: code needs to be introduced optionally to verify such assumptions
               switch (typeOfIndexingAccess)
                  {
                    case UnknownIndexingAccess:
                         printf ("Error case UnknownIndexingAccess \n");
                         ROSE_ABORT();
                         break;

                    case UniformSizeUnitStride:
                      // If we have unit stride then we want ti increment by 1 and the subscript
                      // computation will be simple
                         staticLocalLoopNestTemplate = "   for (_$dim = 0; _$dim < _length$dim; _$dim++) { \n";
                         break;

                    case UniformSizeUniformStride:
                      // Strength reduction that the backend compiler can't seem to do (non trivial strength reduction)
                      // If we have a constant stride (across all operands) that is not 1 then this
                      // permits the subscript computation to be as simple as possible
                         staticLocalLoopNestTemplate = "   for (_$dim = 0; _$dim < _length$dim; _$dim+=_stride$dim) { \n";
                         break;

                    case scalarIndexingAccess:
                         staticLocalLoopNestTemplate = "   _$dim = $ScalarIndexValue$dim; { \n";
                         break;

                    case UniformSizeVariableStride:
                    case VariableSizeUnitStride:
                    case VariableSizeUniformStride:
                    case VariableSizeVariableStride:
                         staticLocalLoopNestTemplate = "   for (_$dim = 0; _$dim < _length$dim; _$dim++) { \n";
                      // printf ("cases of indexing access not yet supported in subscript macro construction (loop nest) \n");
                      // ROSE_ABORT();
                         break;

                    default:
                         printf ("Error: default reached in switch statement! \n");
                         ROSE_ABORT();
                         break;
                  }
             }
            else
             {
            // If the stride is different between operands across the statement then we have to
            // increment by 1 and we are forced to have a more complex subscript computation
            // (containing a multiplication by stride) to make up for the strides being different.
            // Note: subscript generation is handled separately from the loop nest generation.
               staticLocalLoopNestTemplate = "   for (_$dim = 0; _$dim < length$dim; _$dim++) { \n";
             }
          ROSE_ASSERT (staticLocalLoopNestTemplate != NULL);

       // Build the string on the heap (since the copyEdit function requires that)
          char* localLoopNestTemplate = rose::stringDuplicate(staticLocalLoopNestTemplate);
          ROSE_ASSERT (localLoopNestTemplate != NULL);

       // char staticNumberString[128];
       // char* numberString = lltostr(i,staticNumberString+128);
          char numberString[128];
          sprintf (numberString,"%d",i);
          char* tempLoopNestString =
       //      TransformationSpecificationType::copyEdit ( localLoopNestTemplate, "$dim", numberString );
               SgNode::copyEdit ( localLoopNestTemplate, "$dim", numberString );
          loopNestString = rose::stringConcatinate (loopNestString,tempLoopNestString);
        }

  // Add a target string to support the substitution of the inner loop body later
     loopNestString = rose::stringConcatinate (loopNestString,"$INNER_LOOP; \n");

  // Generate the closing braces
     for (i=0; i < dimensionOfArrayStatement; i++)
        {
          char* localLoopNestTemplate = "      } \n";
          loopNestString              = rose::stringConcatinate (loopNestString,localLoopNestTemplate);
        }

     ROSE_ASSERT (loopNestString != NULL);

  // display the template for any processing
  // printf ("loopNestString for transformation = \n%s \n",loopNestString);

#if 0
     printf ("Exiting at base of ArrayTransformationSupport::buildLoopNest() \n");
     ROSE_ABORT();
#endif

     return loopNestString;
   }

string
ArrayTransformationSupport::buildOperandSpecificGlobalDeclarations (
     const ArrayStatementQueryInheritedAttributeType & inheritedAttribute,
     const OperandDataBaseType & operandDataBase )
   {
  // This function generates the global declarations that are required for the support of the
  // transformation. 

  // error checking for the data base
     ROSE_ASSERT (operandDataBase.size() > 0);

     ROSE_ASSERT (inheritedAttribute.arrayStatementDimensionDefined == TRUE);
  // int arrayDimension = inheritedAttribute.arrayStatementDimension;

  // String representing inclusion of header file
     string includeHeaderString = "\
// Include macros required by transformations (uses include guards) \n\
#include \"transformationMacros.h\" \n\n";

  // Generate a string resembling:
  // extern int A_I_J;
  // int SC_A_I_J(int,int);
  // int SC_B(int,int);

  // We have to build variables and functions that will permit the compilation of the
  // transformations that we specify.  This aspect of supporting the specification of
  // transformations via strings is a bit tedious.
     string operandDataTemplate = "\
\n// Supporting declaration for input to macros (seen as functions within intermediate step of transformations) \n\
\n// prototype matching macro declaration (so that use of macro will appear as a function and not be expanded during code generation) \n\
\nint SC$VARIABLE_IDENTIFIER($INTEGER_LIST); \n\
";

#if 0
\nint $VARIABLE_IDENTIFIER; \n\
\nint $VARIABLE_NAME; \n\

#endif

     string integerList = "int";
     int arrayOperandDimension = operandDataBase.getDimension();
     int n;
     for ( n = 1; n < arrayOperandDimension; n++ )
        {
       // Generate a string to represent the parameter list of the function we will build to
       // represent the macro that we use in the subscript computation of the specification of
       // the transformation.
          integerList += ",int";
        }

     string variableData;

     vector<ArrayOperandDataBase>::const_iterator i;
     for ( i = operandDataBase.arrayOperandList.begin();
           i != operandDataBase.arrayOperandList.end(); i++)
        {
       // printf ("At TOP of loop over the operandNameStringList \n");
          ROSE_ASSERT (operandDataTemplate.c_str() != NULL);

       // use a local variable to point to the string we will modify/build
          string localOperandDataTemplate = operandDataTemplate;

#if 0
          string operandName       = string("_") + (*i).arrayVariableName;
       // Size variables only need the _$VARIABLE_NAME prefix while stride variables can need the
       // $VARIABLE_IDENTIFIER prefix.  As a result we need to declare both inorder to compile the
       // final transformation.
          localOperandDataTemplate = SgNode::copyEdit
               ( localOperandDataTemplate.c_str(), "$VARIABLE_NAME", operandName.c_str() );
#endif

       // printf ("Test 1: localOperandDataTemplate    = %s \n",localOperandDataTemplate.c_str());

#if 1
       // Get the identifier from the database
          string operandIdentifier = (*i).generateIdentifierString();

          localOperandDataTemplate = SgNode::copyEdit
               ( localOperandDataTemplate.c_str(), "$VARIABLE_IDENTIFIER", operandIdentifier.c_str() );
#endif

       // printf ("Test 2: localOperandDataTemplate    = %s \n",localOperandDataTemplate.c_str());

          localOperandDataTemplate =
               SgNode::copyEdit ( localOperandDataTemplate.c_str(),
                                  "$INTEGER_LIST",
                                  integerList.c_str() );

       // printf ("Test 3: localOperandDataTemplate    = %s \n",localOperandDataTemplate.c_str());

       // Build of the final string by concatination of the string build within each iteration of the loop
          variableData += localOperandDataTemplate;

#if 0
          printf ("Exiting at bottom of loop in ArrayTransformationSupport::buildOperandSpecificGlobalDeclarations() ... \n");
          printf ("variableData = \n%s\n",variableData.c_str());
          ROSE_ABORT();
#endif
        }


  // Convert the string into a list of strings and separate out the redundent entries
  // (this allows us to handle the case of "A=A", "A(I)=A(I+1)" etc.)
     variableData = StringUtility::removeRedundentSubstrings(variableData);

#if 0
     printf ("variableData = \n%s\n",variableData.c_str());
#endif

  // Build the final return string (include the header preamble)
     string returnString = includeHeaderString + variableData;

#if 0
     printf ("In ArrayTransformationSupport::buildOperandSpecificGlobalDeclarations(): returnString = \n%s\n",returnString.c_str());
#endif

#if 0
     printf ("Exiting at the base of ArrayTransformationSupport::buildOperandSpecificGlobalDeclarations() ... \n");
     ROSE_ABORT();
#endif

     return returnString;
   }

string
ArrayTransformationSupport::buildOperandSpecificVariableDeclarations (
     const ArrayStatementQueryInheritedAttributeType & inheritedAttribute,
     const OperandDataBaseType & operandDataBase )
   {

  // Temp code to test compilation
  // list<int> operandCodeList;          // uncondensed list
  // list<string> operandNameStringList; // uncondensed list

  // This function generates the variable declarations that are required for the support of the
  // transformation. 

  // Introduction to generation of macro within transformations An interesting feature is that is
  // shows how to generate a macro definition within the final transformation (currently a bit of a
  // hack).  The trick is that we generate the macro definition as a string literal expression
  // within an expression statement (e.g. 'RoseTransformationMacro:#define xyz zyx;') and we modify
  // the unparser (temporarily) to permit the recognition of the substring
  // "RoseTransformationMacro:" and if found turn the rest of the string into an UNQUOTED string as
  // output (allowing the final output to be interpreted as a macro if it began with "#define", or
  // and raw text in the source file which would likely generate an error).  Note that the tailing
  // ";" is placed on the next line so that it is a null statement and the macro definition can be
  // represented without a trailing ";".  This is not intended as a way to introduce source code and
  // is a temporary hack to allow the construction of an elegant transformations associated with the
  // A++/P++ preprocessor.  This hack will disappear once we have defined comments (and perhaps
  // macros) within the AST.

     ROSE_ASSERT (operandDataBase.size() > 0);

  // This is should be true since only the uncondensed lists are used as input
  // ROSE_ASSERT (operandNameStringList.size() == operandCodeList.size());

     ROSE_ASSERT (inheritedAttribute.arrayStatementDimensionDefined == TRUE);
     int arrayDimension = inheritedAttribute.arrayStatementDimension;

  // Note that we have to declare the $VARIABLE_NAME_pointer since it is referenced in the innerloop
  // transformation Note also that the subscript declaration is implemented as a string literal
  // expression statement.
     string staticOperandDataTemplate = "\
   $ARRAY_OPERAND_TRANSFORMATION_DATA_MACRO_STRING\n\
   double* APP_RESTRICT $VARIABLE_IDENTIFIER_pointer = NULL;\n\
";

  // Initialize the base and bound of each loop
     string variableData = "";

     int counter = 0;
     vector<ArrayOperandDataBase>::const_iterator i;
     for (i = operandDataBase.arrayOperandList.begin(); i != operandDataBase.arrayOperandList.end(); i++)
        {
       // printf ("Counter = %d (*i).getIndexingAccessCode() = %d \n",counter,(*i).getIndexingAccessCode());
          ROSE_ASSERT ( (*i).getIndexingAccessCode() != ArrayTransformationSupport::UnknownIndexingAccess );
        }

     counter = 0;
     for (i = operandDataBase.arrayOperandList.begin(); i != operandDataBase.arrayOperandList.end(); i++)
        {
       // printf ("At TOP of loop over the operandNameStringList \n");

          string operandDataTemplate = staticOperandDataTemplate;
          ROSE_ASSERT (operandDataTemplate.c_str() != NULL);

       // use a local variable to point to the string we will modify/build
          string localOperandDataTemplate = operandDataTemplate;

          ROSE_ASSERT (localOperandDataTemplate.c_str() != NULL);
       // printf ("Test -1: localOperandDataTemplate    = %s \n",localOperandDataTemplate);

       // process each string representing a unique variable name
       // printf ("     Unique variable names in operandNameStringList = %s \n",(*listStringElementIterator).c_str());
       // printf ("Test 1: localOperandDataTemplate    = %s \n",localOperandDataTemplate);

          string operandDataSetupMacroString = buildOperandVariableDeclarationMacro ( *i , counter );

       // printf ("operandDataSetupMacroString = %s \n",operandDataSetupMacroString.c_str());

#if 0
          printf ("Exiting at middle of loop in ArrayTransformationSupport::buildOperandsSpecificData() ... \n");
          printf ("variableData = \n%s\n",variableData.c_str());
          ROSE_ABORT();
#endif

          localOperandDataTemplate =
       //      TransformationSpecificationType::copyEdit
               SgNode::copyEdit
                  ( localOperandDataTemplate.c_str(), "$ARRAY_OPERAND_TRANSFORMATION_DATA_MACRO_STRING", operandDataSetupMacroString.c_str() );

       // printf ("Test 2: localOperandDataTemplate    = %s \n",localOperandDataTemplate);
#if 0
          string subscriptComputationMacroString =
               buildOperandSubscriptMacro (
                    inheritedAttribute, localOperandCode, (*listStringElementIterator), counter );
       // printf ("subscriptComputationMacroString = %s \n",subscriptComputationMacroString);

          localOperandDataTemplate =
               SgNode::copyEdit ( localOperandDataTemplate.c_str(),
                                  "$SUBSCRIPT_COMPUTATION_MACRO",
                                  subscriptComputationMacroString.c_str() );

       // printf ("Test 3: localOperandDataTemplate    = %s \n",localOperandDataTemplate);
#endif

       // string variableIdentifier              = (*listStringElementIterator);
       // string indexIdentifierString           = (*i).generateIndexIdentifierString();
          string variableName                    = (*i).arrayVariableName;
          string indexVariableArgumentList       = (*i).generateIndexVariableArgumentList();
          string uniqueIdentifierString          = (*i).generateIdentifierString();
          string uniqueIdentifierWithIndexString = string("_") + (*i).arrayVariableName;

          localOperandDataTemplate =
               SgNode::copyEdit ( localOperandDataTemplate.c_str(),
                                  "$VARIABLE_WITH_INDEX_IDENTIFIER",
                                  uniqueIdentifierWithIndexString.c_str() );

          localOperandDataTemplate =
               SgNode::copyEdit ( localOperandDataTemplate.c_str(),
                                  "$VARIABLE_IDENTIFIER",
                                  uniqueIdentifierString.c_str() );

          localOperandDataTemplate =
               SgNode::copyEdit ( localOperandDataTemplate.c_str(),
                                  "$VARIABLE_NAME",
                                  variableName.c_str() );

          localOperandDataTemplate =
               SgNode::copyEdit ( localOperandDataTemplate.c_str(),
                                  "$INDEX_SET",
                                  indexVariableArgumentList.c_str() );

       // printf ("Test 4: localOperandDataTemplate    = %s \n",localOperandDataTemplate);

          localOperandDataTemplate =
               SgNode::copyEdit ( localOperandDataTemplate.c_str(), "$dim",
                                  StringUtility::numberToString(arrayDimension) );

       // printf ("Test 5: localOperandDataTemplate    = %s \n",localOperandDataTemplate);

       // Build of the final string by concatination of the string build within each iteration of the loop
       // variableData = rose::stringConcatinate (variableData,localOperandDataTemplate);
          variableData += localOperandDataTemplate;

       // Increment the operand code to iterate with the listStringElementIterator
       // listOperandCodeElementIterator++;
          counter++;

#if 0
          printf ("Exiting at bottom of loop in ArrayTransformationSupport::buildOperandSpecificVariableDeclarations() ... \n");
          printf ("variableData = \n%s\n",variableData.c_str());
          ROSE_ABORT();
#endif
        }

  // The selection of what loop nest variables to declare depends upon the global properties of the
  // indexing used across the operands in the array statement.
     string loopVariablesMacro;
  // IndexingAccessEnumType typeOfIndexingAccess = typeOfIndexingAccessAcrossAllOperands (operandCodeList);
     IndexingAccessEnumType typeOfIndexingAccess = operandDataBase.globalIndexingProperties();
     switch (typeOfIndexingAccess)
        {
          case UnknownIndexingAccess:
               printf ("Error case UnknownIndexingAccess \n");
               ROSE_ABORT();
               break;

          case scalarIndexingAccess:
               printf ("Error case scalarIndexingAccess \n");
               ROSE_ABORT();
               break;

       // This is the default loop variable declarations (forcing most of the details into the
       // subscript computation).
          case UniformSizeVariableStride:
          case VariableSizeUnitStride:
          case VariableSizeVariableStride:
          case UniformSizeUnitStride:
               loopVariablesMacro = "LOOP_INDEX_VARIABLES_DECLARATION_MACRO_D$dim();\n";
               break;

       // Uniform stride permits a strength reduction optimization which increments the stride and
       // avoids a strinde variable within the subscript computation.
          case UniformSizeUniformStride:
          case VariableSizeUniformStride:
            // loopVariablesMacro = "LOOP_INDEX_VARIABLES_UNIFORM_STRIDE_DECLARATION_MACRO_D$dim();\n";
               loopVariablesMacro = "LOOP_INDEX_VARIABLES_DECLARATION_MACRO_D$dim();\n";

            // This loop nest requires the definition of variables that will be redundently defined
            // by the macros that are built for the variable declarations (so we just use the
            // regular macro (LOOP_INDEX_VARIABLES_DECLARATION_MACRO_D$dim()) and declare these
            // extra stride variables here to be extern).
               loopVariablesMacro += "extern int _stride1,_stride2,_stride3,_stride4,_stride5,_stride6;\n";
               break;

          default:
               printf ("Error: default reached in switch statement! \n");
               ROSE_ABORT();
               break;
        }

  // Add the macro that builds the loop variables at the end
     variableData += loopVariablesMacro;

  // Get the dimensionality of the array statement
     ROSE_ASSERT (inheritedAttribute.arrayStatementDimensionDefined == TRUE);
     int dimensionOfArrayStatement = inheritedAttribute.arrayStatementDimension;

  // Build a string representing the dimensionOfArrayStatement
  // string numberString = StringUtility::numberToString(dimensionOfArrayStatement);
  // returnString = SgNode::copyEdit (returnString,"$dim",numberString);
     variableData = SgNode::copyEdit (variableData,"$dim",StringUtility::numberToString(dimensionOfArrayStatement));
     ROSE_ASSERT (variableData.c_str() != NULL);

  // printf ("Before removing redundent declarations: variableData = \n%s \n",variableData.c_str());

  // Convert the string into a list of strings and separate out the redundent entries
  // (this allows us to handle the case of "A=A", "A(I)=A(I+1)" etc.)
     variableData = StringUtility::removeRedundentSubstrings(variableData);

#if 0
     printf ("After removing redundent declarations: variableData = \n%s\n",variableData.c_str());
     printf ("Exiting at the base of ArrayTransformationSupport::buildOperandSpecificVariableDeclarations() ... \n");
     ROSE_ABORT();
#endif

     return variableData;
  // return StringUtility::stringToList(variableData);
   }

string
ArrayTransformationSupport::buildOperandSpecificVariableInitialization (
     const ArrayStatementQueryInheritedAttributeType & inheritedAttribute,
     const OperandDataBaseType & operandDataBase )
   {
  // This function generates the variable initializations that are required for the support of the
  // transformation. See ArrayTransformationSupport::buildOperandsSpecificVariableDeclaration()
  // member function (above).

     ROSE_ASSERT (inheritedAttribute.arrayStatementDimensionDefined == TRUE);
     int arrayDimension = inheritedAttribute.arrayStatementDimension;

  // Note that we have to declare the $VARIABLE_NAME_pointer since it is referenced in the innerloop
  // transformation Note also that the subscript declaration is implemented as a string literal
  // expression statement.
     string staticOperandDataTemplate = "\
   $ARRAY_OPERAND_TRANSFORMATION_VARIABLE_INITIALIZATION_MACRO_STRING \n\
   $VARIABLE_IDENTIFIER_pointer = $VARIABLE_NAME.getAdjustedDataPointer($INDEX_SET); \n\
   $SUBSCRIPT_COMPUTATION_MACRO \n\
";

  // Initialize the base and bound of each loop
     string variableData = "";

     int counter = 0;
     vector<ArrayOperandDataBase>::const_iterator i;
     for (i = operandDataBase.arrayOperandList.begin(); i != operandDataBase.arrayOperandList.end(); i++)
        {
       // printf ("At TOP of loop over the operandNameStringList \n");

          string operandDataTemplate = staticOperandDataTemplate;
          ROSE_ASSERT (operandDataTemplate.c_str() != NULL);

       // use a local variable to point to the string we will modify/build
          string localOperandDataTemplate = operandDataTemplate;

          ROSE_ASSERT (localOperandDataTemplate.c_str() != NULL);

          string operandDataSetupMacroString = buildOperandVariableInitializationMacro ( *i , counter );

       // printf ("operandDataSetupMacroString = %s \n",operandDataSetupMacroString.c_str());

          localOperandDataTemplate =
               SgNode::copyEdit ( localOperandDataTemplate.c_str(),
                                  "$ARRAY_OPERAND_TRANSFORMATION_VARIABLE_INITIALIZATION_MACRO_STRING",
                                  operandDataSetupMacroString.c_str() );
       // printf ("Test 2: localOperandDataTemplate    = %s \n",localOperandDataTemplate.c_str());

          string subscriptComputationMacroString = buildOperandSubscriptMacro ( *i, counter );

       // printf ("subscriptComputationMacroString = %s \n",subscriptComputationMacroString.c_str());

          localOperandDataTemplate =
               SgNode::copyEdit ( localOperandDataTemplate.c_str(),
                                  "$SUBSCRIPT_COMPUTATION_MACRO",
                                  subscriptComputationMacroString.c_str() );

       // printf ("Test 3: localOperandDataTemplate    = %s \n",localOperandDataTemplate.c_str());

          string variableName              = (*i).arrayVariableName;
          string indexVariableArgumentList = (*i).generateIndexVariableArgumentList();
          string uniqueIdentifierString    = (*i).generateIdentifierString();

          localOperandDataTemplate =
               SgNode::copyEdit ( localOperandDataTemplate.c_str(),
                                  "$VARIABLE_IDENTIFIER",
                                  uniqueIdentifierString.c_str() );

          localOperandDataTemplate =
               SgNode::copyEdit ( localOperandDataTemplate.c_str(),
                                  "$VARIABLE_NAME",
                                  variableName.c_str() );

          localOperandDataTemplate =
               SgNode::copyEdit ( localOperandDataTemplate.c_str(),
                                  "$INDEX_SET",
                                  indexVariableArgumentList.c_str() );

       // printf ("Test 4: localOperandDataTemplate    = %s \n",localOperandDataTemplate.c_str());

          localOperandDataTemplate =
               SgNode::copyEdit ( localOperandDataTemplate.c_str(), "$dim",
                                  StringUtility::numberToString(arrayDimension) );

       // printf ("Test 5: localOperandDataTemplate    = %s \n",localOperandDataTemplate.c_str());

       // Build of the final string by concatination of the string build within each iteration of the loop
       // variableData = rose::stringConcatinate (variableData,localOperandDataTemplate);
          variableData += localOperandDataTemplate;

       // Increment the operand code to iterate with the listStringElementIterator
       // listOperandCodeElementIterator++;
          counter++;

#if 0
          printf ("Exiting at bottom of loop in ArrayTransformationSupport::buildOperandSpecificVariableInitialization() ... \n");
          printf ("variableData = \n%s\n",variableData.c_str());
          ROSE_ABORT();
#endif
        }

  // Remove any redundency in the initializations (since all the operands
  // are the same dimension there are no pseudo-redundant strings)
  // printf ("BEFORE removePseudoRedundentSubstrings(): variableData = \n%s \n",variableData.c_str());
     variableData = StringUtility::removePseudoRedundentSubstrings(variableData);

#if 0
     printf ("AFTER removePseudoRedundentSubstrings(): variableData = \n%s\n",variableData.c_str());
     printf ("Exiting at the base of ArrayTransformationSupport::buildOperandSpecificVariableInitialization() ... \n");
     ROSE_ABORT();
#endif

     return variableData;
   }

string
ArrayTransformationSupport::buildOperandVariableDeclarationMacro (
     const ArrayOperandDataBase & arrayOperandDataBase,
     int operandIndex )
   {
  // We don't need the variableName since we return $VARIABLE_NAME instead which is substituted by
  // the calling function this may be a poor design however.
  // char* variableName = rose::stringDuplicate(operandNameString.c_str());

  // declare the return string
     string operandDataSetupMacroString;

#if 0
  // Table form of data structure (this might be a better implementation of the code!)
     enum sizeSpecification    { uniformSize , variableSize };
     enum strideSpecification  { unitStride, uniformStride , variableStride };
     enum operandSpecification { lhsOperand, rhsOperand };

     sizeSpecification sizeSpecifier       = ;
     strideSpecification strideSpecifier   = ;
     operandSpecification operandSpecifier = (operandIndex == 0) ? lhsOperand : rhsOperand;

     string stringTable [2][3][2] =
        { { {"VARIABLE_DECLARATIONS_UNIFORM_SIZE_STRIDE_ONE_LHS_MACRO($dim,$VARIABLE_NAME);",
             "VARIABLE_DECLARATIONS_UNIFORM_SIZE_STRIDE_ONE_RHS_MACRO($dim,$VARIABLE_NAME);"},
            {"VARIABLE_DECLARATIONS_UNIFORM_SIZE_UNIFORM_STRIDE_LHS_MACRO($dim,$VARIABLE_NAME);",
             "VARIABLE_DECLARATIONS_UNIFORM_SIZE_UNIFORM_STRIDE_RHS_MACRO($dim,$VARIABLE_NAME);"},
            {"not implemented in table",
             "not implemented in table"}
          },
          { {"not implemented in table",
             "not implemented in table"},
            {"not implemented in table",
             "not implemented in table"},
            {"not implemented in table",
             "not implemented in table"} }
        };

     return stringTable[sizeSpecifier][strideSpecifier][operandSpecifier];
#endif

  // Get the recorded information about how this array operand is being indexed
     IndexingAccessEnumType localOperandCode = arrayOperandDataBase.getIndexingAccessCode();

     switch (localOperandCode)
        {
       // handle the different cases of indexing
          case UnknownIndexingAccess:
               printf ("Error case UnknownIndexingAccess in ArrayTransformationSupport::buildOperandVariableDeclarationMacro() \n");
               ROSE_ABORT();
               break;

          case UniformSizeUnitStride:
               operandDataSetupMacroString =
                    "ARRAY_OPERAND_UNIFORM_SIZE_DECLARATION_MACRO_D$dim();\n";
               break;

          case UniformSizeUniformStride:
               operandDataSetupMacroString =
                    "ARRAY_OPERAND_UNIFORM_SIZE_DECLARATION_MACRO_D$dim();\n";
               operandDataSetupMacroString +=
                    "ARRAY_OPERAND_UNIFORM_STRIDE_DECLARATION_MACRO_D$dim();\n";
               break;

          case UniformSizeVariableStride:
               operandDataSetupMacroString =
                    "ARRAY_OPERAND_UNIFORM_SIZE_DECLARATION_MACRO_D$dim();\n";
               operandDataSetupMacroString +=
                    "ARRAY_OPERAND_VARIABLE_STRIDE_DECLARATION_MACRO_D$dim($VARIABLE_IDENTIFIER);\n";
               break;

          case VariableSizeUnitStride:
#if 1
               operandDataSetupMacroString =
                    "ARRAY_OPERAND_VARIABLE_SIZE_DECLARATION_MACRO_D$dim(_$VARIABLE_NAME);\n";
#else
               operandDataSetupMacroString =
                    "ARRAY_OPERAND_VARIABLE_SIZE_DECLARATION_MACRO_D$dim($VARIABLE_IDENTIFIER);\n";
#endif
               break;

          case VariableSizeUniformStride:
               operandDataSetupMacroString =
                    "ARRAY_OPERAND_VARIABLE_SIZE_DECLARATION_MACRO_D$dim(_$VARIABLE_NAME);\n";
               operandDataSetupMacroString +=
                    "ARRAY_OPERAND_UNIFORM_STRIDE_DECLARATION_MACRO_D$dim();\n";
               break;

          case VariableSizeVariableStride:
            // In this case we build a special variable identifier from "_" and "$VARIABLE_NAME"
            // since the stride declaration is independent of the idexing (if any is used).
               operandDataSetupMacroString =
                    "ARRAY_OPERAND_VARIABLE_SIZE_DECLARATION_MACRO_D$dim(_$VARIABLE_NAME);\n";
               operandDataSetupMacroString +=
                    "ARRAY_OPERAND_VARIABLE_STRIDE_DECLARATION_MACRO_D$dim($VARIABLE_IDENTIFIER);\n";
               break;

          case scalarIndexingAccess:
               printf ("case of scalar indexing access not yet supported in variable declaration macro \n");
               ROSE_ABORT();
               break;

          default:
               printf ("default reached in subscript macro construction: buildOperandVariableDeclarationMacro() \n");
               ROSE_ABORT();
               break;
        }

  // Add variable declaration here instead of the global declarations
     operandDataSetupMacroString = "int _$VARIABLE_NAME;\n" + operandDataSetupMacroString;

     ROSE_ASSERT (operandDataSetupMacroString.c_str() != NULL);
  // printf ("operandDataSetupMacroString = %s \n",operandDataSetupMacroString);

     return StringUtility::stringDuplicate(operandDataSetupMacroString.c_str());
   }

string
ArrayTransformationSupport::buildOperandVariableInitializationMacro (
     const ArrayOperandDataBase & arrayOperandDataBase,
     int operandIndex )
   {
  // We don't need the variableName since we return $VARIABLE_NAME instead which is substituted by
  // the calling function this may be a poor design however.
  // char* variableName = rose::stringDuplicate(operandNameString.c_str());

  // declare the return string
     string operandDataSetupMacroString;

     IndexingAccessEnumType localOperandCode = arrayOperandDataBase.getIndexingAccessCode();
     switch (localOperandCode)
        {
       // handle the different cases of indexing
          case UnknownIndexingAccess:
               printf ("Error case UnknownIndexingAccess in ArrayTransformationSupport::buildOperandVariableInitializationMacro() \n");
               ROSE_ABORT();
               break;

          case UniformSizeUnitStride:
            // This could be optimized by only setting the size data using a single array operand (e.g. LHS)
               if (operandIndex == 0)
                  {
                    operandDataSetupMacroString  =
                         "ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D$dim($VARIABLE_NAME);\n";
                  }
               break;

          case UniformSizeUniformStride:
               if (operandIndex == 0)
                  {
                    operandDataSetupMacroString  =
                         "ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D$dim($VARIABLE_NAME);\n";
                    operandDataSetupMacroString +=
                         "ARRAY_OPERAND_UNIFORM_STRIDE_INITIALIZATION_MACRO_D$dim($VARIABLE_NAME);\n";
                  }
               break;

          case UniformSizeVariableStride:
               if (operandIndex == 0)
                  {
                    operandDataSetupMacroString  =
                         "ARRAY_OPERAND_UNIFORM_SIZE_INITIALIZATION_MACRO_D$dim($VARIABLE_NAME);\n";
                  }
               operandDataSetupMacroString +=
                    "ARRAY_OPERAND_VARIABLE_STRIDE_INITIALIZATION_MACRO_D$dim($VARIABLE_NAME,$VARIABLE_IDENTIFIER);\n";
               break;

          case VariableSizeUnitStride:
               operandDataSetupMacroString  =
                    "ARRAY_OPERAND_VARIABLE_SIZE_INITIALIZATION_MACRO_D$dim($VARIABLE_NAME,_$VARIABLE_NAME);\n";
#if 0
               if (operandIndex == 0)
                  {
                    operandDataSetupMacroString +=
                         "ARRAY_OPERAND_STRIDE_ONE_INITIALIZATION_MACRO_D$dim();\n";
                  }
#endif
               break;

          case VariableSizeUniformStride:
               operandDataSetupMacroString  =
                    "ARRAY_OPERAND_VARIABLE_SIZE_INITIALIZATION_MACRO_D$dim($VARIABLE_NAME,_$VARIABLE_NAME);\n";
               if (operandIndex == 0)
                  {
                    operandDataSetupMacroString +=
                         "ARRAY_OPERAND_UNIFORM_STRIDE_INITIALIZATION_MACRO_D$dim($VARIABLE_NAME);\n";
                  }
               break;

          case VariableSizeVariableStride:
               operandDataSetupMacroString  =
                    "ARRAY_OPERAND_VARIABLE_SIZE_INITIALIZATION_MACRO_D$dim($VARIABLE_NAME,_$VARIABLE_NAME);\n";
               operandDataSetupMacroString +=
                    "ARRAY_OPERAND_VARIABLE_STRIDE_INITIALIZATION_MACRO_D$dim($VARIABLE_NAME,$VARIABLE_IDENTIFIER);\n";
               break;

          case scalarIndexingAccess:
               printf ("cases of indexing access not yet supported in subscript macro construction (variable initialization macro) \n");
               ROSE_ABORT();
               break;

          default:
               printf ("case of scalar indexing access not yet supported in variable initialization macro \n");
               ROSE_ABORT();
               break;
        }

     if (operandIndex == 0)
        {
          operandDataSetupMacroString +=
               "ARRAY_TRANSFORMATION_LENGTH_INITIALIZATION_MACRO_D$dim($VARIABLE_NAME);\n";
        }

     ROSE_ASSERT (operandDataSetupMacroString.c_str() != NULL);
  // printf ("operandDataSetupMacroString = %s \n",operandDataSetupMacroString.c_str());

     return StringUtility::stringDuplicate(operandDataSetupMacroString.c_str());
   }

string
ArrayTransformationSupport::buildOperandSubscriptMacro (
     const ArrayOperandDataBase & arrayOperandDataBase,
     int operandIndex )
   {
  // This function builds the macro used to index the array operands

  // ROSE_ASSERT (inheritedAttribute.arrayStatementDimensionDefined == TRUE);
  // int arrayDimension = inheritedAttribute.arrayStatementDimension;
     int arrayDimension = arrayOperandDataBase.getDimension();
     ROSE_ASSERT (arrayDimension > 0);

  // template for final macro for each operand
  // char* macroTemplateString = rose::stringDuplicate("#define SC$VARIABLE_IDENTIFIER($OFFSET_VARIABLE_ARGS) $SUBSCRIPT_COMPUTATION");
     string macroTemplateString = "#define SC$VARIABLE_IDENTIFIER($OFFSET_VARIABLE_ARGS) $SUBSCRIPT_COMPUTATION";
  // ROSE_ASSERT (macroTemplateString != NULL);

  // Build the argument list for the subscript (this function will substitute $DIM
  // with the array dimension for each array dimension from 1 to 'arrayDimension')
     string macroOffsetArgumentList = buildChainedString ( arrayDimension, "x1",",x$DIM");
  // printf ("macroOffsetArgumentList = %s \n",macroOffsetArgumentList);

     macroTemplateString = SgNode::copyEdit ( macroTemplateString, "$OFFSET_VARIABLE_ARGS", macroOffsetArgumentList );
  // printf ("Preliminary macroTemplateString = %s \n",macroTemplateString);

     string subscriptMacroString;
     string subscriptComputationMacroString;

  // Strings used for control of macro numbering using the operand index
     string subscriptMacroNumberingTargetString; // old string
     string subscriptMacroNumberingNewString;    // new string

     string operandNameString = arrayOperandDataBase.arrayVariableName;
  // char* subscriptMacroIdentifierString = arrayOperandDataBase.generateIdentifierString().c_str();
     string subscriptMacroIdentifierString = arrayOperandDataBase.generateIdentifierString();

  // Get the recorded information about how this array operand is being indexed
     IndexingAccessEnumType localOperandCode = arrayOperandDataBase.getIndexingAccessCode();
  // printf ("In ArrayTransformationSupport::buildOperandSubscriptMacro(): localOperandCode = %d \n",localOperandCode);

     switch (localOperandCode)
        {
       // handle the different cases of indexing
          case UnknownIndexingAccess:
               printf ("Error case UnknownIndexingAccess \n");
               ROSE_ABORT();
               break;

          case UniformSizeUnitStride:
#if 0
               subscriptMacroString = 
                    buildChainedString ( arrayDimension, "/* case UniformSizeUnitStride */ (x1)", "+(x$DIM)*_size$LOWER_DIM");
               subscriptComputationMacroString = macroTemplateString;
               ROSE_ASSERT (subscriptComputationMacroString != NULL);
#else
               subscriptMacroString =
                    buildChainedString ( arrayDimension, "/* case UniformSizeUnitStride */ (x1)", "+(x$DIM)*_size$LOWER_DIM");

            // Even if the stride or size is uniform we have to set it to values 
            // taken from the one of the array operands (typically we use the LHS 
            // which is generally the first operand).
               if (operandIndex == 0)
                  {
                 // define a macro once that will work for all operands
                    subscriptComputationMacroString = macroTemplateString;
                  }
                 else
                  {
                 // Don't both defining a subscript computation macro since one is already defined for counter == 0
                    subscriptComputationMacroString = rose::stringDuplicate("");
                  }
#endif
            // Remove the _$VARIABLE_IDENTIFIER substring since this one macro will be use for
            // subscript computation within all operands Note that this has to match the subscript
            // macro called in the inner loop transformation!
               subscriptMacroNumberingTargetString = "$VARIABLE_IDENTIFIER";
               subscriptMacroNumberingNewString    = "";
               break;

          case UniformSizeUniformStride:
            // NOTE: the constuction of the loop nest is such that we can eliminate the stride in
            // the subscript computation
               subscriptMacroString =
                    buildChainedString ( arrayDimension,
                                         "/* case UniformSizeUniformStride */ (x1)",
                                         "+((x$DIM)*_size$LOWER_DIM)");

            // define a different macro for the subscript computation within each operand
               subscriptComputationMacroString = macroTemplateString;

            // Substitute the operand index into the macroTemplateString (so that we can
            // support a different subscript computation for each unique operand)
               subscriptMacroNumberingTargetString = "$VARIABLE_IDENTIFIER";
            // subscriptMacroNumberingNewString    = subscriptMacroIdentifierString;
               subscriptMacroNumberingNewString    = "";
               break;

          case UniformSizeVariableStride:
               subscriptMacroString =
                    buildChainedString ( arrayDimension,
                                         "/* case UniformSizeVariableStride */ (x1)*$VARIABLE_IDENTIFIER_stride1",
                                         "+((x$DIM)*$VARIABLE_IDENTIFIER_stride$DIM*_size$LOWER_DIM)");

            // define a different macro for the subscript computation within each operand
               subscriptComputationMacroString = macroTemplateString;

            // Substitute the operand index into the macroTemplateString (so that we can
            // support a different subscript computation for each unique operand)
               subscriptMacroNumberingTargetString = "$VARIABLE_IDENTIFIER";
               subscriptMacroNumberingNewString    = subscriptMacroIdentifierString;
               break;

          case VariableSizeUnitStride:
               subscriptMacroString =
                    buildChainedString ( arrayDimension,
                                         "/* case VariableSizeUnitStride */ (x1)",
                                         "+((x$DIM)*_$VARIABLE_NAME_size$LOWER_DIM)");

            // define a different macro for the subscript computation within each operand
               subscriptComputationMacroString = macroTemplateString;

            // Substitute the operand index into the macroTemplateString (so that we can
            // support a different subscript computation for each unique operand)
               subscriptMacroNumberingTargetString = "$VARIABLE_IDENTIFIER";
               subscriptMacroNumberingNewString    = subscriptMacroIdentifierString;
               break;

          case VariableSizeUniformStride:
            // NOTE: the constuction of the loop nest is such that we can eliminate the stride in
            // the subscript computation
               subscriptMacroString =
                    buildChainedString ( arrayDimension,
                                         "/* case VariableSizeUniformStride */ (x1)",
                                         "+((x$DIM)*_$VARIABLE_NAME_size$LOWER_DIM)");

            // define a different macro for the subscript computation within each operand
               subscriptComputationMacroString = macroTemplateString;

            // Substitute the operand index into the macroTemplateString (so that we can
            // support a different subscript computation for each unique operand)
               subscriptMacroNumberingTargetString = "$VARIABLE_IDENTIFIER";
               subscriptMacroNumberingNewString    = subscriptMacroIdentifierString;
               break;

          case VariableSizeVariableStride:
            // printf ("NOTE: Should use _$VARIABLE_NAME_size$LOWER_DIM instead of $VARIABLE_IDENTIFIER_size$LOWER_DIM \n");
               subscriptMacroString =
                    buildChainedString ( arrayDimension,
                                         "/* case VariableSizeVariableStride */ (x1)*$VARIABLE_IDENTIFIER_stride1",
                                         "+((x$DIM)*$VARIABLE_IDENTIFIER_stride$DIM*_$VARIABLE_NAME_size$LOWER_DIM)");

            // define a different macro for the subscript computation within each operand
               subscriptComputationMacroString = macroTemplateString;

            // Substitute the operand index into the macroTemplateString (so that we can
            // support a different subscript computation for each unique operand)
               subscriptMacroNumberingTargetString = "$VARIABLE_IDENTIFIER";
               subscriptMacroNumberingNewString    = subscriptMacroIdentifierString;
               break;

          case scalarIndexingAccess:
               printf ("case of scalar indexing access not yet supported in subscript macro construction (macro construction) \n");
               ROSE_ABORT();
               break;

          default:
               printf ("default reached in subscript macro construction: buildOperandSubscriptMacro() \n");
               ROSE_ABORT();
               break;
        }

     ROSE_ASSERT (subscriptMacroString.length() > 0);
     ROSE_ASSERT (subscriptComputationMacroString.length() > 0);

#if 0
     printf ("subscriptMacroString            = %s \n",subscriptMacroString.c_str());
     printf ("subscriptComputationMacroString = %s \n",subscriptComputationMacroString.c_str());
#endif

  // Edit macro numbering into place or remove it depending of the values within
  // subscriptMacroNumberingTargetString and subscriptMacroNumberingNewString
     subscriptComputationMacroString =
          SgNode::copyEdit (
               subscriptComputationMacroString, subscriptMacroNumberingTargetString, subscriptMacroNumberingNewString );

#if 0
     printf ("TEST 1: subscriptComputationMacroString = %s \n",subscriptComputationMacroString);
#endif

  // Build the macro declaration (subscript expression)
     subscriptComputationMacroString =
          SgNode::copyEdit (
               subscriptComputationMacroString, "$SUBSCRIPT_COMPUTATION", subscriptMacroString );

#if 0
     printf ("TEST 2: subscriptComputationMacroString = %s \n",subscriptComputationMacroString);
#endif

     string returnString;

  // Now process it so that the string can be unparsed as a macro by the unparser
  // if (rose::isSameName(subscriptComputationMacroString,"") == TRUE)
     if (subscriptComputationMacroString == "")
        {
       // Don't turn empty strings into macros
       // returnString = rose::stringDuplicate("");
        }
       else
        {
       // Turn any non-empty string into a string literal with a coded prefix
       // that will be unparsed as a macro.  (A Temporary Hack in the ROSE unparser.)
          returnString = TransformationSupport::buildMacro(subscriptComputationMacroString);
        }

#if 0
     printf ("BOTTOM of ArrayTransformationSupport::buildOperandSubscriptMacro(): returnString = %s \n",returnString.c_str());
     ROSE_ABORT();
#endif

     return returnString;
   }


string
ArrayTransformationSupport::buildChainedString (
   int arrayDimension,
   const string firstDimensionString,
   const string nextDimensionString )
   {
  // This function chains multipe strings together according the value of arrayDimension it is
  // useful for building subscript expressions but aso used for building the argument lists of
  // macros.  The substrings "$INDEX_DIM" and "$SIZE_DIM" are substituted with the dimension value
  // and the dimension value minus one, respectively.

     string returnString;

  // Make a copy
     string subscriptMacroString = firstDimensionString;

     int i = 0;
     for (i = 2; i <= arrayDimension; i++)
        {
       // Build the multidimensional case
       // subscriptMacroString = rose::stringConcatinate (subscriptMacroString,nextDimensionString);
          subscriptMacroString = subscriptMacroString + nextDimensionString;

       // Generate a string representing the index into the list of operands
          char dimensionString[128];
          sprintf (dimensionString,"%d",i);
       // printf ("dimensionString = %s \n",dimensionString);

          subscriptMacroString =
               SgNode::copyEdit ( subscriptMacroString, "$DIM", dimensionString );

       // Build the variable name associated with the size of the lower dimension
          char dimensionMinusOneString[128];
          sprintf (dimensionMinusOneString,"%d",i-1);
       // printf ("dimensionMinusOneString = %s \n",dimensionMinusOneString);

          subscriptMacroString =
               SgNode::copyEdit ( subscriptMacroString, "$LOWER_DIM", dimensionMinusOneString );
        }

     returnString = subscriptMacroString;

     ROSE_ASSERT (returnString.length() > 0);
     return returnString;
   }


















