
class SequenceGenerationInheritedAttribute
   {
     public:
         int treeDepth;

      // We only want to process IR nodes in the current file (for now)
         SgFile* file;

         SequenceGenerationInheritedAttribute( int depth, SgFile* f ) : treeDepth(depth), file(f) {}         
   };

class SequenceGenerationSynthesizedAttribute
   {
     public:
   };

class SequenceGenerationTraversal : public SgTopDownBottomUpProcessing<SequenceGenerationInheritedAttribute,SequenceGenerationSynthesizedAttribute>
   {
     public:
          int minTreeDepth;
          int maxTreeDepth;

       // List of IR nodes visited in preorder traversal.
          vector< SgNode* > traversalTrace;

       // Mapping of IR node pointers to the order in which they were traversed.
          map< SgNode*,int > nodeOrder;

       // Mapping of IR node pointers to the depth of the IR node in the AST (root is zero depth)
          map< SgNode*,int > nodeDepth;

       // Functions required
          SequenceGenerationInheritedAttribute evaluateInheritedAttribute (
             SgNode* astNode, 
             SequenceGenerationInheritedAttribute inheritedAttribute );

          SequenceGenerationSynthesizedAttribute evaluateSynthesizedAttribute (
             SgNode* astNode,
             SequenceGenerationInheritedAttribute inheritedAttribute,
             SubTreeSynthesizedAttributes synthesizedAttributeList );

          SequenceGenerationTraversal ( int minDepth, int maxDepth ) : minTreeDepth(minDepth), maxTreeDepth(maxDepth) {}
   };

// Supporting function for generating lists of properties from the AST (for a single subtree, not a SgProject)
void generateTraversalListSupport ( SgNode* node, vector< SgNode* > & traversalTrace, map< SgNode*,int > & nodeOrder, map< SgNode*,int > & nodeDepth, int minDepth , int maxDepth );

// Function that generate lists of properties
vector< SgNode* >  generateTraversalList      ( SgNode* file, int min_depth = 0 /* infinite depth */ , int max_depth = -1 /* infinite depth */ );
map< SgNode*,int > generateTraversalOrderList ( SgNode* node, int minDepth = 0, int maxDepth = -1 );
map< SgNode*,int > generateTraversalDepthList ( SgNode* node, int minDepth = 0, int maxDepth = -1 );

// Function that generates lists of properties for a pair of subtrees (returns list of lists of IR node pointers)
vector< vector< SgNode* > > generateTraversalTraceList ( SgNode* subtree1, SgNode* subtree2, int minDepth , int maxDepth );
