
class FixupTraversal : public ROSE_VisitTraversal
   {
     public:
          int numberOfNodes;
          int numberOfNodesTested;
          int numberOfDataMemberPointersEvaluated;
          int numberOfValidDataMemberPointersEvaluated;
          int numberOfValidDataMemberPointersWithValidKeyEvaluated;
          int numberOfValidDataMemberPointersWithValidKeyButNotInReplacementMap;
          int numberOfValidDataMemberPointersWithValidKeyAndInReplacementMap;
          int numberOfValidDataMemberPointersWithValidKeyAndInReplacementMapEvaluated;
          int numberOfValidDataMemberPointersReset;

       // Map of IR node values to be replaced with the new value (first (in pair) is replaced with second (in pair))
          const ReplacementMapTraversal::ReplacementMapType & replacementMap;
       // MangledNameMapTraversal::SetOfNodesType     & setOfIRnodes;

       // DQ (2/8/2007): Added this back in to support skippingfixup on nodes on delete list
          typedef std::set<SgNode*> listToDeleteType;
          const listToDeleteType & deleteList;

          FixupTraversal ( const ReplacementMapTraversal::ReplacementMapType & inputReplacementMap, const listToDeleteType & inputListToDelete );

          void visit ( SgNode* node);

          void resetChildren ( SgNode* node, SgNode** pointerToKey, SgNode* key, SgNode* originalNode, const std::string & datamember );

       // This avoids a warning by g++
          virtual ~FixupTraversal(){};
   };

// DQ (2/8/2007): Add the delete list so that we can skip the fixup on nodes in the delete 
// list this will allow them to continue to reference there original subtrees and allow the 
// addAssociatedNodes() function to detect related IR nodes to be both saved and deleted 
// (via set_difference algorithm).
// void fixupTraversal(ReplacementMapTraversal::ReplacementMapType & replacementMap );
void fixupTraversal( const ReplacementMapTraversal::ReplacementMapType & replacementMap, const std::set<SgNode*> & deleteList );


// DQ (2/25/2009): Function added to support similar concept for AST outlining.
// this function fixups up references in a subtree (the outlined file when the 
// outlining is done to a separate file).
class FixupSubtreeTraversal : public SgSimpleProcessing
   {
     public:
       // Map of IR node values to be replaced with the new value (first (in pair) is replaced with second (in pair))
          const ReplacementMapTraversal::ReplacementMapType & replacementMap;

          const FixupTraversal::listToDeleteType & deleteList;

          FixupSubtreeTraversal ( const ReplacementMapTraversal::ReplacementMapType & inputReplacementMap, const FixupTraversal::listToDeleteType & inputListToDelete );

          void visit ( SgNode* node);
   };

void fixupSubtreeTraversal( SgNode* node, const ReplacementMapTraversal::ReplacementMapType & inputReplacementMap, const FixupTraversal::listToDeleteType & inputListToDelete );

