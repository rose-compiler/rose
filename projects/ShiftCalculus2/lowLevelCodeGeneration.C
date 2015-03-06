
// Example ROSE Translator reads input program and implements a DSL embedded within C++
// to support the stencil computations, and required runtime support is developed seperately.
#include "rose.h"

// #include "stencilAndStencilOperatorDetection.h"
// #include "stencilEvaluation.h"

// #include "shiftCalculusCompiler.h"

#include "stencilFiniteStateMachine.h"

#include "lowLevelCodeGeneration.h"

#include "dslSupport.h"


using namespace std;

using namespace DSL_Support;

SgExpression* buildStencilSubscript(std::vector<SgExpression*> operand, std::vector<SgExpression*> size, int dimSize)
{
  int i;
  SgExpression* subscript = NULL;
  for(i = dimSize; i >0; i--)
  {
    if(i == dimSize)
    {
      subscript = operand[i-1];
    }
    else
    {
      SgExpression* oldsubscript = subscript;
      subscript = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(size[i-1],oldsubscript),operand[i-1]);
    }
  }
  return subscript; 
}


// SgForStatement* buildLoopNest(int stencilDimension, SgBasicBlock* & innerLoopBody, SgVariableSymbol* sourceVariableSymbol,
//    SgVariableSymbol* & indexVariableSymbol_X, SgVariableSymbol* & indexVariableSymbol_Y, SgVariableSymbol* & indexVariableSymbol_Z, 
//    SgVariableSymbol* & arraySizeVariableSymbol_X, SgVariableSymbol* & arraySizeVariableSymbol_Y)
SgForStatement* buildLoopNest(int stencilDimension, SgBasicBlock* & innerLoopBody, SgVariableSymbol* boxVariableSymbol,
   SgVariableSymbol* & indexVariableSymbol_X, SgVariableSymbol* & indexVariableSymbol_Y, SgVariableSymbol* & indexVariableSymbol_Z, 
   SgVariableSymbol* & arraySizeVariableSymbol_X, SgVariableSymbol* & arraySizeVariableSymbol_Y, SgVariableSymbol* & arraySizeVariableSymbol_Z,  SgStatement* & anchorStatement)
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
     arraySizeNameList.push_back("arraySize_Z");

#if 0
  // We are now supported 3D stencils.
     if (stencilDimension != 2)
        {
          printf ("Sorry, code generation only implemented and tested for 2D stencils at present: stencilDimension = %d \n",stencilDimension);
          ROSE_ASSERT(false);
        }
#endif

  // This fails for the 0D identity stencil specification.
     ROSE_ASSERT(stencilDimension > 0);

     for (int k = 0; k < stencilDimension; k++)
        {
       // Add the scope early so that any possible declarations in the for loop initializer will be put into 
       // the correct scope and not use types or variables from the outer scope.
          SgForStatement* forStatementScope = new SgForStatement((SgStatement*)NULL,(SgExpression*)NULL,(SgStatement*)NULL);
          ROSE_ASSERT(forStatementScope != NULL);

          SgExpression* lowerBound_value = NULL;
          if (boxVariableSymbol != NULL)
             {
               SgExpression* lowerBound_value_part1  = buildMemberFunctionCall(boxVariableSymbol,"getLowCorner",NULL,false);
               ROSE_ASSERT(lowerBound_value_part1 != NULL);
               SgExpression* lowerBound_value_part3  = buildMemberFunctionCall(lowerBound_value_part1,"operator[]",SageBuilder::buildIntVal(indexValueList[k]),true);
               ROSE_ASSERT(lowerBound_value_part3 != NULL);
               std::string lbname= "lb" + std::to_string(indexValueList[k]);
               SgVariableDeclaration* lbDecl = SageBuilder::buildVariableDeclaration(lbname,SageBuilder::buildIntType(),SageBuilder::buildAssignInitializer(lowerBound_value_part3,SageBuilder::buildIntType()),anchorStatement->get_scope());
               SageInterface::insertStatementAfter(anchorStatement,lbDecl,anchorStatement->get_scope());
               anchorStatement = lbDecl;
//               lowerBound_value = lowerBound_value_part3;
               lowerBound_value = SageBuilder::buildVarRefExp(lbname,anchorStatement->get_scope());
             }
            else
             {
            // In the Maple DSL we don't have the same boxVariableSymbol API.
               lowerBound_value = SageBuilder::buildIntVal(1);
             }

       // ROSE_DLL_API SgVariableDeclaration* buildVariableDeclaration_nfi(const SgName & name, SgType *type, SgInitializer *varInit, SgScopeStatement* scope);
       // ROSE_DLL_API SgAssignInitializer * buildAssignInitializer_nfi(SgExpression * operand_i = NULL, SgType * expression_type = NULL);
       // SgAssignInitializer* assignInitializer = SageBuilder::buildAssignInitializer_nfi(SageBuilder::buildIntVal(1));
          SgAssignInitializer* assignInitializer = SageBuilder::buildAssignInitializer_nfi(SageBuilder::buildIntVal());
          ROSE_ASSERT(assignInitializer != NULL);

          SgVariableDeclaration* variableDeclaration  = SageBuilder::buildVariableDeclaration_nfi(indexNameList[k],SageBuilder::buildIntType(),assignInitializer,anchorStatement->get_scope());
          ROSE_ASSERT(variableDeclaration != NULL);
       // Pei-Hung: Normalize the loop in code generation
          SageInterface::insertStatementAfter(anchorStatement,variableDeclaration,anchorStatement->get_scope());
          anchorStatement = variableDeclaration;

          // SgForInitStatement* for_loop_initialization = SageBuilder::buildForInitStatement(variableDeclaration);
          SgExprStatement* loopInit = SageBuilder::buildAssignStatement(SageBuilder::buildVarRefExp(variableDeclaration),lowerBound_value);
          SgForInitStatement* for_loop_initialization = SageBuilder::buildForInitStatement(loopInit);
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

          SgExpression* upperBound_value = NULL;
          if (boxVariableSymbol != NULL)
             {
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
               std::string ubname= "ub" + std::to_string(indexValueList[k]);
               SgVariableDeclaration* ubDecl = SageBuilder::buildVariableDeclaration(ubname,SageBuilder::buildIntType(),SageBuilder::buildAssignInitializer(upperBound_value_part3,SageBuilder::buildIntType()),anchorStatement->get_scope());
               SageInterface::insertStatementAfter(anchorStatement,ubDecl,anchorStatement->get_scope());
               anchorStatement = ubDecl;
               //upperBound_value = upperBound_value_part3;
               upperBound_value = SageBuilder::buildVarRefExp(ubname,anchorStatement->get_scope());;
             }
            else
             {
            // In the Maple DSL we don't have the same boxVariableSymbol API.
               upperBound_value = SageBuilder::buildIntVal(42);
             }

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
       
          // Pei-Hung, the size in dimension n can be retrieved from box(n).
          assignInitializer = SageBuilder::buildAssignInitializer_nfi(buildMemberFunctionCall(boxVariableSymbol,"size",SageBuilder::buildIntVal(k),false));
          ROSE_ASSERT(assignInitializer != NULL);
          SgVariableDeclaration* arraySizeVariableDeclaration  = SageBuilder::buildVariableDeclaration_nfi(arraySizeNameList[k],SageBuilder::buildIntType(),assignInitializer,anchorStatement->get_scope());
          ROSE_ASSERT(variableDeclaration != NULL);
          SageInterface::insertStatementAfter(anchorStatement,arraySizeVariableDeclaration,anchorStatement->get_scope());
          anchorStatement = arraySizeVariableDeclaration;
          if (k == 0)
             {
            // Save the outer most forStatementScope.
               loopNest = forStatementScope;
               currentLoopBody = loopBody;
               if (b_gen_vectorization)
               {
                   string scop_pragma_string;
                   scop_pragma_string = "SIMD";
                   SgPragmaDeclaration* pragma = SageBuilder::buildPragmaDeclaration (scop_pragma_string, NULL);
                   SageInterface::insertStatementBefore(loopNest,  pragma); 
               }
               arraySizeVariableSymbol_X = SageInterface::getFirstVarSym (arraySizeVariableDeclaration);
             }
            else
             {
            // This is not the correct expression in the SgAssignInitializer (fix this later).
               //SgAssignInitializer* assignInitializer = SageBuilder::buildAssignInitializer_nfi(SageBuilder::buildIntVal(42));


               if (k == 1) arraySizeVariableSymbol_Y = SageInterface::getFirstVarSym (arraySizeVariableDeclaration);
               if (k == 2) arraySizeVariableSymbol_Z = SageInterface::getFirstVarSym (arraySizeVariableDeclaration);

            // Add the variable declaration of the array size in the previous dimension.

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
   SgVariableSymbol* arraySizeVariableSymbol_X, SgVariableSymbol* arraySizeVariableSymbol_Y, SgVariableSymbol* arraySizeVariableSymbol_z, bool generateLowlevelCode)
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
       // Pei-Hung: vector to store operands and array size
          std::vector<SgExpression*> operand;
          std::vector<SgExpression*> arraySize;
          arraySize.push_back(arraySizeVarRefExp_X);

       // For the index expression.
          SgExpression* expression = NULL;
          if (stencilOffsetFSM->offsetValues[0] == 0 && stencilOffsetFSM->offsetValues[1] == 0)
             {
            // This is a center point in the stencil
            // We want to generate: source[j*axis_x_size+i]
            // Pei-Hung: replace with a subscript building function
               //expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(indexVarRefExp_Y,arraySizeVarRefExp_X), indexVarRefExp_X);
               operand.push_back(indexVarRefExp_X);
               operand.push_back(indexVarRefExp_Y);
               expression = buildStencilSubscript(operand,arraySize,operand.size());
             }
            else
             {
            // This is a stencil point offset from the center of the stencil (so it has a binary operator associated with the index expression).

            // We want to generate: "source[(j + offset_x)*axis_x_size+i + offset_y]"
               if (stencilOffsetFSM->offsetValues[0] == 0 && stencilOffsetFSM->offsetValues[1] != 0)
                  {
                    SgExpression* offsetValue_Y = SageBuilder::buildIntVal(stencilOffsetFSM->offsetValues[1]);
                 // Pei-Hung: replace with a subscript building function
                    //expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(SageBuilder::buildAddOp(indexVarRefExp_Y,offsetValue_Y),arraySizeVarRefExp_X), indexVarRefExp_X);
                    operand.push_back(indexVarRefExp_X);
                    operand.push_back(SageBuilder::buildAddOp(indexVarRefExp_Y,offsetValue_Y));
                    expression = buildStencilSubscript(operand,arraySize,operand.size());
                  }
                 else
                  {
                    ROSE_ASSERT(stencilOffsetFSM->offsetValues[0] != 0);
                    if (stencilOffsetFSM->offsetValues[0] != 0 && stencilOffsetFSM->offsetValues[1] == 0)
                       {
                         SgExpression* offsetValue_X = SageBuilder::buildIntVal(stencilOffsetFSM->offsetValues[0]);
                         // Pei-Hung: replace with a subscript building function
                         //expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(indexVarRefExp_Y,arraySizeVarRefExp_X), SageBuilder::buildAddOp(indexVarRefExp_X,offsetValue_X));
                         operand.push_back(SageBuilder::buildAddOp(indexVarRefExp_X,offsetValue_X));
                         operand.push_back(indexVarRefExp_Y);
                         expression = buildStencilSubscript(operand,arraySize,operand.size());
                       }
                      else
                       {
                         ROSE_ASSERT(stencilOffsetFSM->offsetValues[0] != 0);
                         ROSE_ASSERT(stencilOffsetFSM->offsetValues[1] != 0);

                         SgExpression* offsetValue_X = SageBuilder::buildIntVal(stencilOffsetFSM->offsetValues[0]);
                         SgExpression* offsetValue_Y = SageBuilder::buildIntVal(stencilOffsetFSM->offsetValues[1]);
                         // Pei-Hung: replace with a subscript building function
                         //expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(SageBuilder::buildAddOp(indexVarRefExp_Y,offsetValue_Y),arraySizeVarRefExp_X), SageBuilder::buildAddOp(indexVarRefExp_X,offsetValue_X));
                         operand.push_back(SageBuilder::buildAddOp(indexVarRefExp_X,offsetValue_X));
                         operand.push_back(SageBuilder::buildAddOp(indexVarRefExp_Y,offsetValue_Y));
                         expression = buildStencilSubscript(operand,arraySize,operand.size());
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
#if 0
       else
        {
          printf ("Error: Dimensions other than 2 not yet supported! \n");
          ROSE_ASSERT(false);
        }
#endif

     if (stencilDimension == 3)
        {
#if 0
          printf ("In buildStencilPoint(): Building stencil point for 3D stencil: stencilCoeficient = %3.2f \n",stencilCoeficient);
#endif
          ROSE_ASSERT(indexVariableSymbol_X != NULL);
          ROSE_ASSERT(indexVariableSymbol_Y != NULL);
          ROSE_ASSERT(indexVariableSymbol_Z != NULL);

          ROSE_ASSERT(arraySizeVariableSymbol_X != NULL);
          ROSE_ASSERT(arraySizeVariableSymbol_Y != NULL);

       // We need to generate the linearized index expression for the "double RectMDArray::operator[](int index)".  To do this
       // we need an expresion for the x_axis_size (and for y_axis_size for 3D stencils).

       // SgVarRefExp* variableVarRefExp    = SageBuilder::buildVarRefExp(variableSymbol);
          SgVarRefExp* indexVarRefExp_Z     = SageBuilder::buildVarRefExp(indexVariableSymbol_Z);
          SgVarRefExp* arraySizeVarRefExp_Y = SageBuilder::buildVarRefExp(arraySizeVariableSymbol_Y);
          SgVarRefExp* indexVarRefExp_Y     = SageBuilder::buildVarRefExp(indexVariableSymbol_Y);
          SgVarRefExp* arraySizeVarRefExp_X = SageBuilder::buildVarRefExp(arraySizeVariableSymbol_X);
          SgVarRefExp* indexVarRefExp_X     = SageBuilder::buildVarRefExp(indexVariableSymbol_X);

       // Pei-Hung: vector to store operands and array size
          std::vector<SgExpression*> operand;
          std::vector<SgExpression*> arraySize;
          arraySize.push_back(arraySizeVarRefExp_X);
          arraySize.push_back(arraySizeVarRefExp_Y);

       // For the index expression.
          SgExpression* expression = NULL;
          if (stencilOffsetFSM->offsetValues[0] == 0 && stencilOffsetFSM->offsetValues[1] == 0 && stencilOffsetFSM->offsetValues[2] == 0)
             {
            // This is a center point in the stencil
            // We want to generate: source[(k*axis_y_size)+j*axis_x_size+i]
            // expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(indexVarRefExp_Y,arraySizeVarRefExp_X), indexVarRefExp_X);
            // Pei-Hung: replace with a subscript building function
               //expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(indexVarRefExp_Z,arraySizeVarRefExp_Y),SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(indexVarRefExp_Y,arraySizeVarRefExp_X), indexVarRefExp_X) );
               operand.push_back(indexVarRefExp_X);
               operand.push_back(indexVarRefExp_Y);
               operand.push_back(indexVarRefExp_Z);
               expression = buildStencilSubscript(operand,arraySize,operand.size());
             }
            else
             {
            // This is a stencil point offset from the center of the stencil (so it has a binary operator associated with the index expression).

            // We want to generate: "source[(j + offset_x)*axis_x_size+i + offset_y]"
               if (stencilOffsetFSM->offsetValues[0] == 0 && stencilOffsetFSM->offsetValues[1] != 0 && stencilOffsetFSM->offsetValues[2] == 0)
                  {
                    SgExpression* offsetValue_Y = SageBuilder::buildIntVal(stencilOffsetFSM->offsetValues[1]);
                 // expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(SageBuilder::buildAddOp(indexVarRefExp_Y,offsetValue_Y),arraySizeVarRefExp_X), indexVarRefExp_X);
                 // Pei-Hung: replace with a subscript building function
                 // expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(indexVarRefExp_Z,arraySizeVarRefExp_Y),
                 //                                      SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(SageBuilder::buildAddOp(indexVarRefExp_Y,offsetValue_Y),arraySizeVarRefExp_X), indexVarRefExp_X));
                    operand.push_back(indexVarRefExp_X);
                    operand.push_back(SageBuilder::buildAddOp(indexVarRefExp_Y,offsetValue_Y));
                    operand.push_back(indexVarRefExp_Z);
                    expression = buildStencilSubscript(operand,arraySize,operand.size());
                  }
                 else
                  {
                    ROSE_ASSERT(stencilOffsetFSM->offsetValues[0] != 0 || stencilOffsetFSM->offsetValues[2] != 0);

                    if (stencilOffsetFSM->offsetValues[0] == 0 && stencilOffsetFSM->offsetValues[1] == 0 && stencilOffsetFSM->offsetValues[2] != 0)
                       {
                         SgExpression* offsetValue_Z = SageBuilder::buildIntVal(stencilOffsetFSM->offsetValues[2]);
                      // expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(SageBuilder::buildAddOp(indexVarRefExp_Y,offsetValue_Y),arraySizeVarRefExp_X), indexVarRefExp_X);
                      // Pei-Hung: replace with a subscript building function
                      //   expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(SageBuilder::buildAddOp(indexVarRefExp_Z,offsetValue_Z),arraySizeVarRefExp_Y),
                      //                                        SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(indexVarRefExp_Y,arraySizeVarRefExp_X), indexVarRefExp_X));
                         operand.push_back(indexVarRefExp_X);
                         operand.push_back(indexVarRefExp_Y);
                         operand.push_back(SageBuilder::buildAddOp(indexVarRefExp_Z,offsetValue_Z));
                         expression = buildStencilSubscript(operand,arraySize,operand.size());
                       }
                      else
                       {
                         ROSE_ASSERT(stencilOffsetFSM->offsetValues[0] != 0);

                         if (stencilOffsetFSM->offsetValues[0] != 0 && stencilOffsetFSM->offsetValues[1] == 0 && stencilOffsetFSM->offsetValues[2] == 0)
                            {
                              SgExpression* offsetValue_X = SageBuilder::buildIntVal(stencilOffsetFSM->offsetValues[0]);
                           // Pei-Hung: replace with a subscript building function
                           // expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(indexVarRefExp_Y,arraySizeVarRefExp_X), SageBuilder::buildAddOp(indexVarRefExp_X,offsetValue_X));
                          //    expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(indexVarRefExp_Z,arraySizeVarRefExp_Y),
                          //                                         SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(indexVarRefExp_Y,arraySizeVarRefExp_X), 
                          //                                                                 SageBuilder::buildAddOp(indexVarRefExp_X,offsetValue_X)));
                              operand.push_back(SageBuilder::buildAddOp(indexVarRefExp_X,offsetValue_X));
                              operand.push_back(indexVarRefExp_Y);
                              operand.push_back(indexVarRefExp_Z);
                              expression = buildStencilSubscript(operand,arraySize,operand.size());
                            }
                           else
                            {
                           // This is still an opportunity to handle the case where either one of the offset axis is zero.

                           // This has to be the more general case.
                              ROSE_ASSERT(stencilOffsetFSM->offsetValues[0] != 0);
                              ROSE_ASSERT(stencilOffsetFSM->offsetValues[1] != 0);
                              ROSE_ASSERT(stencilOffsetFSM->offsetValues[2] != 0);

                              SgExpression* offsetValue_X = SageBuilder::buildIntVal(stencilOffsetFSM->offsetValues[0]);
                              SgExpression* offsetValue_Y = SageBuilder::buildIntVal(stencilOffsetFSM->offsetValues[1]);
                              SgExpression* offsetValue_Z = SageBuilder::buildIntVal(stencilOffsetFSM->offsetValues[2]);
                           // expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(SageBuilder::buildAddOp(indexVarRefExp_Y,offsetValue_Y),arraySizeVarRefExp_X), SageBuilder::buildAddOp(indexVarRefExp_X,offsetValue_X));
                           // Pei-Hung: replace with a subscript building function
                           //   expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(SageBuilder::buildAddOp(indexVarRefExp_Z,offsetValue_Z),arraySizeVarRefExp_Y),
                           //                                        SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(SageBuilder::buildAddOp(indexVarRefExp_Y,offsetValue_Y),arraySizeVarRefExp_X), 
                           //                                                                SageBuilder::buildAddOp(indexVarRefExp_X,offsetValue_X)));
                              operand.push_back(SageBuilder::buildAddOp(indexVarRefExp_X,offsetValue_X));
                              operand.push_back(SageBuilder::buildAddOp(indexVarRefExp_Y,offsetValue_Y));
                              operand.push_back(SageBuilder::buildAddOp(indexVarRefExp_Z,offsetValue_Z));
                              expression = buildStencilSubscript(operand,arraySize,operand.size());
                            }
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
#if 0
       else
        {
          printf ("Error: Dimensions other than 3 not yet supported! \n");
          ROSE_ASSERT(false);
        }
#endif

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
SgExpression* 
buildMultiDimStencilPoint (StencilOffsetFSM* stencilOffsetFSM, double stencilCoeficient, int stencilDimension, SgVariableSymbol* variableSymbol, 
   SgVariableSymbol* indexVariableSymbol_X, SgVariableSymbol* indexVariableSymbol_Y, SgVariableSymbol* indexVariableSymbol_Z, 
   SgVariableSymbol* arraySizeVariableSymbol_X, SgVariableSymbol* arraySizeVariableSymbol_Y, SgVariableSymbol* arraySizeVariableSymbol_Z, bool generateLowlevelCode)
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
          SgVarRefExp* indexVarRefExp_X     = SageBuilder::buildVarRefExp(indexVariableSymbol_X);
       // Pei-Hung: vector to store operands and array size
          std::vector<SgExpression*> operand;

       // For the index expression.
          SgExpression* expression = NULL;
          if (stencilOffsetFSM->offsetValues[0] == 0 && stencilOffsetFSM->offsetValues[1] == 0)
             {
            // This is a center point in the stencil
            // We want to generate: source[j*axis_x_size+i]
               //expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(indexVarRefExp_Y,arraySizeVarRefExp_X), indexVarRefExp_X);
               operand.push_back(indexVarRefExp_X);
               operand.push_back(indexVarRefExp_Y);
             }
            else
             {
            // This is a stencil point offset from the center of the stencil (so it has a binary operator associated with the index expression).

            // We want to generate: "source[(j + offset_x)*axis_x_size+i + offset_y]"
               if (stencilOffsetFSM->offsetValues[0] == 0 && stencilOffsetFSM->offsetValues[1] != 0)
                  {
                    SgExpression* offsetValue_Y = SageBuilder::buildIntVal(stencilOffsetFSM->offsetValues[1]);
                    //expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(SageBuilder::buildAddOp(indexVarRefExp_Y,offsetValue_Y),arraySizeVarRefExp_X), indexVarRefExp_X);
                    operand.push_back(indexVarRefExp_X);
                    operand.push_back(SageBuilder::buildAddOp(indexVarRefExp_Y,offsetValue_Y));
                  }
                 else
                  {
                    ROSE_ASSERT(stencilOffsetFSM->offsetValues[0] != 0);
                    if (stencilOffsetFSM->offsetValues[0] != 0 && stencilOffsetFSM->offsetValues[1] == 0)
                       {
                         SgExpression* offsetValue_X = SageBuilder::buildIntVal(stencilOffsetFSM->offsetValues[0]);
                         //expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(indexVarRefExp_Y,arraySizeVarRefExp_X), SageBuilder::buildAddOp(indexVarRefExp_X,offsetValue_X));
                         operand.push_back(SageBuilder::buildAddOp(indexVarRefExp_X,offsetValue_X));
                         operand.push_back(indexVarRefExp_Y);
                       }
                      else
                       {
                         ROSE_ASSERT(stencilOffsetFSM->offsetValues[0] != 0);
                         ROSE_ASSERT(stencilOffsetFSM->offsetValues[1] != 0);

                         SgExpression* offsetValue_X = SageBuilder::buildIntVal(stencilOffsetFSM->offsetValues[0]);
                         SgExpression* offsetValue_Y = SageBuilder::buildIntVal(stencilOffsetFSM->offsetValues[1]);
                         //expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(SageBuilder::buildAddOp(indexVarRefExp_Y,offsetValue_Y),arraySizeVarRefExp_X), SageBuilder::buildAddOp(indexVarRefExp_X,offsetValue_X));
                         operand.push_back(SageBuilder::buildAddOp(indexVarRefExp_X,offsetValue_X));
                         operand.push_back(SageBuilder::buildAddOp(indexVarRefExp_Y,offsetValue_Y));
                       }
                  }
             }

          SgExpression* dataReferenceExpression = NULL;
          if (generateLowlevelCode == true)
             {
               SgVarRefExp* arrayPointerVarRefExp = SageBuilder::buildVarRefExp(variableSymbol);
               SgPntrArrRefExp* arrayRefExp       = SageBuilder::buildPntrArrRefExp(arrayPointerVarRefExp,operand[0]);
               for(int id = 1; id < stencilDimension ; ++id)
               {
                  arrayRefExp       = SageBuilder::buildPntrArrRefExp(arrayRefExp,operand[id]);
               }
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
#if 0
       else
        {
          printf ("Error: Dimensions other than 2 not yet supported! \n");
          ROSE_ASSERT(false);
        }
#endif

     if (stencilDimension == 3)
        {
#if 0
          printf ("In buildStencilPoint(): Building stencil point for 3D stencil: stencilCoeficient = %3.2f \n",stencilCoeficient);
#endif
          ROSE_ASSERT(indexVariableSymbol_X != NULL);
          ROSE_ASSERT(indexVariableSymbol_Y != NULL);
          ROSE_ASSERT(indexVariableSymbol_Z != NULL);

          ROSE_ASSERT(arraySizeVariableSymbol_X != NULL);
          ROSE_ASSERT(arraySizeVariableSymbol_Y != NULL);

       // We need to generate the linearized index expression for the "double RectMDArray::operator[](int index)".  To do this
       // we need an expresion for the x_axis_size (and for y_axis_size for 3D stencils).

       // SgVarRefExp* variableVarRefExp    = SageBuilder::buildVarRefExp(variableSymbol);
          SgVarRefExp* indexVarRefExp_Z     = SageBuilder::buildVarRefExp(indexVariableSymbol_Z);
          SgVarRefExp* indexVarRefExp_Y     = SageBuilder::buildVarRefExp(indexVariableSymbol_Y);
          SgVarRefExp* indexVarRefExp_X     = SageBuilder::buildVarRefExp(indexVariableSymbol_X);

       // Pei-Hung: vector to store operands and array size
          std::vector<SgExpression*> operand;

       // For the index expression.
          SgExpression* expression = NULL;
          if (stencilOffsetFSM->offsetValues[0] == 0 && stencilOffsetFSM->offsetValues[1] == 0 && stencilOffsetFSM->offsetValues[2] == 0)
             {
            // This is a center point in the stencil
            // We want to generate: source[(k*axis_y_size)+j*axis_x_size+i]
            // expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(indexVarRefExp_Y,arraySizeVarRefExp_X), indexVarRefExp_X);
               //expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(indexVarRefExp_Z,arraySizeVarRefExp_Y),SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(indexVarRefExp_Y,arraySizeVarRefExp_X), indexVarRefExp_X) );
               operand.push_back(indexVarRefExp_X);
               operand.push_back(indexVarRefExp_Y);
               operand.push_back(indexVarRefExp_Z);
             }
            else
             {
            // This is a stencil point offset from the center of the stencil (so it has a binary operator associated with the index expression).

            // We want to generate: "source[(j + offset_x)*axis_x_size+i + offset_y]"
               if (stencilOffsetFSM->offsetValues[0] == 0 && stencilOffsetFSM->offsetValues[1] != 0 && stencilOffsetFSM->offsetValues[2] == 0)
                  {
                    SgExpression* offsetValue_Y = SageBuilder::buildIntVal(stencilOffsetFSM->offsetValues[1]);
                 // expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(SageBuilder::buildAddOp(indexVarRefExp_Y,offsetValue_Y),arraySizeVarRefExp_X), indexVarRefExp_X);
                 // expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(indexVarRefExp_Z,arraySizeVarRefExp_Y),
                 //                                      SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(SageBuilder::buildAddOp(indexVarRefExp_Y,offsetValue_Y),arraySizeVarRefExp_X), indexVarRefExp_X));
                    operand.push_back(indexVarRefExp_X);
                    operand.push_back(SageBuilder::buildAddOp(indexVarRefExp_Y,offsetValue_Y));
                    operand.push_back(indexVarRefExp_Z);
                  }
                 else
                  {
                    ROSE_ASSERT(stencilOffsetFSM->offsetValues[0] != 0 || stencilOffsetFSM->offsetValues[2] != 0);

                    if (stencilOffsetFSM->offsetValues[0] == 0 && stencilOffsetFSM->offsetValues[1] == 0 && stencilOffsetFSM->offsetValues[2] != 0)
                       {
                         SgExpression* offsetValue_Z = SageBuilder::buildIntVal(stencilOffsetFSM->offsetValues[2]);
                      // expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(SageBuilder::buildAddOp(indexVarRefExp_Y,offsetValue_Y),arraySizeVarRefExp_X), indexVarRefExp_X);
                      //   expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(SageBuilder::buildAddOp(indexVarRefExp_Z,offsetValue_Z),arraySizeVarRefExp_Y),
                      //                                        SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(indexVarRefExp_Y,arraySizeVarRefExp_X), indexVarRefExp_X));
                         operand.push_back(indexVarRefExp_X);
                         operand.push_back(indexVarRefExp_Y);
                         operand.push_back(SageBuilder::buildAddOp(indexVarRefExp_Z,offsetValue_Z));
                       }
                      else
                       {
                         ROSE_ASSERT(stencilOffsetFSM->offsetValues[0] != 0);

                         if (stencilOffsetFSM->offsetValues[0] != 0 && stencilOffsetFSM->offsetValues[1] == 0 && stencilOffsetFSM->offsetValues[2] == 0)
                            {
                              SgExpression* offsetValue_X = SageBuilder::buildIntVal(stencilOffsetFSM->offsetValues[0]);
                           // expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(indexVarRefExp_Y,arraySizeVarRefExp_X), SageBuilder::buildAddOp(indexVarRefExp_X,offsetValue_X));
                          //    expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(indexVarRefExp_Z,arraySizeVarRefExp_Y),
                          //                                         SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(indexVarRefExp_Y,arraySizeVarRefExp_X), 
                          //                                                                 SageBuilder::buildAddOp(indexVarRefExp_X,offsetValue_X)));
                              operand.push_back(SageBuilder::buildAddOp(indexVarRefExp_X,offsetValue_X));
                              operand.push_back(indexVarRefExp_Y);
                              operand.push_back(indexVarRefExp_Z);
                            }
                           else
                            {
                           // This is still an opportunity to handle the case where either one of the offset axis is zero.

                           // This has to be the more general case.
                              ROSE_ASSERT(stencilOffsetFSM->offsetValues[0] != 0);
                              ROSE_ASSERT(stencilOffsetFSM->offsetValues[1] != 0);
                              ROSE_ASSERT(stencilOffsetFSM->offsetValues[2] != 0);

                              SgExpression* offsetValue_X = SageBuilder::buildIntVal(stencilOffsetFSM->offsetValues[0]);
                              SgExpression* offsetValue_Y = SageBuilder::buildIntVal(stencilOffsetFSM->offsetValues[1]);
                              SgExpression* offsetValue_Z = SageBuilder::buildIntVal(stencilOffsetFSM->offsetValues[2]);
                           // expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(SageBuilder::buildAddOp(indexVarRefExp_Y,offsetValue_Y),arraySizeVarRefExp_X), SageBuilder::buildAddOp(indexVarRefExp_X,offsetValue_X));
                           //   expression = SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(SageBuilder::buildAddOp(indexVarRefExp_Z,offsetValue_Z),arraySizeVarRefExp_Y),
                           //                                        SageBuilder::buildAddOp(SageBuilder::buildMultiplyOp(SageBuilder::buildAddOp(indexVarRefExp_Y,offsetValue_Y),arraySizeVarRefExp_X), 
                           //                                                                SageBuilder::buildAddOp(indexVarRefExp_X,offsetValue_X)));
                              operand.push_back(SageBuilder::buildAddOp(indexVarRefExp_X,offsetValue_X));
                              operand.push_back(SageBuilder::buildAddOp(indexVarRefExp_Y,offsetValue_Y));
                              operand.push_back(SageBuilder::buildAddOp(indexVarRefExp_Z,offsetValue_Z));
                            }
                       }
                  }
             }

          SgExpression* dataReferenceExpression = NULL;
          if (generateLowlevelCode == true)
             {
               SgVarRefExp* arrayPointerVarRefExp = SageBuilder::buildVarRefExp(variableSymbol);
               SgPntrArrRefExp* arrayRefExp       = SageBuilder::buildPntrArrRefExp(arrayPointerVarRefExp,operand[stencilDimension-1]);
               for(int id = stencilDimension-2; id >= 0 ; --id)
               {
                  arrayRefExp       = SageBuilder::buildPntrArrRefExp(arrayRefExp,operand[id]);
               }
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
#if 0
       else
        {
          printf ("Error: Dimensions other than 3 not yet supported! \n");
          ROSE_ASSERT(false);
        }
#endif

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

