
// typedef bool InheritedAttribute;
// typedef bool SynthesizedAttribute;

class TreeDiffInheritedAttribute
   {
     public:
          int treeDepth;
   };

class TreeDiffSynthesizedAttribute
   {
     public:
   };

class TreeDiffTraversal : public SgTopDownBottomUpProcessing<TreeDiffInheritedAttribute,TreeDiffSynthesizedAttribute>
   {
     public:
          vector< SgNode* > traversalTrace;

       // Functions required
          TreeDiffInheritedAttribute evaluateInheritedAttribute (
             SgNode* astNode, 
             TreeDiffInheritedAttribute inheritedAttribute );

          TreeDiffSynthesizedAttribute evaluateSynthesizedAttribute (
             SgNode* astNode,
             TreeDiffInheritedAttribute inheritedAttribute,
             SubTreeSynthesizedAttributes synthesizedAttributeList );
   };

void inorderTraversal ( SgFile* file, vector< SgNode* > & traversalTrace );
