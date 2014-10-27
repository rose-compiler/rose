
#include "rose.h"

#include "operatorFiniteStateMachine.h"

#include "operatorEvaluation.h"

#include "mapleDSLCompiler.h"

#include "dslCodeGeneration.h"

#if 0
// Old API
void 
generateMapleCode(OperatorEvaluationTraversal & X, bool generateLowlevelCode)
   {
     printf ("Sorry, Maple code generation for Maple DSL not implemented yet! \n");
  // ROSE_ASSERT(false);
   }
#endif

void
generateMapleCode(SgExpression* lhs, SgExpression* rhs, OperatorFSM discritizationOperator, bool generateLowlevelCode)
   {
     printf ("In generateMapleCode(): \n");
     printf ("   --- lhs = %p \n",lhs);
     printf ("   --- rhs = %p \n",rhs);
     discritizationOperator.display("In generateMapleCode(): discritizationOperator");




     printf ("Sorry, Maple code generation for Maple DSL not implemented yet! \n");
  // ROSE_ASSERT(false);
   }


