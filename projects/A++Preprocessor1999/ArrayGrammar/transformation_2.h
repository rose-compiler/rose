#ifndef TRANSFORMATION_2_H 
#define TRANSFORMATION_2_H 

// This class represents the most simple transformation for an array statement
// Function in this class define the target, transformationTemplate, and the support
// functionality required to recognise the target and build the
// equivalent transform.  

#if 0
// This transformation exists independent of all other transformations
// include the header file for the transformationBase 
// (a common base class for all transformations)
#include <rose.h>
#include "TransformationBase.h"

#include "grammar.h"
#endif

#if 0
#include "array_class_interface.h"
#include "../TransformBaseClass/TransformationBase.h"
#include "ROSE_Transformations.h"
#endif

#define OUTPUT_SHOWFILE_TO_FILE TRUE

// This transformation acomplishes the same as the previous Transform_1, but does so in a more general
// way using the definition of a grammar to drive the transformation. We outline the steps here
// to make clear what we are trying to do:
// 1) A predefined grammar is used and we implement the non-terminals of that grammar as objects
//    within the code that defines this transformation (Transform_2).
// 2) We parse a given statement using the grammar defined for this transofrmation. If the statement
//    can be parsed then we can do the transformation assocated with this pass (the one for which 
//    our grammar is defined). If we can't parse the given statement using our grammar then we
//    can't expect to do anything with that statement within this pass.
// 3) So now we may assume we have an expression tree defined using our specific grammar for this
//    transformation.  Now we can do the transormation of the nonterminals defined within our grammar
//    effectively acomplishing a translation from our grammar to the C++ (mostly C) grammar we use 
//    to define the final transformation.
// 4) Now we insert the new statement to replace the original statement, and we are done.


class ArrayAssignmentUsingGrammar : public ROSE_TransformationBase
   {
  // This class now sees the entire program tree so this design is
  // capable of more than just single statement optimizations.

     public:
         ~ArrayAssignmentUsingGrammar();
          ArrayAssignmentUsingGrammar ( SgFile *file );

       // This function is a Virtual function in the base class (ROSE_TransformationBase)
          ROSE_StatementBlock* pass( ROSE_StatementBlock* roseProgramTree );

       // Return the ROSE_StatementBlock so that we can have a handle 
       // on the program tree using the new intermediate form
          ROSE_StatementBlock* traverseProgramTree ();

      // This is where the transformation is done
          void transform( ROSE_StatementBlock* roseProgramTree );

          ROSE_Statement*  parse ( SgStatement*  stmt );

       // What should be the return type???
          ROSE_Expression* parse ( SgExpression* expr );

       // These should return more specific types within the grammar that is defined
          ROSE_Statement*      parse ( SgFunctionDeclaration *functionDeclaration );
          ROSE_Statement*      parse ( SgMemberFunctionDeclaration *memberFunctionDeclaration );
          ROSE_Statement*      parse ( SgForStatement *forStatement );
          ROSE_StatementBlock* parse ( SgBasicBlock *basicBlock );

     private:
	 ArrayAssignmentUsingGrammar();
         ArrayAssignmentUsingGrammar ( const ArrayAssignmentUsingGrammar & X );
         ArrayAssignmentUsingGrammar & operator= ( const ArrayAssignmentUsingGrammar & X );
   };

#endif // ifndef TRANSFORMATION_2_H






