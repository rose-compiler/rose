
#ifndef TYPEFORGE_OPERAND_NETWORK_H
#  define TYPEFORGE_OPERAND_NETWORK_H

#include <string>
#include <ostream>
#include <map>
#include <set>

class SgLocatedNode;
class SgProject;
class SgType;

namespace Typeforge {
  
enum class OperandKind {
  base,

// declarations
  variable,
  function,
  parameter,

// references & values
  varref,
  fref,
  value,

// operations:
  arithmetic,
  call,
  array_access,
  address_of,
  dereference,

  unknown
};

template <OperandKind opkind>
struct OperandData;

class OperandNetwork {
  private:
    std::map<SgLocatedNode * const, OperandData<OperandKind::base> *> node_to_data;
    std::map<OperandKind const, std::set<SgLocatedNode *> > kind_to_nodes;
    std::map<std::string const, OperandData<OperandKind::base> *> handle_to_data;

  private:
    OperandData<OperandKind::base> * addNode(SgLocatedNode * node);

    void toDot_node(std::ostream & out, SgLocatedNode * node, OperandData<OperandKind::base> const * data) const;
    void toDot_edge(std::ostream & out, SgLocatedNode * source, SgLocatedNode * target) const;

  public:
    void initialize(SgProject * p = nullptr);

    std::string     const getHandle ( SgLocatedNode * const) const;
    SgLocatedNode * const getNode   ( std::string const & ) const;

    SgType * const getOriginalType ( SgLocatedNode       * const ) const;
    SgType * const getOriginalType ( std::string   const & ) const;

    std::string const getFilename ( SgLocatedNode       * const ) const;
    std::string const getFilename ( std::string   const & ) const;

    size_t const getLineStart ( SgLocatedNode       * const ) const;
    size_t const getLineStart ( std::string   const & ) const;

    size_t const getLineEnd ( SgLocatedNode       * const ) const;
    size_t const getLineEnd ( std::string   const & ) const;

    size_t const getColumnStart ( SgLocatedNode       * const ) const;
    size_t const getColumnStart ( std::string   const & ) const;

    size_t const getColumnEnd ( SgLocatedNode       * const ) const;
    size_t const getColumnEnd ( std::string   const & ) const;

    std::set<SgLocatedNode * const> const & getAll(const OperandKind opkind) const;

    void toDot(std::string const & fname) const;
    void toDot(std::ostream & out) const;
};

extern OperandNetwork opnet;

}

#endif /* TYPEFORGE_OPERAND_NETWORK_H */

