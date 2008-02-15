#ifndef ROSE_ARRAY_ASSIGNMENT_STATEMENT_TRANSFORMATION_H
#define ROSE_ARRAY_ASSIGNMENT_STATEMENT_TRANSFORMATION_H

#include "arrayAssignmentStatementInheritedAttribute.h"
#include "arrayAssignmentStatementSynthesizedAttribute.h"
#include "arrayAssignmentStatementAccumulatorAttribute.h"

class ArrayAssignmentStatementTransformation
   : public HighLevelRewrite::RewriteTreeTraversal<ArrayAssignmentStatementQueryInheritedAttributeType,
                                                   ArrayAssignmentStatementQuerySynthesizedAttributeType>
   {
     public:
       // Example of support for accumulator attributes
          ArrayAssignmentStatementQueryAccumulatorType & accumulatorValue;

       // Access to the operand database (stores variable names and index names used with each variable)
       // OperandDataBaseType operandDataBase;

         ~ArrayAssignmentStatementTransformation();

       // Note that the rewrite mechanism requires access to the input 
       // command line which it gets from the SgProject object.
//        ArrayAssignmentStatementTransformation( SgProject & project );
          ArrayAssignmentStatementTransformation();

          OperandDataBaseType & getOperandDataBase();

       // If this is static then it must take the SgProject pointer as an input parameter
       // We should decide upon a policy here:
       //      1) the SgProject could be specified in the constructor and then the 
       //         transformation function would have to be non-static; OR
       //      2) The transformation function is static and the project is handed in 
       //         as a parameter (better I think, since it is simpler for the user).
          static SynthesizedAttributeBaseClassType transformation
          ( const ArrayStatementQueryInheritedAttributeType & X, SgNode* astNode );

       // Non-static version of function
//        SynthesizedAttributeBaseClassType transformation
//        ( const ArrayStatementQueryInheritedAttributeType & X, SgNode* astNode );

       // Should return type be ArrayAssignmentStatementQuerySynthesizedAttributeType?
          friend ArrayAssignmentStatementQuerySynthesizedAttributeType expressionStatementTransformation
             ( SgExprStatement* astNode,
               const ArrayAssignmentStatementQueryInheritedAttributeType   & inheritedAttribute,
               const ArrayAssignmentStatementQuerySynthesizedAttributeType & innerLoopTransformation,
               const ArrayAssignmentStatementQueryAccumulatorType          & accumulatedValue );

       // Functions required by the global tree traversal mechanism
          ArrayAssignmentStatementQueryInheritedAttributeType evaluateRewriteInheritedAttribute (
             SgNode* astNode,
             ArrayAssignmentStatementQueryInheritedAttributeType inheritedValue );

          ArrayAssignmentStatementQuerySynthesizedAttributeType evaluateRewriteSynthesizedAttribute (
             SgNode* astNode,
             ArrayAssignmentStatementQueryInheritedAttributeType inheritedValue,
             SubTreeSynthesizedAttributes attributList );

          static bool targetForTransformation( SgNode* astNode );

      private:
       // Make this inacessable within the interface (by defining it but making it private)
//        ArrayAssignmentStatementTransformation();
   };

// endif for ROSE_ARRAY_ASSIGNMENT_STATEMENT_TRANSFORMATION_H
#endif






