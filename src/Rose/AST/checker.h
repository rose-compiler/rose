
#ifndef __ROSE_AST_CHECKER_H__
#define __ROSE_AST_CHECKER_H__

namespace Rose {
namespace AST {

//! Mechanism to describe defect in the AST
namespace Defects {
  enum class Kind {
    any,                     //!< base kind for any defect
    integrity_edges,         //!< Edges integrity: for any node in the memory pool, check that all edges point to valid nodes
    integrity_declarations,  //!< Declarations integrity:
    integrity_symbols,       //!< Symbols integrity:
    integrity_types,         //!< Types integrity:
  };

  template <Kind kind> struct defect_t;

  // Defect for the default kind "any" is the base class for any defect
  template <>
  struct defect_t<Kind::any> {
    Kind kind;

    //! Call new for the specific kind of defect, forwards all argument to the constructor
    template <Kind k, typename... Args>
    static defect_t<k> * create(Args... args) {
      return new defect_t<k>{k, args...};
    }

    //! Call new for the specific defect type, forwards all argument to the constructor (requires the specialization to declare __kind)
    template <typename Defect, typename... Args>
    static Defect * create(Args... args) {
      return new Defect{Defect::__kind, args...};
    }
  };

  //! Specialization of the defect_t template for the case of an edge integrity defect
  template <>
  struct defect_t<Kind::integrity_edges> : defect_t<Kind::any> {
    static constexpr Kind __kind = Kind::integrity_edges; //!< Needed to `create` defects from the typename instead of the kind.

    SgNode * source;   //!<  Pointer to the source node of the edge (always a valid pointer and node)
    SgNode * target;   //!<  Pointer to the target node of the edge. Either the pointer or the node are invalid.

    std::string label; //!<  Label of this edge in the grammar
    bool traversed;    //!<  Traversed edges forms the structure of the AST while the other one represent relations like types and symbols.
    bool container;    //!<  If the edge have multiplicity (like a node with a std::vector<SgExpression*>)

    VariantT expected; //!<  The expected variant for target (like V_SgExpression)
    VariantT found;    //!<  The actual variant of target if the pointer is valid (obtained by finding the pointer in the memory pool)

    enum class Reason {
      invalid,         //!<  Case when a node points to an address that is not in the memory pool (most probalby uninitialized pointer)
      incompatible,    //!< Case when a node points to a node of incompatible type (a declaration statement used in place of an expression -- means that there was some incorrect cast somewhere)
      unallocated,     //!< Case when a node points to a deleted node
    } reason;
  };

  // TODO binding function?
}

using Defect = Defects::defect_t<Defects::Kind::any>;
using IntegrityEdgeDefect = Defects::defect_t<Defects::Kind::integrity_edges>;

// Note for later:
//   How to ues:
//     res.push_back(Defect::create<Kind::integrity_edges>(source, target, label, traversed, container, expected, found, IntegrityEdgeDefect::Reason::incompatible));
//     res.push_back(Defect::create<IntegrityEdgeDefect>(source, target, label, traversed, container, expected, found, IntegrityEdgeDefect::Reason::incompatible));
//   DO NOT DO: (using new)
//     res.push_back(new IntegrityEdgeDefect(Defects::Kind::integrity_edges, source, target, label, traversed, container, expected, found, IntegrityEdgeDefect::Reason::incompatible));

/**
 * \brief check multiple low-level properties of the AST's graph.
 * @param project
 * @param defects is used to store pointers to defect descriptors (user must delete these descriptors)
 * @return true if no defect was found
 */
bool integrity(SgProject * project, std::vector<Defect const *> & defects);

/**
 * \brief for all valid node in the AST's graph check that the target of all edges are valid.
 * @param project
 * @param defects is used to store pointers to defect descriptors (user must delete these descriptors)
 * @return true if no defect was found
 */
bool integrity_edges(SgProject * project, std::vector<Defect const *> & defects);

/**
 * \brief check all declaration subgraphs (1st-nondef / defn)
 * @param project
 * @param defects is used to store pointers to defect descriptors (user must delete these descriptors)
 * @return true if no defect was found
 */
bool integrity_declarations(SgProject * project, std::vector<Defect const *> & defects);

/**
 * \brief check all symbol subgraphs.
 * @param project
 * @param defects is used to store pointers to defect descriptors (user must delete these descriptors)
 * @return true if no defect was found
 */
bool integrity_symbols(SgProject * project, std::vector<Defect const *> & defects);

/**
 * \brief check all type subgraphs.
 * @param project
 * @param defects is used to store pointers to defect descriptors (user must delete these descriptors)
 * @return true if no defect was found
 */
bool integrity_types(SgProject * project, std::vector<Defect const *> & defects);

/**
 * \brief check (potentially complex) rules
 *
 * @param project
 * @param defects is used to store pointers to defect descriptors
 * @return true if no defect was found
 * 
 * @{
 */
bool consistency(SgProject * project, std::vector<Defect const *> & defects);
/** @} */

}
}

#endif /* __ROSE_AST_CHECKER_H__ */

