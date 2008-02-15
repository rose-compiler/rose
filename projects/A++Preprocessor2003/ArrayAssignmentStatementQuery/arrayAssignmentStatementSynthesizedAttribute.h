#ifndef ROSE_ARRAY_SYNTHESIZED_ATTRIBUTE_H
#define ROSE_ARRAY_SYNTHESIZED_ATTRIBUTE_H

#include "rewrite.h"

typedef HighLevelRewrite::SynthesizedAttribute SynthesizedAttributeBaseClassType;

// This class defines a transformation which thus rewrites the AST so derive from the AST_Rewrite
// class to define the synthesized attribute.
class ArrayAssignmentStatementQuerySynthesizedAttributeType
   : public SynthesizedAttributeBaseClassType
   {
     public:
       // This triggers the handling of loop dependence transformations
          bool loopDependence;
          string loopDependenceLhs;
          string loopDependenceRhs;

       // Work space is required to accumulate the pieces of what will become the final
       // transformation to be inserted (.e.g. operators and operands in expressions).
          string workspace;

       // string used to propagate variable name up through tree
       // string variableName;
       // list<string> variableNameList;

       // Database for array operand info
       // vector<ArrayOperandDataBase> arrayOperandList;

          virtual ~ArrayAssignmentStatementQuerySynthesizedAttributeType();

       // The default constructor is required by the tree traversal mechanism
          ArrayAssignmentStatementQuerySynthesizedAttributeType();

       // We try to always use this constructor
          ArrayAssignmentStatementQuerySynthesizedAttributeType( SgNode* astNode );

          ArrayAssignmentStatementQuerySynthesizedAttributeType (
               const ArrayAssignmentStatementQuerySynthesizedAttributeType & X );

       // The more usual assignment operator (calls the base class assignment operator)
          ArrayAssignmentStatementQuerySynthesizedAttributeType & operator= (
               const ArrayAssignmentStatementQuerySynthesizedAttributeType & X );

       // Assignment of the base class data
          ArrayAssignmentStatementQuerySynthesizedAttributeType & operator= ( 
               const SynthesizedAttributeBaseClassType & X );

       // access function for loopDependence variable
          bool getLoopDependence() const;
          void setLoopDependence ( bool value );

       // access function for loopDependenceLhs
          string getWorkSpace() const;
          void setWorkSpace ( string X );

       // access function for loopDependenceLhs
          string getLoopDependenceLhs() const;
          void setLoopDependenceLhs ( string X );

       // access function for loopDependenceRhs
          string getLoopDependenceRhs() const;
          void setLoopDependenceRhs ( string X );

       // The details of the aggrigation of attributes is abstracted away in to an overloaded
       // operator+= member function
          ArrayAssignmentStatementQuerySynthesizedAttributeType & operator+=
             ( const ArrayAssignmentStatementQuerySynthesizedAttributeType & X );

       // generate a display string of the information in the attribute
          string displayString() const;

          void display( const string & label ) const;

#if 0
       // Make this virtual so that it can be derived from again!
          template <class ListOfChildSynthesizedAttributes>
          virtual void mergeChildSynthesizedAttributes(
               ListOfChildSynthesizedAttributes & synthesizedAttributeList )
#endif
   };

#if 0
// Memeber functions called by derived classes to simplify the implementation 
// of the evaluateSynthesizedAttribute() member function.
template <class ListOfChildSynthesizedAttributes>
virtual void mergeChildSynthesizedAttributes( 
     ListOfChildSynthesizedAttributes & synthesizedAttributeList )
   {
  // Sorted form to support accumulation of child attributes within the parent attribute

  // DQ: Could not get direct use of template parameter or typedef to work with g++ 2.96
  // ListOfChildSynthesizedAttributes::iterator i;
  // typedef ListOfChildSynthesizedAttributes::iterator ListIterator;
  // ListIterator i;
  // Container<SynthesizedAttribute>::iterator i;
  // for (i = synthesizedAttributeList.begin(); i != synthesizedAttributeList.end(); i++)

     int i;
     for (i = 0; i < synthesizedAttributeList.size(); i++)
        {
       // printf ("Process synthesized attribute #%d \n",counter++);
       // (*i).display("Called from <some>Traversal::evaluateSynthesizedAttribute()");
       // returnAttribute += *i;
          (*this) += synthesizedAttributeList[i];
        }
   }
#endif



// endif for ROSE_ARRAY_SYNTHESIZED_ATTRIBUTE_H
#endif










