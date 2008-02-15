#ifndef MARK_TRANSFORMATION_FOR_OUTPUT_H
#define MARK_TRANSFORMATION_FOR_OUTPUT_H

// DQ (8/19/2005):
/*! \brief Fixups transformations (which are often marked at a high level and need to be marked properly through their subtree).

    All transformations should be output into the source file.

    \internal This might have to be modified when we unparse header files!

   \todo Talk to Rich about how this might effect unparsing of header files.
 */

void markTransformationsForOutput( SgNode* node );

//! Inherited attribute required for MarkTemplateSpecializationsForOutput class.
class MarkTransformationsForOutputInheritedAttribute
   {
     public:
          bool insideTransformationToOutput;

          MarkTransformationsForOutputInheritedAttribute() : insideTransformationToOutput(false) {}
   };

// DQ (6/21/2005): This class controls the output of template declarations in the generated code (by the unparser).
class MarkTransformationsForOutput
   : public SgTopDownProcessing<MarkTransformationsForOutputInheritedAttribute>
   {
     public:
      //! Required traversal function
          MarkTransformationsForOutputInheritedAttribute
               evaluateInheritedAttribute ( SgNode* node, MarkTransformationsForOutputInheritedAttribute inheritedAttribute );
   };

// endif for MARK_TRANSFORMATION_FOR_OUTPUT_H
#endif
