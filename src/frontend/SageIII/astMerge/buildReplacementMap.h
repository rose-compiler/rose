struct hash_nodeptr
   {
     rose_hash::hash<char*> hasher;

     public:
          size_t operator()(SgNode* node) const
             {
               return (size_t) node;
             }
   };


class ReplacementMapTraversal : public ROSE_VisitTraversal
   {
     public:
       // Keep a count of the number of IR nodes visited
          int numberOfNodes;

       // Keep a count of the number of IR nodes tests (shared)
          int numberOfNodesTested;

       // Keep a count of the number of IR nodes added to the replacement map (requiring fixup later)
          int numberOfNodesMatching;


       // DQ (2/11/2007): Note that we don't require a multimap and that a map would be faster and simpler.  Fix this later!
       // The key (first SgNode*) is the node in the new AST while the second SgNode* is 
       // the node in the original AST. Later the first SgNode* will be looked for in the 
       // second AST and replaced by the second SgNode*.

       // DQ (2/19/2007): Move to a hash map (the move from multi-map to map didn't make much difference (nothing noticable).
       // typedef std::multimap<SgNode*,SgNode*>          ReplacementMapType;
       // typedef std::map<SgNode*,SgNode*>               ReplacementMapType;
          typedef rose_hash::hash_map<SgNode*, SgNode*, hash_nodeptr> ReplacementMapType;

       // Simple STL container types for internal use
          typedef std::list< std::pair<SgNode*,SgNode*> > ODR_ViolationType;
          typedef std::set<SgNode*>                       ListToDeleteType;

       // Map of mangled names to IR nodes in the Original AST 
       // (this can't be const, because operator[] is not defined for a const object in std::map)
          MangledNameMapTraversal::MangledNameMapType & mangledNameMap;
       // MangledNameMapTraversal::SetOfNodesType     & setOfIRnodes;

       // Map of IR node values to be replaced with the new value (first (in pair) is replaced with second (in pair))
       // ReplacementMapType replacementMap;
          ReplacementMapType & replacementMap;

       // Accumulate set of pointers to nodes to delete later
          ListToDeleteType & deleteList;

       // Record all One-time Definition Rule (ODR) violations
          ODR_ViolationType odrViolations;

       // DQ (2/19/2007): Modified to permit replacement map to be built externally and updated
       // ReplacementMapTraversal( MangledNameMapTraversal::MangledNameMapType & inputMangledNameMap, ListToDeleteType & inputDeleteList );
          ReplacementMapTraversal( MangledNameMapTraversal::MangledNameMapType & inputMangledNameMap, ReplacementMapType & replacementMap, ListToDeleteType & inputDeleteList );

          void visit ( SgNode* node);

       // Check to make sure that they are the same under ODR
          bool verifyODR(SgNode* node, SgNode* duplicateNodeFromOriginalAST);

       // Uses a unique key to get the original node (in the original AST) 
       // from the generated name using the mangledNameMap.
          SgNode* getOriginalNode (const std::string & key) const;

       // Convert the list< pair<SgNode*,SgNode*> > into a list<SgNode*> (note that these specify subtrees)
          static std::set<SgNode*> buildListOfODRviolations ( ReplacementMapTraversal::ODR_ViolationType & ODR_Violations );

          static void displayReplacementMap ( const ReplacementMapTraversal::ReplacementMapType & m );

       // static void displayDeleteList ( ListToDeleteType & deleteList );

       // This avoids a warning by g++
          virtual ~ReplacementMapTraversal(){};
   };

// ReplacementMapTraversal::ReplacementMapType
void
replacementMapTraversal (
   MangledNameMapTraversal::MangledNameMapType & mangledNameMap,
   ReplacementMapTraversal::ReplacementMapType & replacementMap,
   ReplacementMapTraversal::ODR_ViolationType  & violations,
   ReplacementMapTraversal::ListToDeleteType   & deleteList );
