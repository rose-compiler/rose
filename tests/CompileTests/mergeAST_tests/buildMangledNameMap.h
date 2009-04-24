
#if 0
// DQ (4/23/2009): These are already defined in Cxx_Grammar.h
struct eqstr_string
   {
     bool operator()(const std::string & s1, const std::string & s2) const
        {
          return strcmp(s1.c_str(), s2.c_str()) == 0;
        }
   };

struct hash_string
   {
     rose_hash::hash<char*> hasher;

     public:
          size_t operator()(const std::string & name) const
             {
               return hasher(name.c_str());
             }
   };
#endif

// This class builds a map of unique names and associated IR nodes.
// It uses the memory pool traversal so that ALL IR nodes will be visited.
class MangledNameMapTraversal : public ROSE_VisitTraversal
   {
  // Build the list of mangle name and SgNode pointer pairs
     public:
       // Would it make a different to make the first template argument a "const std::string" instead of a "std::string"?
       // typedef std::map<std::string,SgNode*> MangledNameMapType;
       // typedef hash_multimap<const std::string, SgNode*, hash_string, eqstr_string> MangledNameMapType;
          typedef rose_hash::hash_map<const std::string, SgNode*, rose_hash::hash_string, rose_hash::eqstr_string> MangledNameMapType;

       // The delete list is just a set
          typedef std::set<SgNode*> SetOfNodesType;

          int numberOfNodes;
          int numberOfNodesSharable;
          int numberOfNodesEvaluated;
          int numberOfNodesAddedToManagledNameMap;
          int numberOfNodesAlreadyInManagledNameMap;

       // Allow these containers to be built (empty) outside of this class and set by the visit function.
          MangledNameMapType & mangledNameMap;
          SetOfNodesType     & setOfNodesToDelete;

          void visit ( SgNode* node);
          void addToMap ( std::string key, SgNode* node);

          static void displayMagledNameMap ( MangledNameMapType & mangledNameMap );

          static std::set<SgNode*> buildSetFromMangleNameMap ( MangledNameMapTraversal::MangledNameMapType & m );

       // This function determines if we will share the IR node
          static bool shareableIRnode ( const SgNode* node );

          MangledNameMapTraversal ( MangledNameMapType & m, SetOfNodesType & deleteSet );

       // This avoids a warning by g++
          virtual ~MangledNameMapTraversal(){};
   };

void generateMangledNameMap (MangledNameMapTraversal::MangledNameMapType & mangledMap, MangledNameMapTraversal::SetOfNodesType & setOfIRnodesToDelete );
