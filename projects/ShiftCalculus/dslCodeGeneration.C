
// Example ROSE Translator reads input program and implements a DSL embedded within C++
// to support the stencil computations, and required runtime support is developed seperately.
#include "rose.h"

#include "stencilAndStencilOperatorDetection.h"
#include "stencilEvaluation.h"

#include "shiftCalculusCompiler.h"

#include "stencilFiniteStateMachine.h"

#include "dslSupport.h"

using namespace std;

using namespace DSL_Support;

//We don't generate cuda code by default
bool b_gen_cuda = false;
// an internal variable to store the generated serial loop nests.
static SgForStatement* temp_for_loop_nest = NULL; 

// SgForStatement* buildLoopNest(int stencilDimension, SgBasicBlock* & innerLoopBody, SgVariableSymbol* sourceVariableSymbol,
//    SgVariableSymbol* & indexVariableSymbol_X, SgVariableSymbol* & indexVariableSymbol_Y, SgVariableSymbol* & indexVariableSymbol_Z, 
//    SgVariableSymbol* & arraySizeVariableSymbol_X, SgVariableSymbol* & arraySizeVariableSymbol_Y)
SgForStatement* buildLoopNest(int stencilDimension, SgBasicBlock* & innerLoopBody, SgVariableSymbol* boxVariableSymbol,
   SgVariableSymbol* & indexVariableSymbol_X, SgVariableSymbol* & indexVariableSymbol_Y, SgVariableSymbol* & indexVariableSymbol_Z, 
   SgVariableSymbol* & arraySizeVariableSymbol_X, SgVariableSymbol* & arraySizeVariableSymbol_Y)
   {
  // SgScopeStatement* currentScope    = outerScope;
     SgForStatement*   loopNest        = NULL;
     SgBasicBlock*     currentLoopBody = NULL;

     vector<string> indexNameList;
     vector<int>    indexValueList;

     if (stencilDimension == 3)
        {
          indexNameList.push_back("k");
          indexNameList.push_back("j");
          indexNameList.push_back("i");

          indexValueList.push_back(2);
          indexValueList.push_back(1);
          indexValueList.push_back(0);
        }

     if (stencilDimension == 2)
        {
          indexNameList.push_back("j");
          indexNameList.push_back("i");

          indexValueList.push_back(1);
          indexValueList.push_back(0);
        }

     if (stencilDimension == 1)
        {
          indexNameList.push_back("i");

          indexValueList.push_back(0);
        }

     vector<string> arraySizeNameList;
     arraySizeNameList.push_back("arraySize_X");
     arraySizeNameList.push_back("arraySize_Y");

     if (stencilDimension != 2)
        {
          printf ("Sorry, code generation only implemented and tested for 2D stencils at present: stencilDimension = %d \n",stencilDimension);
          ROSE_ASSERT(false);
        }
  // This fails for the 0D identity stencil specification.
     ROSE_ASSERT(stencilDimension > 0);


     for (int k = 0; k < stencilDimension; k++)
        {
       // Add the scope early so that any possible declarations in the for loop initializer will be put into 
       // the correct scope and not use types or variables from the outer scope.
          SgForStatement* forStatementScope = new SgForStatement((SgStatement*)NULL,(SgExpression*)NULL,(SgStatement*)NULL);
          ROSE_ASSERT(forStatementScope != NULL);

          SgExpression* lowerBound_value_part1  = buildMemberFunctionCall(boxVariableSymbol,"getLowCorner",NULL,false);
          ROSE_ASSERT(lowerBound_value_part1 != NULL);
          SgExpression* lowerBound_value_part3  = buildMemberFunctionCall(lowerBound_value_part1,"operator[]",SageBuilder::buildIntVal(indexValueList[k]),true);
          ROSE_ASSERT(lowerBound_value_part3 != NULL);
          SgExpression* lowerBound_value = lowerBound_value_part3;

       // ROSE_DLL_API SgVariableDeclaration* buildVariableDeclaration_nfi(const SgName & name, SgType *type, SgInitializer *varInit, SgScopeStatement* scope);
       // ROSE_DLL_API SgAssignInitializer * buildAssignInitializer_nfi(SgExpression * operand_i = NULL, SgType * expression_type = NULL);
       // SgAssignInitializer* assignInitializer = SageBuilder::buildAssignInitializer_nfi(SageBuilder::buildIntVal(1));
          SgAssignInitializer* assignInitializer = SageBuilder::buildAssignInitializer_nfi(lowerBound_value);
          ROSE_ASSERT(assignInitializer != NULL);

          SgVariableDeclaration* variableDeclaration  = SageBuilder::buildVariableDeclaration_nfi(indexNameList[k],SageBuilder::buildIntType(),assignInitializer,forStatementScope);
          ROSE_ASSERT(variableDeclaration != NULL);

          SgForInitStatement* for_loop_initialization = SageBuilder::buildForInitStatement(variableDeclaration);
          ROSE_ASSERT(for_loop_initialization != NULL);

          SgVarRefExp* varRefExp_for_test = SageBuilder::buildVarRefExp(variableDeclaration);

       // Set the indexVariableSymbol pointers (passed by reference).
          if (stencilDimension == 1)
             {
               if (k == 0) indexVariableSymbol_X = varRefExp_for_test->get_symbol();
             }
          if (stencilDimension == 2)
             {
               if (k == 0) indexVariableSymbol_Y = varRefExp_for_test->get_symbol();
               if (k == 1) indexVariableSymbol_X = varRefExp_for_test->get_symbol();
             }
          if (stencilDimension == 3)
             {
               if (k == 0) indexVariableSymbol_Z = varRefExp_for_test->get_symbol();
               if (k == 1) indexVariableSymbol_Y = varRefExp_for_test->get_symbol();
               if (k == 2) indexVariableSymbol_X = varRefExp_for_test->get_symbol();
             }

       // This is the loop bound (which is not set correctly yet).
       // SgExpression* upperBound_value  = SageBuilder::buildIntVal(42);
       // SgExpression* upperBound_value_part1  = buildMemberFunctionCall(sourceVariableSymbol,"getBox",NULL,false);
          SgExpression* upperBound_value_part1  = buildMemberFunctionCall(boxVariableSymbol,"getHighCorner",NULL,false);
          ROSE_ASSERT(upperBound_value_part1 != NULL);
       // SgType* type_1 = upperBound_value_part1->get_type();
       // SgExpression* upperBound_value_part2  = buildMemberFunctionCall(upperBound_value_part1,type_1,"getHighCorner",NULL,false);
       // ROSE_ASSERT(upperBound_value_part2 != NULL);
       // SgType* type_2 = upperBound_value_part2->get_type();
       // SgExpression* upperBound_value_part3  = buildMemberFunctionCall(upperBound_value_part2,type_2,"operator[]",SageBuilder::buildIntVal(k),true);
       // SgExpression* upperBound_value_part3  = buildMemberFunctionCall(upperBound_value_part2,type_2,"operator[]",SageBuilder::buildIntVal(indexValueList[k]),true);
       // SgExpression* upperBound_value_part3  = buildMemberFunctionCall(upperBound_value_part1,type_1,"operator[]",SageBuilder::buildIntVal(indexValueList[k]),true);
          SgExpression* upperBound_value_part3  = buildMemberFunctionCall(upperBound_value_part1,"operator[]",SageBuilder::buildIntVal(indexValueList[k]),true);
          ROSE_ASSERT(upperBound_value_part3 != NULL);
          SgExpression* upperBound_value = upperBound_value_part3;

       // We want to use the exact bound from the box, not modify it to subtract 1.
       // SgExpression* value_one         = SageBuilder::buildIntVal(1);
       // SgExpression* upperBound_expr   = SageBuilder::buildSubtractOp(upperBound_value,value_one);

       // SgExpression* testExpression    = SageBuilder::buildLessThanOp(varRefExp_for_test,upperBound_expr);
          SgExpression* testExpression    = SageBuilder::buildLessThanOp(varRefExp_for_test,upperBound_value);
          SgStatement* testStatement      = SageBuilder::buildExprStatement(testExpression);
          ROSE_ASSERT(testStatement != NULL);

          SgVarRefExp* varRefExp_for_incr    = SageBuilder::buildVarRefExp(variableDeclaration);
          SgExpression* incrementExpression  = SageBuilder::buildPlusPlusOp(varRefExp_for_incr);
          ROSE_ASSERT(incrementExpression != NULL);

          SgBasicBlock* loopBody = SageBuilder::buildBasicBlock_nfi();
          ROSE_ASSERT(loopBody != NULL);

       // Use the previously build scope (forStatementScope)
          SageBuilder::buildForStatement_nfi(forStatementScope, for_loop_initialization, testStatement, incrementExpression, loopBody);
#if 0
          printf ("dimension: k = %d \n",k);
#endif
       // For loop nests with more than one level we need to insert the for loop into the body of the previous for loop.
          if (k == 0)
             {
            // Save the outer most forStatementScope.
               loopNest = forStatementScope;
               currentLoopBody = loopBody;
             }
            else
             {
            // This is not the correct expression in the SgAssignInitializer (fix this later).
               SgAssignInitializer* assignInitializer = SageBuilder::buildAssignInitializer_nfi(SageBuilder::buildIntVal(42));
               ROSE_ASSERT(assignInitializer != NULL);

               SgVariableDeclaration* arraySizeVariableDeclaration  = SageBuilder::buildVariableDeclaration_nfi(arraySizeNameList[k-1],SageBuilder::buildIntType(),assignInitializer,currentLoopBody);
               ROSE_ASSERT(variableDeclaration != NULL);

               if (k == 1) arraySizeVariableSymbol_X = SageInterface::getFirstVarSym (arraySizeVariableDeclaration);
               if (k == 2) arraySizeVariableSymbol_Y = SageInterface::getFirstVarSym (arraySizeVariableDeclaration);

            // Add the variable declaration of the array size in the previous dimension.
               SageInterface::appendStatement(arraySizeVariableDeclaration,currentLoopBody);

               SageInterface::appendStatement(forStatementScope,currentLoopBody);
               currentLoopBody = loopBody;
             }

          innerLoopBody = currentLoopBody;
        }

     ROSE_ASSERT(innerLoopBody != NULL);

     return loopNest;
   }


SgExpression* 
buildStencilPoint (StencilOffsetFSM* stencilOffsetFSM, double stencilCoeficient, int stencilDimension, SgVariableSymbol* variableSymbol, 
   SgVariableSymbol* indexVariableSymbol_X, SgVariableSymbol* indexVariableSymbol_Y, SgVariableSymbol* indexVariableSymbol_Z, 
   SgVariableSymbol* arraySizeVariableSymbol_X, SgVariableSymbol* arraySizeVariableSymbol_Y, bool generateLowlevelCode)
   {
  // We want to generate: source[j*axis_x_size+i]

     SgExpression* returnExpression = NULL;

     static int counter = 0;

     ROSE_ASSERT(variableSymbol != NULL);

  // Hard coding for stencil dimensions will be removed later.
     if (stencilDimension == 2)
        {
#if 0
          printf ("In buildStencilPoint(): Building stencil point for 2D stencil: stencilCoeficient = %3.2f \n",stencilCoeficient);
#endif
          ROSE_ASSERT(indexVariableSymbol_X != NULL);
          ROSE_ASSERT(indexVariableSymbol_Y != NULL);

          ROSE_ASSERT(arraySizeVariableSymbol_X != NULL);

       // We need to generate the linearized index expression for the "double RectMDArray::operator[](int index)".  To do this
       // we need an expresion for the x_axis_size (and for y_axis_size for 3D stencils).

       // SgVarRefExp* variableVarRefExp    = SageBuilder::buildVarRefExp(variableSymbol);
          SgVarRefExp* indexVarRefExp_Y     = SageBuilder::buildVarRefExp(indexVariableSymbol_Y);
          SgVarRefExp* arraySizeVarRefExp_X = SageBuilder::buildVarRefExp(arraySizeVariableSymbol_X);
          SgVarRefExp* indexVarRefExp_X     = SageBuilder::buildVarRefExp(indexVariableSymbol_X);

       // For the index expression.
          SgExpression* expression = NULL;
          if (stencilOffsetFSM->offsetValues[0] == 0 && stencilOffsetFSM->offsetValues[1] == 0)
             {
            // This is a center point in the stencil
            // We want to generate: source[j*axis_x_size+i]
               expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(indexVarRefExp_Y,arraySizeVarRefExp_X), indexVarRefExp_X);
             }
            else
             {
            // This is a stencil point offset from the center of the stencil (so it has a binary operator associated with the index expression).

            // We want to generate: "source[(j + offset_x)*axis_x_size+i + offset_y]"
               if (stencilOffsetFSM->offsetValues[0] == 0 && stencilOffsetFSM->offsetValues[1] != 0)
                  {
                    SgExpression* offsetValue_Y = SageBuilder::buildIntVal(stencilOffsetFSM->offsetValues[1]);
                    expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(SageBuilder::buildAddOp(indexVarRefExp_Y,offsetValue_Y),arraySizeVarRefExp_X), indexVarRefExp_X);
                  }
                 else
                  {
                    ROSE_ASSERT(stencilOffsetFSM->offsetValues[0] != 0);
                    if (stencilOffsetFSM->offsetValues[0] != 0 && stencilOffsetFSM->offsetValues[1] == 0)
                       {
                         SgExpression* offsetValue_X = SageBuilder::buildIntVal(stencilOffsetFSM->offsetValues[0]);
                         expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(indexVarRefExp_Y,arraySizeVarRefExp_X), SageBuilder::buildAddOp(indexVarRefExp_X,offsetValue_X));
                       }
                      else
                       {
                         ROSE_ASSERT(stencilOffsetFSM->offsetValues[0] != 0);
                         ROSE_ASSERT(stencilOffsetFSM->offsetValues[1] != 0);

                         SgExpression* offsetValue_X = SageBuilder::buildIntVal(stencilOffsetFSM->offsetValues[0]);
                         SgExpression* offsetValue_Y = SageBuilder::buildIntVal(stencilOffsetFSM->offsetValues[1]);
                         expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(SageBuilder::buildAddOp(indexVarRefExp_Y,offsetValue_Y),arraySizeVarRefExp_X), SageBuilder::buildAddOp(indexVarRefExp_X,offsetValue_X));
                       }
                  }
             }


          SgExpression* dataReferenceExpression = NULL;
          if (generateLowlevelCode == true)
             {
               SgVarRefExp* arrayPointerVarRefExp = SageBuilder::buildVarRefExp(variableSymbol);
               SgPntrArrRefExp* arrayRefExp       = SageBuilder::buildPntrArrRefExp(arrayPointerVarRefExp,expression);
               dataReferenceExpression = arrayRefExp;
             }
            else
             {
               SgFunctionCallExp* functionCallExp = buildMemberFunctionCall(variableSymbol,"operator[]",expression,true);
               dataReferenceExpression = functionCallExp;
             }

       // If stencilCoeficient == 1.0 then don't build the SgMultiplyOp (I will handle this better later).
          if (stencilCoeficient < 0.99999 || stencilCoeficient > 1.00001)
             {
            // Build the value expression for the coeficient.
               SgDoubleVal* doubleVal = SageBuilder::buildDoubleVal(stencilCoeficient);

            // Build the expression to multiply the array reference (using "operator[]" member function) times the stencil coeficient.
            // SgMultiplyOp* multiplyOp = SageBuilder::buildMultiplyOp(functionCallExp,doubleVal);
               SgMultiplyOp* multiplyOp = SageBuilder::buildMultiplyOp(dataReferenceExpression,doubleVal);
               returnExpression = multiplyOp;
             }
            else
             {
            // This is the case where stencilCoeficient == 1.0 (but I want to avoid equality comparisions of floating point values).
            // returnExpression = functionCallExp;
               returnExpression = dataReferenceExpression;
             }          
        }
       else
        {
          printf ("Error: Dimensions other than 2 not yet supported! \n");
          ROSE_ASSERT(false);
        }

#if 1
     if (returnExpression == NULL)
        {
          printf ("returnExpression == NULL: temp code set to SgIntVal \n");
          returnExpression = SageBuilder::buildIntVal(counter);
          counter++;
        }
#endif

     return returnExpression;
   }


SgExprStatement* assembleStencilSubTreeArray(SgExpression* stencil_lhs, vector<SgExpression*> & stencilSubTreeArray, int stencilDimension, SgVariableSymbol* destinationVariableSymbol)
   {
  // Assemble the subtrees for each point in the stencil into a single expression, and
  // put the expression into a SgExprStatement.

     ROSE_ASSERT(stencil_lhs != NULL);

     SgExpression* expression = NULL;
     if (stencilSubTreeArray.size() == 0)
        {
       // This case shoudl not happen, but it is at least defined.
          expression = SageBuilder::buildNullExpression();
        }
       else
        {
          if (stencilSubTreeArray.size() == 1)
             {
            // This is the trivial case of a stencil with a single point (identity stencil).
               expression = stencilSubTreeArray[0];
               ROSE_ASSERT(expression != NULL);
             }
            else
             {
            // These are the more meaningful stencils.

            // This builds the first binary operator to sum the stencil points.
               if (stencilSubTreeArray.size() >= 2)
                  {
                    expression = SageBuilder::buildAddOp(stencilSubTreeArray[0],stencilSubTreeArray[1]);
                    ROSE_ASSERT(expression != NULL);
                  }

            // This builds the remaining binary operators to sum the stencil points (skip the first two used to build the first binary operator).
               for (size_t i = 2; i < stencilSubTreeArray.size(); i++)
                  {
                    expression = SageBuilder::buildAddOp(expression,stencilSubTreeArray[i]);
                    ROSE_ASSERT(expression != NULL);
                  }
             }
        }

     expression = SageBuilder::buildAssignOp(stencil_lhs,expression);
     ROSE_ASSERT(expression != NULL);

     SgExprStatement* returnStatement = SageBuilder::buildExprStatement(expression);
     ROSE_ASSERT(returnStatement != NULL);

     return returnStatement;
   }


void generateStencilCode(StencilEvaluationTraversal & traversal, bool generateLowlevelCode)
   {
  // Read the stencil and generate the inner most loop AST for the stencil.

  // Note that generateLowlevelCode controls the generation of low level C code using a
  // base pointer to raw memory and linearized indexing off of that pointer.  The
  // alternative is to use the operator[] member function in the RectMDArray class.

  // Example of code that we want to generate:
  // for (j=0; j < source.size(0); j++)
  //    {
  //      int axis_x_size = source.size(0);
  //      for (i=0; i < source.size(0); i++)
  //         {
  //           destination[j*axis_x_size+i] = source[j*axis_x_size+i];
  //         }
  //    }

  // This function genertes the loop nest only:
  //    SgForStatement* buildLoopNest(int stencilDimension, SgBasicBlock* & innerLoopBody)

  // This function generates the statement in the inner most loop body:
  //    SgExprStatement* assembleStencilSubTreeArray(vector<SgExpression*> & stencilSubTreeArray)

  // This function generates the AST representing the stencil points:
  //    SgExpression* buildStencilPoint (StencilOffsetFSM* stencilOffsetFSM, double stencilCoeficient, int stencilDimension, SgVariableSymbol* destinationVariableSymbol, SgVariableSymbol* sourceVariableSymbol)



  // The generated code should be in terms of the operator[]() functions on the 
  // RectMDArray objects.  Likely we have to support a wider range of generated code later.
  //    const RectMDArray<TDest>& a_LOfPhi,
  //    const RectMDArray<TSrc>& a_phi,

  // std::vector<SgFunctionCallExp*> stencilOperatorFunctionCallList;
     std::vector<SgFunctionCallExp*> stencilOperatorFunctionCallList = traversal.get_stencilOperatorFunctionCallList();
     for (size_t i = 0; i < stencilOperatorFunctionCallList.size(); i++)
        {
          SgFunctionCallExp* functionCallExp = stencilOperatorFunctionCallList[i];
          ROSE_ASSERT(functionCallExp != NULL);

          printf ("processing functionCallExp = %p \n",functionCallExp);

          SgStatement* associatedStatement = TransformationSupport::getStatement(functionCallExp);
          ROSE_ASSERT(associatedStatement != NULL);

          string filename = associatedStatement->get_file_info()->get_filename();
          int lineNumber  = associatedStatement->get_file_info()->get_line();

          printf ("Generating code for stencil operator used at file = %s at line = %d \n",filename.c_str(),lineNumber);

          SgExprListExp* argumentList = functionCallExp->get_args();
          ROSE_ASSERT(argumentList != NULL);

       // There should be four elements to a stencil operator.
          ROSE_ASSERT(argumentList->get_expressions().size() == 4);

       // Stencil
          SgExpression* stencilExpression = argumentList->get_expressions()[0];
          SgVarRefExp* stencilVarRefExp = isSgVarRefExp(stencilExpression);
          ROSE_ASSERT(stencilVarRefExp != NULL);

       // RectMDArray (destination)
          SgExpression* destinationArrayReferenceExpression = argumentList->get_expressions()[1];
          SgVarRefExp* destinationArrayVarRefExp = isSgVarRefExp(destinationArrayReferenceExpression);
          ROSE_ASSERT(destinationArrayVarRefExp != NULL);

       // RectMDArray (source)
          SgExpression* sourceArrayReferenceExpression = argumentList->get_expressions()[2];
          SgVarRefExp* sourceArrayVarRefExp = isSgVarRefExp(sourceArrayReferenceExpression);
          ROSE_ASSERT(sourceArrayVarRefExp != NULL);

       // Box
          SgExpression* boxReferenceExpression = argumentList->get_expressions()[3];
          SgVarRefExp* boxVarRefExp = isSgVarRefExp(boxReferenceExpression);
          ROSE_ASSERT(boxVarRefExp != NULL);

          printf ("DONE: processing inputs to stencil operator \n");

          ROSE_ASSERT(stencilVarRefExp->get_symbol() != NULL);
          SgInitializedName* stencilInitializedName = stencilVarRefExp->get_symbol()->get_declaration();
          ROSE_ASSERT(stencilInitializedName != NULL);

          string stencilName = stencilInitializedName->get_name();

          printf ("stencilName = %s \n",stencilName.c_str());

          std::map<std::string,StencilFSM*> & stencilMap = traversal.get_stencilMap();
          
          ROSE_ASSERT(stencilMap.find(stencilName) != stencilMap.end());

          StencilFSM* stencilFSM = stencilMap[stencilName];
          ROSE_ASSERT(stencilFSM != NULL);

          int stencilDimension = stencilFSM->stencilDimension();

       // These are computed values.
          printf ("Stencil dimension = %d \n",stencilDimension);
          printf ("Stencil width     = %d \n",stencilFSM->stencilWidth());

          std::vector<std::pair<StencilOffsetFSM,double> > & stencilPointList = stencilFSM->stencilPointList;

       // This is the scope where the stencil operator is evaluated.
          SgScopeStatement* outerScope = associatedStatement->get_scope();
          ROSE_ASSERT(outerScope != NULL);

          SgVariableSymbol* indexVariableSymbol_X     = NULL;
          SgVariableSymbol* indexVariableSymbol_Y     = NULL;
          SgVariableSymbol* indexVariableSymbol_Z     = NULL;
          SgVariableSymbol* arraySizeVariableSymbol_X = NULL;
          SgVariableSymbol* arraySizeVariableSymbol_Y = NULL;

          SgVariableSymbol* destinationVariableSymbol = destinationArrayVarRefExp->get_symbol();
          ROSE_ASSERT(destinationVariableSymbol != NULL);
          SgVariableSymbol* sourceVariableSymbol = sourceArrayVarRefExp->get_symbol();
          ROSE_ASSERT(sourceVariableSymbol != NULL);
          SgVariableSymbol* boxVariableSymbol = boxVarRefExp->get_symbol();
          ROSE_ASSERT(boxVariableSymbol != NULL);

       // This can be important in handling of comments and CPP directives.
          bool autoMovePreprocessingInfo = true;

          SgStatement* lastStatement = associatedStatement;
          if (generateLowlevelCode == true)
             {
#if 1
               SgVariableDeclaration* sourceDataPointerVariableDeclaration = buildDataPointer("sourceDataPointer",sourceVariableSymbol,outerScope);
#else
            // Optionally build a pointer variable so that we can optionally support a C style indexing for the DTEC DSL blocks.
               SgExpression* sourcePointerExp = buildMemberFunctionCall(sourceVariableSymbol,"getPointer",NULL,false);
               ROSE_ASSERT(sourcePointerExp != NULL);
               SgAssignInitializer* assignInitializer = SageBuilder::buildAssignInitializer_nfi(sourcePointerExp);
               ROSE_ASSERT(assignInitializer != NULL);

            // Build the variable declaration for the pointer to the data.
               string sourcePointerName = "sourceDataPointer";
               SgVariableDeclaration* sourceDataPointerVariableDeclaration  = SageBuilder::buildVariableDeclaration_nfi(sourcePointerName,SageBuilder::buildPointerType(SageBuilder::buildDoubleType()),assignInitializer,outerScope);
               ROSE_ASSERT(sourceDataPointerVariableDeclaration != NULL);
#endif

            // SageInterface::insertStatementAfter(associatedStatement,forStatementScope,autoMovePreprocessingInfo);
               SageInterface::insertStatementAfter(associatedStatement,sourceDataPointerVariableDeclaration,autoMovePreprocessingInfo);

               SgVariableDeclaration* destinationDataPointerVariableDeclaration = buildDataPointer("destinationDataPointer",destinationVariableSymbol,outerScope);
               SageInterface::insertStatementAfter(sourceDataPointerVariableDeclaration,destinationDataPointerVariableDeclaration,autoMovePreprocessingInfo);

            // Reset the variable symbols we will use in the buildStencilPoint() function.
               sourceVariableSymbol      = SageInterface::getFirstVarSym(sourceDataPointerVariableDeclaration);
               destinationVariableSymbol = SageInterface::getFirstVarSym(destinationDataPointerVariableDeclaration);

               lastStatement = destinationDataPointerVariableDeclaration;
             }

          SgBasicBlock* innerLoopBody = NULL;
       // SgForStatement* loopNest = buildLoopNest(stencilFSM->stencilDimension(),innerLoopBody,sourceVariableSymbol,indexVariableSymbol_X,indexVariableSymbol_Y,indexVariableSymbol_Z,arraySizeVariableSymbol_X,arraySizeVariableSymbol_Y);
          SgForStatement* loopNest = buildLoopNest(stencilFSM->stencilDimension(),innerLoopBody,boxVariableSymbol,indexVariableSymbol_X,indexVariableSymbol_Y,indexVariableSymbol_Z,arraySizeVariableSymbol_X,arraySizeVariableSymbol_Y);
          ROSE_ASSERT(innerLoopBody != NULL);

          ROSE_ASSERT(lastStatement != NULL);
          SageInterface::insertStatementAfter(lastStatement,loopNest,autoMovePreprocessingInfo);

       // Mark this as compiler generated so that it will not be unparsed.
          associatedStatement->get_file_info()->setCompilerGenerated();

       // Form an array of AST subtrees to represent the different points in the stencil.
       // vector<SgFunctionCallExp*> stencilSubTreeArray;
          vector<SgExpression*> stencilSubTreeArray;
          for (size_t j = 0; j < stencilPointList.size(); j++)
             {
#if 0
               printf ("Forming stencil point subtree for offsetValues[0] = %3d [1] = %3d [2] = %3d \n",stencilPointList[j].first.offsetValues[0],stencilPointList[j].first.offsetValues[1],stencilPointList[j].first.offsetValues[2]);
#endif
               StencilOffsetFSM* stencilOffsetFSM = &(stencilPointList[j].first);
               double stencilCoeficient           = stencilPointList[j].second;

            // SgFunctionCallExp* stencilSubTree = buildStencilPoint(stencilOffsetFSM,stencilCoeficient,stencilFSM->stencilDimension());
               SgExpression* stencilSubTree = 
                    buildStencilPoint(stencilOffsetFSM,stencilCoeficient,stencilDimension,sourceVariableSymbol,
                         indexVariableSymbol_X,indexVariableSymbol_Y,indexVariableSymbol_Z,arraySizeVariableSymbol_X,arraySizeVariableSymbol_Y,generateLowlevelCode);

               ROSE_ASSERT(stencilSubTree != NULL);

               stencilSubTreeArray.push_back(stencilSubTree);
             }

       // Construct the lhs value for the stencil inner loop statement.
          StencilOffsetFSM* stencilOffsetFSM_lhs = new StencilOffsetFSM(0,0,0);
          double stencilCoeficient_lhs = 1.00;
          SgExpression* stencil_lhs = buildStencilPoint(stencilOffsetFSM_lhs,stencilCoeficient_lhs,stencilDimension,destinationVariableSymbol,
                                           indexVariableSymbol_X,indexVariableSymbol_Y,indexVariableSymbol_Z,arraySizeVariableSymbol_X,arraySizeVariableSymbol_Y,generateLowlevelCode);
          ROSE_ASSERT(stencil_lhs != NULL);

       // Assemble the stencilSubTreeArray into a single expression.
          SgExprStatement* stencilStatement = assembleStencilSubTreeArray(stencil_lhs,stencilSubTreeArray,stencilDimension,destinationVariableSymbol);
          SageInterface::appendStatement(stencilStatement,innerLoopBody);

          // Liao, 11/10/2014, further CUDA code generation if requested 
          if (b_gen_cuda)
          {
            if (stencilFSM->stencilDimension()==2)
            {
              // currently arraySize_X is generated inside the loop nest. No need to specify its sharing attribute.
              //string parallel_pragma_string= "omp parallel for shared (destinationDataPointer, sourceDataPointer, arraySize_X)"
              string parallel_pragma_string= "omp parallel for shared (destinationDataPointer, sourceDataPointer)";
              SgPragmaDeclaration* pragma1 = SageBuilder::buildPragmaDeclaration (parallel_pragma_string, NULL);
              SageInterface::insertStatementBefore(loopNest,  pragma1);
              // TODO: once total arraySize is calculated , we use a variable arraySize_Total instead of 1764 ( 42*42 )
              string target_pragma_string = "omp target device(0) map (out:destinationDataPointer[0:1764]) map(in:sourceDataPointer[0:1764])";
              SgPragmaDeclaration* pragma2 = SageBuilder::buildPragmaDeclaration (target_pragma_string, NULL);
              SageInterface::insertStatementBefore(pragma1,  pragma2);
            }
            else
            {
              std::cerr<<"Error, only 2-D CUDA generation is considered for now."<<std::endl;
              ROSE_ASSERT (false);
            }
          } 


        }
   }


