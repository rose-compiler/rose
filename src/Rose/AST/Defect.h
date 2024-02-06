#ifndef ROSE_AST_Defect_H
#define ROSE_AST_Defect_H

namespace Rose {
namespace AST {

/** Describe defect in the AST
 *
 * How to record a defect:
 *
 * @code
 *    Defect::record<Defects::Kind::my_defect_kind>(my_descriptor_arguments);
 * @endcode
 *
 * Examples:
 *
 * @code
 *     Defect::record<Defects::Kind::integrity_edges>(source, target, label, traversed, container, expected, found,
 *                                                    IntegrityEdgeDefect::Reason::incompatible);
 *     Defect::record<IntegrityEdgeDefect>(source, target, label, traversed, container, expected, found,
 *                                         IntegrityEdgeDefect::Reason::incompatible);
 * @endcode
 */
namespace Defects {

  //! List of the supported kind of defects. Kind::any 
  enum class Kind {
    any,                     //!< base kind for any defect
    integrity_edges,         //!< Edges integrity: for any node in the memory pool, check that all edges point to valid nodes
    integrity_declarations,  //!< Declarations integrity:
    integrity_symbols,       //!< Symbols integrity:
    integrity_types,         //!< Types integrity:
  };

  /** The generic defect descriptor.
   *
   *  @attention The name of this class violates the ROSE naming convention (not PascalCase). Users should not use this class until
   *  this is fixed since the change will be API breaking. */
  template <Kind kind> struct defect_t;

  /** Defect descriptor specialization for the default kind "any".
   *
   *  @attention The name of this class violates the ROSE naming convention (not PascalCase). Users should not use this class until
   *  this is fixed since the change will be API breaking.
   *
   * All other defect descriptors must inherit from this descritor.
   * It provides a static method `record` which should always be used to create a defect descriptor.
   * This method collects all descriptors and return const references.
   * Hence, all defect descriptors can be deleted by a call to clear.
   */
  template <>
  struct defect_t<Kind::any> {
    using self_t = defect_t<Kind::any>;

    //! Set of all defects
    static std::set<self_t *> all;

    //! Call new for the specific kind of defect, forwards all argument to the constructor. Add pointer to all
    template <Kind k, typename... Args>
    static defect_t<k> const & record(Args... args) {
      defect_t<k> * defect = new defect_t<k>(args...);
      all.insert(defect);
      return *defect;
    }

    //! Call new for the specific defect type, forwards all argument to the constructor (requires the specialization to declare __kind)
    template <typename DefectT, typename... Args>
    static DefectT const & record(Args... args) {
      return record<DefectT::__kind>(args...);
    }

    Kind kind;

    defect_t<Kind::any>(Kind kind_);
    virtual ~defect_t<Kind::any>();

    static void clear(); //!< Delete all stored defect and clear the container
    static void display(std::ostream & out); //<! Calls print on all stored defects

    virtual void print(std::ostream & out) const = 0;
  };
}

/**
 * \brief Main interface for Rose::AST::Defects.
 *
 * Use Defect::record<Kind>(args...) to record a defect.
 *
 * Use Defect::display(std::ostream&) to display all defects.
 *
 * Iterate over Defect::all.
 *
 * TODO[Tristan]: const iterator and filter.
 */ 
using Defect = Defects::defect_t<Defects::Kind::any>;

namespace Defects {

  /** Specialization of the defect_t template for the case of an edge integrity defect.
   *
   *  @attention The name of this class violates the ROSE naming convention (not PascalCase). Users should not use this class until
   *  this is fixed since the change will be API breaking. */
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
      incompatible,    //!<  Case when a node points to a node of incompatible type (a declaration statement used in place of an expression -- means that there was some incorrect cast somewhere)
      unallocated,     //!<  Case when a node points to a deleted node
    } reason;

    defect_t<Kind::integrity_edges>(
      SgNode * source_,
      SgNode * target_,
      std::string label_,
      bool traversed_,
      bool container_,
      VariantT expected_,
      VariantT found_,
      Reason reason_
    );

    virtual void print(std::ostream & out) const;
  };
}
using IntegrityEdgeDefect = Defects::defect_t<Defects::Kind::integrity_edges>;

} }

#endif
