
// Example ROSE Translator reads input program and implements a DSL embedded within C++
// to support the stencil computations, and required runtime support is developed seperately.
#include "rose.h"

#include "stencilAndStencilOperatorDetection.h"
#include "stencilEvaluation.h"

#include "shiftCalculusCompiler.h"

#include "stencilFiniteStateMachine.h"

#include "dslSupport.h"

#include "lowLevelCodeGeneration.h"

using namespace std;

using namespace DSL_Support;

//We don't generate cuda code by default
bool b_gen_cuda = false;
// disable collapse by default
bool b_enable_collapse = false;
// an internal variable to store the generated serial loop nests.
static SgForStatement* temp_for_loop_nest = NULL; 

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
#if 0
          printf ("processing functionCallExp = %p \n",functionCallExp);
#endif
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

       // RectMDArray (source)
          SgExpression* sourceArrayReferenceExpression = argumentList->get_expressions()[1];
          SgVarRefExp* sourceArrayVarRefExp = isSgVarRefExp(sourceArrayReferenceExpression);
          ROSE_ASSERT(sourceArrayVarRefExp != NULL);

       // RectMDArray (destination)
          SgExpression* destinationArrayReferenceExpression = argumentList->get_expressions()[2];
          SgVarRefExp* destinationArrayVarRefExp = isSgVarRefExp(destinationArrayReferenceExpression);
          ROSE_ASSERT(destinationArrayVarRefExp != NULL);

       // Box
          SgExpression* boxReferenceExpression = argumentList->get_expressions()[3];
          SgVarRefExp* boxVarRefExp = isSgVarRefExp(boxReferenceExpression);
          ROSE_ASSERT(boxVarRefExp != NULL);
#if 0
          printf ("DONE: processing inputs to stencil operator \n");
#endif
          ROSE_ASSERT(stencilVarRefExp->get_symbol() != NULL);
          SgInitializedName* stencilInitializedName = stencilVarRefExp->get_symbol()->get_declaration();
          ROSE_ASSERT(stencilInitializedName != NULL);

          string stencilName = stencilInitializedName->get_name();
#if 0
          printf ("stencilName = %s \n",stencilName.c_str());
#endif
          std::map<std::string,StencilFSM*> & stencilMap = traversal.get_stencilMap();
          
          ROSE_ASSERT(stencilMap.find(stencilName) != stencilMap.end());

          StencilFSM* stencilFSM = stencilMap[stencilName];
          ROSE_ASSERT(stencilFSM != NULL);

       // DQ (2/8/2015): Moved out of loop.
          int stencilDimension = stencilFSM->stencilDimension();
          ROSE_ASSERT(stencilDimension > 0);
#if 0
       // These are computed values.
          printf ("Stencil dimension = %d \n",stencilDimension);
          printf ("Stencil width     = %d \n",stencilFSM->stencilWidth());
#endif
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
               SgVariableDeclaration* sourceDataPointerVariableDeclaration = buildDataPointer("sourceDataPointer",sourceVariableSymbol,outerScope);

               SageInterface::insertStatementAfter(associatedStatement,sourceDataPointerVariableDeclaration,autoMovePreprocessingInfo);

               SgVariableDeclaration* destinationDataPointerVariableDeclaration = buildDataPointer("destinationDataPointer",destinationVariableSymbol,outerScope);
               SageInterface::insertStatementAfter(sourceDataPointerVariableDeclaration,destinationDataPointerVariableDeclaration,autoMovePreprocessingInfo);

            // Reset the variable symbols we will use in the buildStencilPoint() function.
               sourceVariableSymbol      = SageInterface::getFirstVarSym(sourceDataPointerVariableDeclaration);
               destinationVariableSymbol = SageInterface::getFirstVarSym(destinationDataPointerVariableDeclaration);

               lastStatement = destinationDataPointerVariableDeclaration;
             }

          SgBasicBlock* innerLoopBody = NULL;

          SgForStatement* loopNest = buildLoopNest(stencilFSM->stencilDimension(),innerLoopBody,boxVariableSymbol,indexVariableSymbol_X,indexVariableSymbol_Y,indexVariableSymbol_Z,arraySizeVariableSymbol_X,arraySizeVariableSymbol_Y, lastStatement);
          ROSE_ASSERT(innerLoopBody != NULL);

          ROSE_ASSERT(lastStatement != NULL);
          SageInterface::insertStatementAfter(lastStatement,loopNest,autoMovePreprocessingInfo);

       // Mark this as compiler generated so that it will not be unparsed.
          associatedStatement->get_file_info()->setCompilerGenerated();

       // Form an array of AST subtrees to represent the different points in the stencil.
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
            if (stencilFSM->stencilDimension()==2 || stencilFSM->stencilDimension()==3)
            {
              // currently arraySize_X is generated inside the loop nest. No need to specify its sharing attribute.
              //string parallel_pragma_string= "omp parallel for shared (destinationDataPointer, sourceDataPointer, arraySize_X)"
              string parallel_pragma_string;
              if(b_enable_collapse)
                parallel_pragma_string= "omp parallel for collapse(" + std::to_string(stencilFSM->stencilDimension()) + ") shared (destinationDataPointer, sourceDataPointer)";
              else
                parallel_pragma_string= "omp parallel for shared (destinationDataPointer, sourceDataPointer)";
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


