
#include "sage3basic.h"
#include "fixupTraversal.h"

namespace Rose {
namespace AST {

struct DeleteDisconnectedNode : public ROSE_VisitTraversal {
  std::set<SgNode*> saves;
  std::vector<SgNode*> deletes;

  void visit (SgNode* node) {
//  if (isSgStorageModifier(node)) return; // FIXME idk why?
    if (
      saves.find(node) == saves.end()                                 || // part of the `saves` nodes
      ( isSg_File_Info(node) && isSgTypeDefault(node->get_parent()) )    // or signature of a Sg_File_Info object that is used as the location in a PreprocessingInfo object
    ) {
      deletes.push_back(node);
    }
  }

  // Recursively traverse all data members of AST nodes that are AST nodes themselves (not restricted to actual tree structure)
  void recursive_saves( SgNode* node) {
    // Stop on sinks and loops
    if (node == NULL || !saves.insert(node).second) return;

    std::vector<std::pair<SgNode*, std::string> > data_members = node->returnDataMemberPointers();
    for (std::vector<std::pair<SgNode*, std::string> >::iterator i = data_members.begin(); i != data_members.end(); ++i) {
      recursive_saves(i->first);
    }
  }

  // Prevent double deletes by traversing the various subtree in a top down fashion
  void ordered_delete() {
    std::map< SgNode *, std::vector<SgNode *> > descendants; // direct descendants
    for (std::vector<SgNode *>::const_iterator i = deletes.begin(); i != deletes.end(); ++i) {
      descendants.insert(std::pair< SgNode *, std::vector<SgNode *> >(*i, std::vector<SgNode *>()));
    }

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

    // Delete starting from the roots
    while (roots.size() > 0) {
      std::vector<SgNode *> next;
      for (std::vector<SgNode *>::const_iterator i = roots.begin(); i != roots.end(); ++i) {
        SgNode * r = *i;

        // Only delete if it has not already been deleted
        if (r->get_freepointer() == AST_FileIO::IS_VALID_POINTER()) {
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
  }

  void apply(SgProject * project) {
    recursive_saves(project);
    recursive_saves(SgNode::get_globalFunctionTypeTable());
    recursive_saves(SgNode::get_globalTypeTable());

    traverseMemoryPool();

    ordered_delete();
  }
};

void deleteIslands(SgProject * project) {
  DeleteDisconnectedNode ddn;
  ddn.apply(project);
}

}
}

