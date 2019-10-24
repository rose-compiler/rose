
#ifndef TYPEFORGE_OPNET_COMMONS_H
#  define TYPEFORGE_OPNET_COMMONS_H

class SgLocatedNode;
class SgProject;
class SgType;

namespace Typeforge {
  typedef SgLocatedNode * lnode_ptr;
  
  enum class OperandKind {
    base,

// declarations
    variable,
    function,
    parameter,

// references & values
    varref,
    fref,
    thisref,
    value,

// operations:
    assign,
    unary_arithmetic,
    binary_arithmetic,
    call,
    array_access,
    address_of,
    dereference,
    member_access,

    unknown
  };
  
  enum class EdgeKind {
    value,
    deref,
    address,
    traversal,
    unknown
  };

  EdgeKind operator| (const EdgeKind & a, const EdgeKind & b);
  EdgeKind operator& (const EdgeKind & a, const EdgeKind & b);

  namespace CollapseRules {

    enum class Pattern {
      binary_operator,
      unary_operator,
      call_operator,
      reference,
      unknown
    };

  }

  bool is_template_with_dependent_return_type(lnode_ptr fnc);
}

#endif /* TYPEFORGE_OPNET_COMMONS_H */

