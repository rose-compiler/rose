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

class CollectTemplateInstantiationsMarkedForOutput
: public SgSimpleProcessing
   {
     public:
       // Data structure to accumulate list of defining template instatiations marked to be output in source code.
          std::set<SgDeclarationStatement*> definingTemplateInstantiationSet;

      //! Required traversal function
          void visit ( SgNode* node );
   };



class AddPrototypesForTemplateInstantiations
   : public SgTopDownProcessing<AddPrototypesForTemplateInstantiationsInheritedAttribute>
   {
     public:
          std::set<SgDeclarationStatement*> & definingTemplateInstantiationSet;
          std::set<SgFunctionRefExp*>         usedTemplateInstantiationSet;
          std::set<SgDeclarationStatement*>   prototypeTemplateInstantiationSet;

          AddPrototypesForTemplateInstantiations(std::set<SgDeclarationStatement*> & definingTemplateInstantiationSet);

      //! Required traversal function
          AddPrototypesForTemplateInstantiationsInheritedAttribute
               evaluateInheritedAttribute ( SgNode* node, AddPrototypesForTemplateInstantiationsInheritedAttribute inheritedAttribute );
   };

// endif for ADD_PROTOTYPES_FOR_TEMPLATE_INSTANTIATIONS_H
#endif
