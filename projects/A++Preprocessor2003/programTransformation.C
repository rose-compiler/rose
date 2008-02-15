// Treat config.h separately from other include files
#ifdef HAVE_CONFIG_H
// This avoids requiring the user to use config.h and follows
// the automake manual request that we use <> instead of ""
#include <config.h>
#endif

// We need this header file so that we can use the mechanisms within ROSE to build a preprocessor
#include "arrayPreprocessor.h"

#if 0
// **************************************************
// Function definitions for the Synthesized attribute
// **************************************************

ProgramTransformationSynthesizedAttributeType &
ProgramTransformationSynthesizedAttributeType::operator= 
   ( const SynthesizedAttributeBaseClassType & X )
   {
     SynthesizedAttributeBaseClassType::operator= ( X );
     return *this;
   }

ProgramTransformationSynthesizedAttributeType & 
ProgramTransformationSynthesizedAttributeType::operator=
   ( const ProgramTransformationSynthesizedAttributeType & X )
   {
  // Assignment of the derived class data (using the base class operator=)
     SynthesizedAttributeBaseClassType::operator= ( X );
     return *this;
   }

ProgramTransformationSynthesizedAttributeType & 
ProgramTransformationSynthesizedAttributeType::operator+=
   ( const ProgramTransformationSynthesizedAttributeType & X )
   {
  // Function used accumulate variable declarations
     SynthesizedAttributeBaseClassType::operator+= ( X );
     return *this;
   }
#endif

// ************************************************
// Function definitions for ArrayStatementTraversal
// ************************************************

ArrayStatementTraversal::ArrayStatementTraversal ()
   {
  // Nothing to do here
   }

ProgramTransformationInheritedAttributeType
ArrayStatementTraversal::evaluateRewriteInheritedAttribute (
   SgNode* astNode,
   ProgramTransformationInheritedAttributeType inheritedAttribute )
   {
  // Call the copy constructor to build the new inherited attribute that knows about the 
  // current location (astNode) or call a member function to update the current copy of 
  // the inherited attribute if making a copy is too expensive.
     ProgramTransformationInheritedAttributeType locatedInheritedAttribute(inheritedAttribute,astNode);
  // locatedInheritedAttribute.display("In ArrayStatementTraversal::evaluateRewriteInheritedAttribute");

     return locatedInheritedAttribute;
   }

// Functions required by the global tree traversal mechanism
ProgramTransformationSynthesizedAttributeType
ArrayStatementTraversal::evaluateRewriteSynthesizedAttribute (
   SgNode* astNode,
   ProgramTransformationInheritedAttributeType inheritedAttribute,
   SubTreeSynthesizedAttributes synthesizedAttributeList)
   {
  // Build the synthesizied attribute that this function will return
     ProgramTransformationSynthesizedAttributeType returnSynthesizedAttribute(astNode);

#if 0
     printf ("$$$$$ TOP of ArrayStatementTraversal::evaluateRewriteSynthesizedAttribute (astNode = %s) (synthesizedAttributeList.size() = %d) \n",
          astNode->sage_class_name(),synthesizedAttributeList.size());
  // inheritedAttribute.display("In ArrayStatementTraversal::evaluateRewriteInheritedAttribute");
#endif

     if (astNode->variantT() == V_SgExprStatement)
        {
       // The program logic can't reset the returnSynthesizedAttribute for each transformation
#if 0
          printf ("ERROR: In ArrayStatementTraversal::evaluateRewriteSynthesizedAttribute(): The program logic can't reset the returnSynthesizedAttribute for each transformation... \n");
          ROSE_ABORT();
#endif

       // printf ("Found an expression statement (programTransformation.C) \n");

       // We use the operator+=() because more than one transformation function could be called in general.
       // Below we will call the scalar indexing transformation as a second transformation.  Both, and more,
       // will be called for Expression statements in the future (e.g. indirect addressing).
       // returnSynthesizedAttribute += arrayAssignmentTransformation(inheritedAttribute,astNode);
#if 0
       // printf ("Calling arrayAssignmentTransformation() \n");
          returnSynthesizedAttribute = arrayAssignmentTransformation(inheritedAttribute,astNode);
       // arrayAssignmentTransformation(inheritedAttribute,astNode);
       // printf ("DONE: arrayAssignmentTransformation() \n");
#endif

#if 1
       // Turn off the scalar indexing transformation for now
       // returnSynthesizedAttribute += ScalarIndexingArrayStatementTransformation::transformation(project,astNode);
       // returnSynthesizedAttribute += ScalarIndexingArrayStatementTransformation::transformation(inheritedAttribute,astNode);
          returnSynthesizedAttribute = arrayScalarIndexingTransformation(inheritedAttribute,astNode);
#endif
        }

#if 0
     printf ("$$$$$ BOTTOM of attributeAssemblyFunction (astNode = %s) (declaration list size = %d) (returnSynthesizedAttribute.getTransformationSourceCode() = \n%s) \n",
          astNode->sage_class_name(),
          returnSynthesizedAttribute.variableDeclarationList.size(),
          returnSynthesizedAttribute.getSourceCodeString().c_str());
#endif

     return returnSynthesizedAttribute;
   }


ProgramTransformationSynthesizedAttributeType
ArrayStatementTraversal::arrayAssignmentTransformation (
   const ProgramTransformationInheritedAttributeType & inheritedAttribute,
   SgNode* astNode )
   {
  // The purpose of this transformation is to insert the array transformation in place of each array
  // statement.  This is a function defined in the class derived from the TransformationSpecificationType.
  // This function is a pure virtual function within the TransformationSpecificationType base class.

  // This function requires that the inherited attribute be passed so that we can pass it along to 
  // the nested transformation.

     ROSE_ASSERT (astNode != NULL);
     ROSE_ASSERT (isSgExprStatement(astNode) != NULL);

  // This function is at the top level using the SgProject as input (to make the source position mechanism work)
  // ROSE_ASSERT (isSgProject(astNode) != NULL);

     ProgramTransformationSynthesizedAttributeType gatheredInfo(astNode);
     if (ArrayAssignmentStatementTransformation::targetForTransformation(astNode) == true)
        {
       // printf ("Calling ArrayAssignmentStatementTransformation::transformation() \n");
          gatheredInfo = ArrayAssignmentStatementTransformation::transformation ( inheritedAttribute, astNode );
       // printf ("At base of case 'ArrayAssignmentStatementTransformation::targetForTransformation(astNode) == true' \n");

       // gatheredInfo.display("Called from ArrayStatementTraversal::arrayAssignmentTransformation() after ArrayAssignmentStatementTransformation::transformation()");

       // Since the transformation shouldhave generated global variable we can make sure that some strings are present
          ROSE_ASSERT (gatheredInfo.isEmpty() == false);

#if 0
          printf ("Exiting as part of testing ... (at base of ArrayStatementTraversal::arrayAssignmentTransformation) \n");
          ROSE_ABORT();
#endif
        }

  // gatheredInfo.display("Called from ArrayStatementTraversal::arrayAssignmentTransformation()");

#if 0
     printf ("Exiting as part of testing ... (at base of ArrayStatementTraversal::arrayTransformation) \n");
     ROSE_ABORT();
#endif

  // This copies and returns the SynthesizedAttributeBaseClassType information
     return gatheredInfo;
   }


ProgramTransformationSynthesizedAttributeType
ArrayStatementTraversal::arrayScalarIndexingTransformation (
   const ProgramTransformationInheritedAttributeType & inheritedAttribute,
   SgNode* astNode )
   {
     ROSE_ASSERT (astNode != NULL);
     ROSE_ASSERT (isSgExprStatement(astNode) != NULL);

     ProgramTransformationSynthesizedAttributeType gatheredInfo(astNode);
     if (ScalarIndexingStatementTransformation::targetForTransformation(astNode) == true)
        {
       // printf ("Calling ScalarIndexingArrayStatementTransformation::transformation() \n");
          gatheredInfo = ScalarIndexingStatementTransformation::transformation(inheritedAttribute,astNode);
       // printf ("At base of case 'ScalarIndexingArrayStatementTransformation::targetForTransformation(astNode) == true' \n");

       // gatheredInfo.display("Called from ArrayStatementTraversal::arrayAssignmentTransformation() after ArrayAssignmentStatementTransformation::transformation()");

       // Since the transformation shouldhave generated global variable we can make sure that some strings are present
          ROSE_ASSERT (gatheredInfo.isEmpty() == false);

#if 0
          printf ("Exiting as part of testing ... (at base of ArrayStatementTraversal::arrayScalarIndexingTransformation) \n");
          ROSE_ABORT();
#endif
        }

  // gatheredInfo.display("Called from ArrayStatementTraversal::arrayScalarIndexingTransformation()");

#if 0
     printf ("Exiting as part of testing ... (at base of ArrayStatementTraversal::arrayScalarIndexingTransformation) \n");
     ROSE_ABORT();
#endif

  // This copies and returns the SynthesizedAttributeBaseClassType information
     return gatheredInfo;
   }












