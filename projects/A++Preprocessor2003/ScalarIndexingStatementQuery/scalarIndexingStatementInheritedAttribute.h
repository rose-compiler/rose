#ifndef ROSE_SCALAR_INDEXING_STATEMENT_QUERY_INHERITED_ATTRIBUTE_H
#define ROSE_SCALAR_INDEXING_STATEMENT_QUERY_INHERITED_ATTRIBUTE_H

class ScalarIndexingStatementQueryInheritedAttributeType : public ArrayStatementQueryInheritedAttributeType
   {
     public:
          bool skipSubstitutionOfSubscriptComputationMacro;
          bool usingIndexObjectsInSubscriptComputation;

       // Make the attribute as being indexed so that in the assembly we can generate the correct
       // transformation
          bool isIndexedArrayOperand;

       // Destructors and Constructors
         ~ScalarIndexingStatementQueryInheritedAttributeType();
          ScalarIndexingStatementQueryInheritedAttributeType();
          ScalarIndexingStatementQueryInheritedAttributeType ( SgNode* astNode );

          ScalarIndexingStatementQueryInheritedAttributeType
               ( const ArrayStatementQueryInheritedAttributeType & X, SgNode* astNode );

          ScalarIndexingStatementQueryInheritedAttributeType
               ( const ScalarIndexingStatementQueryInheritedAttributeType & X, SgNode* astNode );

       // Copy constructor and assignment operator
          ScalarIndexingStatementQueryInheritedAttributeType
               ( const ScalarIndexingStatementQueryInheritedAttributeType & X );
          ScalarIndexingStatementQueryInheritedAttributeType & operator= 
               ( const ScalarIndexingStatementQueryInheritedAttributeType & X );

       // access functions
       // list<TransformationAssertion::TransformationOption> & getTransformationOptions () const;
       // void setTransformationOptions ( const list<TransformationAssertion::TransformationOption> & X );

          list<int> & getTransformationOptions () const;
          void setTransformationOptions ( const list<int> & X );

          bool getSkipSubstitutionOfSubscriptComputationMacro () const;
          void setSkipSubstitutionOfSubscriptComputationMacro ( bool newValue );

          bool getUsingIndexObjectsInSubscriptComputation () const;
          void setUsingIndexObjectsInSubscriptComputation ( bool newValue );

       // Access functions for isIndexedArrayOperand variable
          bool getIsIndexedArrayOperand ();
          void setIsIndexedArrayOperand ( bool value );

       // output class data for debugging
          void display ( const char* label = "" ) const;
   };

// endif for ROSE_SCALAR_INDEXING_STATEMENT_QUERY_INHERITED_ATTRIBUTE_H
#endif
