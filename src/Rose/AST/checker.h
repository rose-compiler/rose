
#ifndef __ROSE_AST_CHECKER_H__
#define __ROSE_AST_CHECKER_H__

namespace Rose {
namespace AST {

namespace Defect {
  enum class Kind {
    any,                     //< base kind for any defect
    integrity_edges,         //< Edges integrity: for any node in the memory pool, check that all edges point to valid nodes
    integrity_declarations,  //< Declarations integrity:
    integrity_symbols,       //< Symbols integrity:
    integrity_types,         //< Types integrity:
  };

  template <Kind kind> struct defect_t;

  // Defect for the default kind "any" is the base class for any defect
  template <>
  struct defect_t<Kind::any> {
    Kind kind;

    template <Kind k, typename... Args>
    static defect_t<k> * create(Args... args) {
      return new defect_t<k>{k, args...};
    }
  };

  template <>
  struct defect_t<Kind::integrity_edge> : defect_t<Kind::any> {
    SgNode * source;
    SgNode * target;

    std::string label;
    bool traversed;
    bool container;

    VariantT expected;
    VariantT found;

    enum class Kind {
      invalid_node,      //< Case when a node points to an address that is not in the memory pool (most probalby uninitialized pointer)
      incompatible_node, //< Case when a node points to a node of incompatible type (a declaration statement used in place of an expression -- means that there was some incorrect cast somewhere)
      unallocated_node,  //< Case when a node points to a deleted node
    } kind;
  };

  template <> struct defect_t<Kind::integrity_edge_to_invalid_node>      : defect_t<Kind::integrity_edge> {};
  template <> struct defect_t<Kind::integrity_edge_to_incompatible_node> : defect_t<Kind::integrity_edge> {};
  template <> struct defect_t<Kind::integrity_edge_to_unallocated_node>  : defect_t<Kind::integrity_edge> {};
}

using Defect = Defect::defect_t<Defect::Kind::any>;

// res.pushback(Defect::create<Kind::integrity_edge_to_invalid_node>(source, target, label, traversed, container, expected, found));

bool integrity(SgProject *, std::vector<Defect *> &);
bool consistency(SgProject *, std::vector<Defect *> &);

}
}

#endif /* __ROSE_AST_CHECKER_H__ */

