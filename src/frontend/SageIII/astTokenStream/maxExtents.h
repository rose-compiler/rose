
// Support to the evaluation of source positions of subtrees to propagate source positions start and end positions to 
// compiler generated SgStatement IR nodes (that don't have a source position) that contain valid subtrees.  Where the 
// source position map to a single character position the subtree is either:
//   1) A macro expansion
//   2) A single character statement (e.g. ";", or SgValueExpression convertedt to a SgExprStatement).
// This work supports the token mapping to the AST.

namespace MaxSourceExtents {
   
class InheritedAttribute 
   {
     public:
       // int source_position_start;
       // int source_position_end;
          int start_line;
          int start_column;
          int end_line; 
          int end_column;

       // Detect when to stop processing deeper into the AST.
          bool processChildNodes;

       // Restrict source position evaluation specific to a given file.
          SgSourceFile* sourceFile;

          InheritedAttribute(SgSourceFile* input_sourceFile, int input_start_line, int input_start_column, int input_end_line, int input_end_column, bool processed);

          InheritedAttribute ( const InheritedAttribute & X );
   };

class SynthesizedAttribute
   {
     public:
          SgNode* node;

          int start_line;
          int start_column;
          int end_line; 
          int end_column;

          SynthesizedAttribute();
          SynthesizedAttribute(SgNode* n, int start_line, int start_column, int end_line, int end_column);

          SynthesizedAttribute(const SynthesizedAttribute & X);
   };


// We need this to be a SgTopDownBottomUpProcessing traversal.
// class TokenMappingTraversal : public AstTopDownProcessing<InheritedAttribute>
class SourceExtentsTraversal : public SgTopDownBottomUpProcessing<InheritedAttribute,SynthesizedAttribute>
   {
     public:
       // int source_position_start;
       // int source_position_end;
          int accumulator_attribute_start_line;
          int accumulator_attribute_start_column;
          int accumulator_attribute_end_line; 
          int accumulator_attribute_end_column;

       // virtual function must be defined
          InheritedAttribute evaluateInheritedAttribute(SgNode* n, InheritedAttribute inheritedAttribute);

       // virtual function must be defined
          SynthesizedAttribute evaluateSynthesizedAttribute (SgNode* n,InheritedAttribute inheritedAttribute,SubTreeSynthesizedAttributes childAttributes);
   };


void computeMaxSourceExtents(SgSourceFile* sourceFile, SgNode* n, int & start_line, int & start_column, int & end_line, int & end_column);

// end of namespace
}
