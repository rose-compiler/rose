#ifndef ROSE_SCALAR_INDEXING_SYNTHESIZED_ATTRIBUTE_H
#define ROSE_SCALAR_INDEXING_SYNTHESIZED_ATTRIBUTE_H

// This base class header file is defined in ROSE/src
// #include "synthesizedAttributeBaseClass.h"

#include "rewrite.h"

typedef HighLevelRewrite::SynthesizedAttribute SynthesizedAttributeBaseClassType;

class ScalarIndexingStatementQuerySynthesizedAttributeType
   : public SynthesizedAttributeBaseClassType
   {
     public:
       // Work space is required to accumulate the pieces of what will become the final
       // transformation to be inserted (.e.g. operators and operands in expressions).
          string workspace;

       // Constuctors and member functions declarations
         ~ScalarIndexingStatementQuerySynthesizedAttributeType();
          ScalarIndexingStatementQuerySynthesizedAttributeType();
          ScalarIndexingStatementQuerySynthesizedAttributeType(SgNode* astNode);

          ScalarIndexingStatementQuerySynthesizedAttributeType
             ( const ScalarIndexingStatementQuerySynthesizedAttributeType & X );

       // Used to build objects in the assembly function
          ScalarIndexingStatementQuerySynthesizedAttributeType
             ( const vector<ScalarIndexingStatementQuerySynthesizedAttributeType> & synthesizedAttributeList );

          ScalarIndexingStatementQuerySynthesizedAttributeType & operator=
             ( const ScalarIndexingStatementQuerySynthesizedAttributeType & X );

       // Assignment operator for base class objects
          ScalarIndexingStatementQuerySynthesizedAttributeType & operator=
             ( const SynthesizedAttributeBaseClassType & X );

#if 1
       // The details of the aggrigation of attributes is abstracted away in to an overloaded
       // operator+= member function
          ScalarIndexingStatementQuerySynthesizedAttributeType & operator+=
             ( const ScalarIndexingStatementQuerySynthesizedAttributeType & X );
#endif

       // generate a display string of the information in the attribute
          string displayString() const;

       // access function for loopDependenceLhs
          string getWorkSpace() const;
          void setWorkSpace ( string X );
   };

// endif for ROSE_SCALAR_INDEXING_SYNTHESIZED_ATTRIBUTE_H
#endif










