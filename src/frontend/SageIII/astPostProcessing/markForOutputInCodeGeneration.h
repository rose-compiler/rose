#ifndef MARK_FOR_OUTPUT_IN_CODE_GENERATION_H
#define MARK_FOR_OUTPUT_IN_CODE_GENERATION_H

// DQ (5/22/2005):
/*! \brief Mark an AST subtree as compiler generated and to be output in the source file (code generation phase).

    \internal Nodes may be marked as "compiler generated" and "compiler generated to be unparsed" separately.
 */
void markForOutputInCodeGeneration( SgNode* node );

/*! \brief Mark an AST subtree as compiler generated and to be output in the source file (code generation phase).

    \internal Nodes may be marked as "compiler generated" and "compiler generated to be unparsed" separately.
 */
class MarkForOutputInCodeGeneration : public SgSimpleProcessing
   {
     public:
      //! Required traversal function
          void visit (SgNode* node);
   };

// endif for MARK_FOR_OUTPUT_IN_CODE_GENERATION_H
#endif
