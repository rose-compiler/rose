

#include "sage3basic.h"
#include "AST_FILE_IO.h"

#define DEBUG__ROSE_AST_CONSISTENCY 0

namespace Rose {
namespace AST {

struct CheckAllMembers : public ROSE_VisitTraversal {
  std::map<SgNode*, std::map<std::string, std::tuple<SgNode*,VariantT, SgNode*>>> results;

  void visit (SgNode* node) {
    auto const & data_members = node->returnDataMemberPointers();
    auto & res = results[node];
    for (auto const & dm: data_members) {
      if (dm.first == nullptr) continue;
      VariantT variant = SgNode::variantFromPool(dm.first);
      SgNode * freepointer = variant != 0 ? dm.first->get_freepointer() : nullptr;
      if (freepointer != AST_FileIO::IS_VALID_POINTER()) {
        res[dm.second] = std::tuple<SgNode*,VariantT, SgNode*>(dm.first, variant, freepointer);
      }
    }
  }
};

std::map<SgNode*, std::map<std::string, std::tuple<SgNode*,VariantT, SgNode*>>> consistency_details() {
  CheckAllMembers cam;
  cam.traverseMemoryPool();
  return cam.results;
}

bool consistency(std::ostream & out) {
#if DEBUG__ROSE_AST_CONSISTENCY
  printf("Rose::AST::consistency\n");
#endif
  CheckAllMembers cam;
  cam.traverseMemoryPool();

  unsigned node_cnt = 0;
  unsigned invalid_cnt = 0;
  unsigned unalloc_cnt = 0;
  for (auto n: cam.results) {
    auto const node = n.first;
    auto const cls_name = node->class_name();
    if (n.second.size() == 0) continue;

    node_cnt++;
    out << cls_name << " " << std::hex << node << " has " << std::dec << n.second.size() << " dangling pointers" << std::endl;
    for (auto m: n.second) {
      auto const fld_name        = m.first;
      auto const fld_pointer     = std::get<0>(m.second);
      auto const fld_variant     = std::get<1>(m.second);
      auto const fld_freepointer = std::get<2>(m.second);
      if (fld_variant == 0) invalid_cnt++;
      else unalloc_cnt++;
      out << "    " << (fld_variant == 0 ? "out-of-pools" : "unallocated") << " " << fld_name << " " << std::hex << fld_pointer << " " << std::dec << fld_variant << " " << std::hex << fld_freepointer << std::endl;
    }
  }
  if (node_cnt > 0) {
    out << "Found " << std::dec << node_cnt << " nodes with invalid pointers." << std::endl;
    out << "  " << invalid_cnt << " pointers outside of any memory pool." << std::endl;
    out << "  " << unalloc_cnt << " not currently allocated (part of reuse chain -- deleted or never used nodes)" << std::endl;
  }

  return node_cnt == 0;
}

} }

