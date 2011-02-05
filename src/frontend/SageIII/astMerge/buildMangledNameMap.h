#ifndef ROSE_BUILD_MANGLED_NAME_MAP_H
#define ROSE_BUILD_MANGLED_NAME_MAP_H

#include <string>
//#include "sage3.h"


#if 0
// #if OLD_GRAPH_NODES
// #ifndef ROSE_USE_NEW_GRAPH_NODES
// DQ (8/19/2008): This is already defined in src/frontend/SageIII/Cxx_Grammar.h
struct eqstr_string
   {
     bool operator()(const std::string & s1, const std::string & s2) const
        {
          return std::string(s1) == s2;
        }
   };

// DQ (8/19/2008): This is already defined in src/frontend/SageIII/Cxx_Grammar.h
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
// CH (4/9/2010): Use boost::unordered instead 
//#ifdef _MSC_VER
#if 0
       // typedef rose_hash::unordered_map<std::string, SgNode*, rose_hash::hash_string> MangledNameMapType;
          typedef rose_hash::unordered_map<std::string, SgNode*> MangledNameMapType;
#else
          // CH (4/13/2010): Use boost::hash<string> instead
          //typedef rose_hash::unordered_map<std::string, SgNode*, rose_hash::hash_string, rose_hash::eqstr_string> MangledNameMapType;
          typedef rose_hash::unordered_map<std::string, SgNode*> MangledNameMapType;
#endif
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
          SetOfNodesType     setOfNodesPreviouslyVisited;

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

#endif // ROSE_BUILD_MANGLED_NAME_MAP_H
