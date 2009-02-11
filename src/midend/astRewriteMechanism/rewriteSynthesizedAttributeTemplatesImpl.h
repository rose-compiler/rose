#ifndef AST_REWRITE_SYNTHESIZED_ATTRIBUTE_TEMPLATES_C
#define AST_REWRITE_SYNTHESIZED_ATTRIBUTE_TEMPLATES_C

// #include "rose.h"

#include "rewrite.h"


// Use a templated member function since we are dependent upon the STL
// list of synthesized attributes used in the dirived classes
// synthesized attribute.
template <class ListOfChildSynthesizedAttributes>
void
HighLevelRewrite::SynthesizedAttribute::mergeChildSynthesizedAttributes(
   ListOfChildSynthesizedAttributes & synthesizedAttributeList )
   {
  // Sorted form to support accumulation of child attributes within the parent attribute

  // DQ: Could not get direct use of template parameter or typedef to work with g++ 2.96
  // ListOfChildSynthesizedAttributes::iterator i;
  // typedef ListOfChildSynthesizedAttributes::iterator ListIterator;
  // ListIterator i;
  // Container<SynthesizedAttribute>::iterator i;
  // for (i = synthesizedAttributeList.begin(); i != synthesizedAttributeList.end(); i++)

     unsigned int i;
     for (i = 0; i < synthesizedAttributeList.size(); i++)
        {
       // printf ("Process synthesized attribute #%d \n",counter++);
       // (*i).display("Called from MyTraversal::evaluateSynthesizedAttribute()");
       // returnAttribute += *i;
          (*this) += synthesizedAttributeList[i];
        }
   }

// endif for AST_REWRITE_SYNTHESIZED_ATTRIBUTE_TEMPLATES_C
#endif
