#ifndef ADD_PROTOTYPES_FOR_TEMPLATE_INSTANTIATIONS_H
#define ADD_PROTOTYPES_FOR_TEMPLATE_INSTANTIATIONS_H

// This post processing step is build to support the testTemplates translator which
// forces all instantations to be output and demonstrates that some template instantaitions 
// require prototypes that EDG has not already placed into the AST; and so they are not
// showing up in the ROSE AST after translation.


// DQ (5/18/2017):
/*! \brief Fixups transformations that have added template instantiations or caused template instantiations to be output so that there is a proper prototype declaration before the template instantation is called.

    All prototypes if built, should be output into the source file.

   \internal This post processing needs to be run before unparsing, and mostly after user transformations.

   \todo Needc to consider how this might effect unparsing of header files.
 */

void addPrototypesForTemplateInstantiations( SgNode* node );

//! Inherited attribute required for AddPrototypesForTemplateInstantiations class.
class AddPrototypesForTemplateInstantiationsInheritedAttribute
   {
     public:
          bool insideTransformationToOutput;

          AddPrototypesForTemplateInstantiationsInheritedAttribute() : insideTransformationToOutput(false) {}
   };

// DQ (6/21/2005): This class controls the output of template declarations in the generated code (by the unparser).
class CollectTemplateInstantiationsMarkedForOutput
: public SgSimpleProcessing
   {
     public:
      //! Required traversal function
          void visit ( SgNode* node );
   };



// DQ (6/21/2005): This class controls the output of template declarations in the generated code (by the unparser).
class AddPrototypesForTemplateInstantiations
   : public SgTopDownProcessing<AddPrototypesForTemplateInstantiationsInheritedAttribute>
   {
     public:
      //! Required traversal function
          AddPrototypesForTemplateInstantiationsInheritedAttribute
               evaluateInheritedAttribute ( SgNode* node, AddPrototypesForTemplateInstantiationsInheritedAttribute inheritedAttribute );
   };

// endif for ADD_PROTOTYPES_FOR_TEMPLATE_INSTANTIATIONS_H
#endif
