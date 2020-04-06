
#include "sage3basic.h"

#include "merge_support.h"

using namespace std;

namespace Rose {
namespace AST {

ROSE_DLL_API std::set<SgNode*> getFrontendSpecificNodes() {
  struct FrontendSpecificTraversal : public ROSE_VisitTraversal {
    std::set<SgNode*> specific;
    std::set<SgNode*> non_specific;
 
    static void recursive_collect( SgNode* node , std::set<SgNode *> & collection ) {
      // Stop on sinks and loops
      if (node == NULL || !collection.insert(node).second) return;

      std::vector<std::pair<SgNode*, std::string> > data_members = node->returnDataMemberPointers();
      for (std::vector<std::pair<SgNode*, std::string> >::iterator i = data_members.begin(); i != data_members.end(); ++i) {
        recursive_collect(i->first, collection);
      }
    }

    void visit (SgNode* n) {
      Sg_File_Info * fileInfo = n->get_file_info();

      if (fileInfo != NULL) {
        if (fileInfo->isFrontendSpecific()) {
          specific.insert(n);
          recursive_collect(n, specific);
        } else {
          non_specific.insert(n);
          recursive_collect(n, non_specific);
        }
      } else {
        fileInfo = isSg_File_Info(n);
        if (fileInfo != NULL) {
          if (fileInfo->isFrontendSpecific()) {
            specific.insert(n);
          } else {
            non_specific.insert(n);
          }
        }
      }
    }

    std::set<SgNode*> apply() {
      traverseMemoryPool();

      std::set<SgNode*> result;

      std::set_difference(
        specific.begin(), specific.end(),
        non_specific.begin(), non_specific.end(),
        std::insert_iterator<set<SgNode*> >(result, result.begin())
      );

      return result;
    }
  };

  FrontendSpecificTraversal fst;
  return fst.apply();
}

}
}

