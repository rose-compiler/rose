#ifndef ROSE_SCALAR_INDEXING_STATEMENT_TRANSFORMATION_H
#define ROSE_SCALAR_INDEXING_STATEMENT_TRANSFORMATION_H

// Include the base class
// #include "specification.h"
// #include "scalarDataBase.h"

#include "scalarIndexingStatementInheritedAttribute.h"
#include "scalarIndexingStatementSynthesizedAttribute.h"
#include "scalarIndexingStatementAccumulatorAttribute.h"

class ScalarIndexingStatementTransformation
// : public SgTopDownBottomUpProcessing<ScalarIndexingStatementQueryInheritedAttributeType,
//                                      ScalarIndexingStatementQuerySynthesizedAttributeType>
   : public HighLevelRewrite::RewriteTreeTraversal<ScalarIndexingStatementQueryInheritedAttributeType,
                                                   ScalarIndexingStatementQuerySynthesizedAttributeType>
   {
     public:
       // Example of support for accumulator attributes
          ScalarIndexingStatementQueryAccumulatorType & accumulatorValue;

         ~ScalarIndexingStatementTransformation();
          ScalarIndexingStatementTransformation();

          static bool targetForTransformation( SgNode* astNode );

      // Some aspects of the transformation (e.g. adding a macro to define the 
      // subscript computation and an #undef to undefine it) only occure once
      // if certain optimizations are applied (e.g. if all the sizes and strides 
      // are the same) independent of how many operands are processed. This records
      // the processing of the first operand within the traversal so that the 
      // processing of subsequent operands will not generate redundent pieces of
      // the transformation.
         bool firstOperandAlreadyProcessed;

       // Access to the operand database (stores variable names and index names used with each variable)
       // ScalarIndexingOperandDataBaseType operandDataBase;
       // ScalarIndexingOperandDataBaseType & getOperandDataBase();

       // ScalarIndexingStatementQuerySynthesizedAttributeType transformation ( SgNode* astNode );
       // static SynthesizedAttributeBaseClassType transformation ( SgNode* astNode );
          static SynthesizedAttributeBaseClassType transformation
          ( const ArrayStatementQueryInheritedAttributeType & X, SgNode* astNode );

       // Functions required by the global tree traversal mechanism
          ScalarIndexingStatementQueryInheritedAttributeType evaluateRewriteInheritedAttribute (
             SgNode* astNode,
             ScalarIndexingStatementQueryInheritedAttributeType inheritedValue );

          ScalarIndexingStatementQuerySynthesizedAttributeType evaluateRewriteSynthesizedAttribute (
             SgNode* astNode,
             ScalarIndexingStatementQueryInheritedAttributeType inheritedValue,
             SubTreeSynthesizedAttributes attributList );

   // This should not be required
      private:
//        ScalarIndexingStatementTransformation();
   };

// endif for ROSE_SCALAR_INDEXING_STATEMENT_TRANSFORMATION_H
#endif





