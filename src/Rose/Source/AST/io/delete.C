#include "sage3basic.h"
#include "AST_FILE_IO.h"

#define DEBUG_DeleteDisconnectedNode 0

#define DEBUG_ordered_delete 0
#if DEBUG_ordered_delete
#  include "wholeAST_API.h"
#  include "memory-pool-snapshot.h"
#endif

// EDG as persistent caches that need to be cleared
namespace EDG_ROSE_Translation { void clear_global_caches(); }

namespace Rose { namespace Source { namespace AST { namespace IO {

// Prevent double deletes by traversing the various subtrees in a top down fashion
static void ordered_delete(std::vector<SgNode*> const & deletes) {
  std::map< SgNode *, std::vector<SgNode *> > descendants; // direct descendants
  for (std::vector<SgNode *>::const_iterator i = deletes.begin(); i != deletes.end(); ++i) {
    descendants.insert(std::pair< SgNode *, std::vector<SgNode *> >(*i, std::vector<SgNode *>()));
  }

  // Find all of the nodes that don't have a parent, these are the root nodes
  std::vector<SgNode *> roots;
  for (std::vector<SgNode *>::const_iterator i = deletes.begin(); i != deletes.end(); ++i) {
    SgNode * ni = *i;
    SgNode * pi = ni->get_parent();

    // If there is an entry for this node's parent then it is NOT a root (because previous loop init.)
    std::map< SgNode *, std::vector<SgNode *> >::iterator d = descendants.find(pi);
    if (d != descendants.end()) {
      d->second.push_back(ni);
    } else {
      roots.push_back(ni);
    }
  }

#if DEBUG_ordered_delete
  unsigned cnt = 0;
#endif

  // Delete starting from the root nodes
  while (roots.size() > 0) {
    std::vector<SgNode *> next;
    for (std::vector<SgNode *>::const_iterator i = roots.begin(); i != roots.end(); ++i) {
      SgNode * r = *i;

      // Only delete if it has not already been deleted
      if (r->get_freepointer() == AST_FileIO::IS_VALID_POINTER()) {
#if DEBUG_ordered_delete
        std::cout << "Delete[" << std::dec << cnt << "] " << std::hex << r << " " << r->class_name() << std::endl;
//      std::ostringstream oss; oss << "astmerge-delete-" << cnt;
//      generateWholeGraphOfAST(oss.str());
//      Rose::MemPool::snapshot(oss.str()+".csv");
        cnt++;
#endif
        delete r;
      }

      std::map< SgNode *, std::vector<SgNode *> >::const_iterator d = descendants.find(r);
      if (d != descendants.end()) {
        next.insert(next.end(), d->second.begin(), d->second.end());
      }
    }
    roots.clear();
    roots.insert(roots.end(), next.begin(), next.end());
  }
#if DEBUG_ordered_delete
//  std::ostringstream oss; oss << "astmerge-delete-" << cnt;
//  generateWholeGraphOfAST(oss.str());
//  Rose::MemPool::snapshot(oss.str()+".csv");
#endif
}

struct DeleteAllNode : public ROSE_VisitTraversal {
  std::vector<SgNode*> deletes;

  void visit (SgNode* node) {
    deletes.push_back(node);
  }

  void apply() {
    traverseMemoryPool();
    ordered_delete(deletes);
    SgNode::set_globalFunctionTypeTable(nullptr);
    SgNode::set_globalTypeTable(nullptr);
    EDG_ROSE_Translation::clear_global_caches();
  }
};

ROSE_DLL_API void clear() {
  DeleteAllNode dan;
  dan.apply();
  AST_FILE_IO::clearAllMemoryPools();
}

ROSE_DLL_API void free() {
  DeleteAllNode dan;
  dan.apply();
  AST_FILE_IO::deleteMemoryPools();
}

struct DeleteDisconnectedNode : public ROSE_VisitTraversal {
  std::set<SgNode*> saves;
  std::vector<SgNode*> deletes;

  void visit (SgNode* node) {
    if (saves.find(node) == saves.end()) {
      deletes.push_back(node);
    }
  }

  // Recursively traverse all data members of AST nodes that are AST nodes themselves (not restricted to actual tree structure)
  void recursive_saves( SgNode* node) {
    // Stop on sinks and loops
    if (node == NULL || !saves.insert(node).second) return;

    auto data_members = node->returnDataMemberPointers();
    for (auto dm: data_members) {
      recursive_saves(dm.first);
    }
    SgLocatedNode * lnode = isSgLocatedNode(node);
    if (lnode != nullptr) {
      AttachedPreprocessingInfoType * comments = lnode->getAttachedPreprocessingInfo();
      if (comments != NULL) {
        for (auto comment: *comments) {
#if DEBUG_DeleteDisconnectedNode
          std::cout << "Comment with FI: " << std::hex << comment->get_file_info() << std::endl;
#endif
          recursive_saves(comment->get_file_info());
        }
      }
    }
  }

  void apply(SgProject * project) {
    recursive_saves(project);
    recursive_saves(SgNode::get_globalFunctionTypeTable());
    recursive_saves(SgNode::get_globalTypeTable());
    // FIXME should we include EDG_ROSE_Translation::edg_include_file_map

    traverseMemoryPool();
    ordered_delete(deletes);
  }
};

ROSE_DLL_API void prune(SgProject * project) {
  DeleteDisconnectedNode ddn;
  ddn.apply(project);
}

} } } }

