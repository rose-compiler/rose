
#include "rose.h"

#include "operatorFiniteStateMachine.h"

#include "operatorEvaluation.h"

#include "mapleDSLCompiler.h"

#include "mapleOperatorAPI.h"

#include "dslCodeGeneration.h"

#include "dslSupport.h"

#include "lowLevelCodeGeneration.h"

using namespace std;

using namespace DSL_Support;


string
generateMapleCode(SgExpression* lhs, SgExpression* rhs, OperatorFSM discritizationOperator, bool generateLowlevelCode)
   {
#if 0
     printf ("In generateMapleCode(): \n");
     printf ("   --- lhs = %p \n",lhs);
     printf ("   --- rhs = %p \n",rhs);
     discritizationOperator.display("In generateMapleCode(): discritizationOperator");
#endif

  // Maple code generation (this is the code we want to generate).
     string exampleOutputCode = "Lap2D := w -> dpx(dmx(w))+dpy(dmy(w)):";

  // This name has to be consistant with what is expected in the Maple worksheet we use as a template.
     string gridFunctionName = "w";

  // We need to use the name already referenced in the Maple template file.
  // string operatorName     = discritizationOperator.operatorName;
     string operatorName     = "Lap2D";

     string generatedCode = operatorName + " := " + gridFunctionName + " -> ";

  // printf ("   --- discritizationOperator.discretizationList (size = %zu): \n",discritizationOperator.discretizationList.size());
     for (size_t i = 0; i < discritizationOperator.discretizationList.size(); i++)
        {
#if 0
          printf ("   --- discretizationList[%zu]: \n",i);
          discritizationOperator.discretizationList[i].display("DiscretizationFSM element");
#endif
       // If there is another discritizationOperator operator in the list then add it to this one.
          if (i > 0)
             {
               generatedCode += "+";
             }

          generatedCode += discritizationOperator.discretizationList[i].discretizationName;
          if (discritizationOperator.discretizationList[i].builtFromOperatorParens == true)
             {
               generatedCode += "(" + discritizationOperator.discretizationList[i].operatorParensArgument->discretizationName + "(" + gridFunctionName + "))";
             }
        }
     generatedCode += ":";

     printf ("generated Maple code string: generatedCode     = %s \n",generatedCode.c_str());
     printf ("expected Maple code string:  exampleOutputCode = %s \n",exampleOutputCode.c_str());

  // printf ("Sorry, Maple code generation for Maple DSL not completed yet! \n");
  // ROSE_ASSERT(false);


  // callMaple(generatedCode);

     printf ("Leaving generateMapleCode() \n");

     return generatedCode;
   }


std::vector<std::pair<StencilOffsetFSM,double> > 
buildStencilPointList(StencilOperator X, SgVariableSymbol* sourceVariableSymbol, 
     SgVariableSymbol* indexVariableSymbol_X, SgVariableSymbol* indexVariableSymbol_Y, SgVariableSymbol* indexVariableSymbol_Z, 
     SgVariableSymbol* arraySizeVariableSymbol_X, SgVariableSymbol* arraySizeVariableSymbol_Y)
   {
  // This is using the data structure developed for the Shift Calculus DSL so that we can share the
  // code generation between the two DSLs.
     std::vector<std::pair<StencilOffsetFSM,double> > stencilPointList;

#if 0
     printf ("In buildStencilPointList(): stencil point list not computed yet \n");
     ROSE_ASSERT(false);
#endif

     int stencilDimension = 2;

#if 0
     SgVariableSymbol* sourceVariableSymbol = NULL;
     SgVariableSymbol* indexVariableSymbol_X = NULL;
     SgVariableSymbol* indexVariableSymbol_Y = NULL;
     SgVariableSymbol* indexVariableSymbol_Z = NULL;
     SgVariableSymbol* arraySizeVariableSymbol_X = NULL;
     SgVariableSymbol* arraySizeVariableSymbol_Y = NULL;
#endif

     bool generateLowlevelCode = true;

     int size_z = X.stencil.size();
     int size_y = X.stencil[0].size();
     int size_x = X.stencil[0][0].size();

     printf ("In buildStencilPointList(): size_x = %d size_y = %d size_z = %d \n",size_x,size_y,size_z);

     ROSE_ASSERT(sourceVariableSymbol != NULL);

     ROSE_ASSERT(indexVariableSymbol_X != NULL);
     ROSE_ASSERT(indexVariableSymbol_Y != NULL);
  // ROSE_ASSERT(indexVariableSymbol_Z != NULL);

     ROSE_ASSERT(arraySizeVariableSymbol_X != NULL);
  // ROSE_ASSERT(arraySizeVariableSymbol_Y != NULL);

     for (int k = 0; k < size_z; k++)
        {
          for (int j = 0; j < size_y; j++)
             {
               for (int i = 0; i < size_x; i++)
                  {
                 // X.stencil[k][j][i].isNonzero = false;
                 // X.stencil[k][j][i].coefficientValue = 0.0;

                    printf ("In buildStencilPointList(): In loop: stencil point(%d,%d,%d) isNonzero = %s coefficientValue = %4.2f \n",k,j,i,X.stencil[k][j][i].isNonzero ? "true" : "false",X.stencil[k][j][i].coefficientValue);

                    if (X.stencil[k][j][i].isNonzero == true)
                       {
                      // Construct the stencil points.
                         StencilOffsetFSM* stencilOffsetFSM = new StencilOffsetFSM(i,j,k);
                         double stencilCoeficient = X.stencil[k][j][i].coefficientValue;
                      // SgExpression* stencilPoint = buildStencilPoint(stencilOffsetFSM,stencilCoeficient,stencilDimension,sourceVariableSymbol,
                      //                                   indexVariableSymbol_X,indexVariableSymbol_Y,indexVariableSymbol_Z,arraySizeVariableSymbol_X,arraySizeVariableSymbol_Y,generateLowlevelCode);
                      // ROSE_ASSERT(stencilPoint != NULL);

                         stencilPointList.push_back(pair<StencilOffsetFSM,double>(*stencilOffsetFSM,stencilCoeficient));
                       }
                  }
             }
        }

     printf ("Leaving buildStencilPointList(): stencilPointList.size() = %zu \n",stencilPointList.size());

#if 0
     printf ("In buildStencilPointList(): stencil point list not computed yet \n");
     ROSE_ASSERT(false);
#endif

     return stencilPointList;
   }


void
generateStencilCode(StencilOperator X, SgExpression* lhs, SgExpression* rhs)
   {
     printf ("Generate ROSE AST for low level code for stencil: \n");
     printf ("   --- size_x = %zu \n",X.stencil[0][0].size());
     printf ("   --- size_y = %zu \n",X.stencil[0].size());
     printf ("   --- size_z = %zu \n",X.stencil.size());

  // Mark this true for now and pass it to the API upon more consideration.
     bool generateLowlevelCode = true;

  // Hard code this for now.
     int stencilDimension = 2;

     ROSE_ASSERT(lhs != NULL);
     ROSE_ASSERT(rhs != NULL);

     printf ("   --- lhs = %p = %s \n",lhs,lhs->class_name().c_str());
     printf ("   --- rhs = %p = %s \n",rhs,rhs->class_name().c_str());

     SgVarRefExp* destinationArrayVarRefExp = isSgVarRefExp(lhs);
     ROSE_ASSERT(destinationArrayVarRefExp != NULL);

     SgVarRefExp* sourceArrayVarRefExp = isSgVarRefExp(rhs);
     ROSE_ASSERT(sourceArrayVarRefExp != NULL);

     SgStatement* associatedStatement = TransformationSupport::getStatement(lhs);
     ROSE_ASSERT(associatedStatement != NULL);

     string filename = associatedStatement->get_file_info()->get_filename();
     int lineNumber  = associatedStatement->get_file_info()->get_line();

     printf ("Generating code for stencil operator used at file = %s at line = %d \n",filename.c_str(),lineNumber);

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
  // SgVariableSymbol* boxVariableSymbol = boxVarRefExp->get_symbol();
  // ROSE_ASSERT(boxVariableSymbol != NULL);
     SgVariableSymbol* boxVariableSymbol = NULL;

  // This can be important in handling of comments and CPP directives.
     bool autoMovePreprocessingInfo = true;

     SgStatement* lastStatement = associatedStatement;
     if (generateLowlevelCode == true)
        {
          SgVariableDeclaration* sourceDataPointerVariableDeclaration = buildDataPointer("sourceDataPointer",sourceVariableSymbol,outerScope);

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
  // SgForStatement* loopNest = buildLoopNest(stencilFSM->stencilDimension(),innerLoopBody,boxVariableSymbol,indexVariableSymbol_X,indexVariableSymbol_Y,indexVariableSymbol_Z,arraySizeVariableSymbol_X,arraySizeVariableSymbol_Y);
     SgForStatement* loopNest = buildLoopNest(stencilDimension,innerLoopBody,boxVariableSymbol,indexVariableSymbol_X,indexVariableSymbol_Y,indexVariableSymbol_Z,arraySizeVariableSymbol_X,arraySizeVariableSymbol_Y);
     ROSE_ASSERT(innerLoopBody != NULL);

     ROSE_ASSERT(lastStatement != NULL);
     SageInterface::insertStatementAfter(lastStatement,loopNest,autoMovePreprocessingInfo);

  // Mark this as compiler generated so that it will not be unparsed.
     associatedStatement->get_file_info()->setCompilerGenerated();

     ROSE_ASSERT(sourceVariableSymbol != NULL);
     ROSE_ASSERT(destinationVariableSymbol != NULL);

     ROSE_ASSERT(indexVariableSymbol_X != NULL);
     ROSE_ASSERT(indexVariableSymbol_Y != NULL);
  // ROSE_ASSERT(indexVariableSymbol_Z != NULL);

     ROSE_ASSERT(arraySizeVariableSymbol_X != NULL);
  // ROSE_ASSERT(arraySizeVariableSymbol_Y != NULL);

     std::vector<std::pair<StencilOffsetFSM,double> > stencilPointList = buildStencilPointList(X,sourceVariableSymbol,indexVariableSymbol_X,indexVariableSymbol_Y,indexVariableSymbol_Z,arraySizeVariableSymbol_X,arraySizeVariableSymbol_Y);

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

   }

