
#include "sage3basic.h"
#include "AST_FILE_IO.h"

#include "Rose/AST/Checker.h"
#include "Rose/AST/Defect.h"

namespace Rose { namespace AST { namespace Defects {

defect_t<Kind::integrity_edges>::defect_t(
  SgNode * source_,
  SgNode * target_,
  std::string label_,
  bool traversed_,
  bool container_,
  VariantT expected_,
  VariantT found_,
  Reason reason_
) :
  defect_t<Kind::any>(Kind::integrity_edges),
  source(source_),
  target(target_),
  label(label_),
  traversed(traversed_),
  container(container_),
  expected(expected_),
  found(found_),
  reason(reason_)
{}

void defect_t<Kind::integrity_edges>::print(std::ostream & out) const {
  out << "EdgeIntegrityDefect,source,target,label,traversed,container,expected,found,reason" << std::endl;
}

} } }

namespace Rose { namespace AST { namespace Checker {

struct EdgeIntegrityTraversal : public ROSE_VisitTraversal {
  // TODO visitor where argument type is a template parameter
  void visit (SgNode * node) { 
    // TODO make loop body a lambda then apply to the (meta-)list of fields provided by the argument's type
    auto const & data_members = node->returnDataMemberPointers();
    for (auto const & dm: data_members) {
      if (dm.first == nullptr) continue;

      using SgExpected = SgNode;        // TODO from (meta-)list of fields descriptor
      constexpr bool traversed = false; // TODO from (meta-)list of fields descriptor
      constexpr bool container = false; // TODO from (meta-)list of fields descriptor + handling container in apply function

      VariantT found = SgNode::variantFromPool(dm.first);
      if (found == 0) {
        Defect::record<Defects::Kind::integrity_edges>(node, dm.first, dm.second, traversed, container, (VariantT)SgExpected::static_variant, found, IntegrityEdgeDefect::Reason::invalid);
        continue;
      }

      if (false) { // TODO !SgExpected::hasDescendant(found)
        Defect::record<Defects::Kind::integrity_edges>(node, dm.first, dm.second, traversed, container, (VariantT)SgExpected::static_variant, found, IntegrityEdgeDefect::Reason::incompatible);
      }

      SgNode * freepointer = dm.first->get_freepointer();
      if (freepointer != AST_FileIO::IS_VALID_POINTER()) {
        Defect::record<Defects::Kind::integrity_edges>(node, dm.first, dm.second, traversed, container, (VariantT)SgExpected::static_variant, found, IntegrityEdgeDefect::Reason::unallocated);
      }
    }
  }
};

bool integrity_edges(SgProject*) {
  auto defect_cnt = Defect::all.size();
  EdgeIntegrityTraversal eit;
  eit.traverseMemoryPool();
  return defect_cnt == Defect::all.size();
}

} } }

#if 0
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
#endif

