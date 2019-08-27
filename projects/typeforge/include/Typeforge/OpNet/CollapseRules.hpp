
#ifndef TYPEFORGE_COLLAPSE_RULES_H
#  define TYPEFORGE_COLLAPSE_RULES_H

#include "Typeforge/OpNet/commons.hpp"
#include "Typeforge/OpNet/OperandNetwork.hpp"

#include <string>
#include <ostream>
#include <map>
#include <set>

namespace Typeforge {

namespace CollapseRules {

struct AssignOperator {
  static constexpr OperandKind opkind = OperandKind::assign;
  static constexpr Pattern pattern = Pattern::binary_operator;
  static const std::string lhs;
  static const std::string rhs;

  static std::pair<EdgeKind,bool> link();

  static lnode_ptr predeccessor(lnode_ptr lhs, lnode_ptr rhs);
  static lnode_ptr successor(lnode_ptr lhs, lnode_ptr rhs);

  static std::pair<EdgeKind,bool> predeccessor(EdgeKind ekind);
  static std::pair<EdgeKind,bool> successor(EdgeKind ekind);
};

struct MemberAccess {
  static constexpr OperandKind opkind = OperandKind::member_access;
  static constexpr Pattern pattern = Pattern::binary_operator;
  static const std::string lhs;
  static const std::string rhs;

  static std::pair<EdgeKind,bool> link();

  static lnode_ptr predeccessor(lnode_ptr lhs, lnode_ptr rhs);
  static lnode_ptr successor(lnode_ptr lhs, lnode_ptr rhs);

  static std::pair<EdgeKind,bool> predeccessor(EdgeKind ekind);
  static std::pair<EdgeKind,bool> successor(EdgeKind ekind);
};

struct ArrayAccess {
  static constexpr OperandKind opkind = OperandKind::array_access;
  static constexpr Pattern pattern = Pattern::binary_operator;
  static const std::string lhs;
  static const std::string rhs;

  static std::pair<EdgeKind,bool> link();

  static lnode_ptr predeccessor(lnode_ptr lhs, lnode_ptr rhs);
  static lnode_ptr successor(lnode_ptr lhs, lnode_ptr rhs);

  static std::pair<EdgeKind,bool> predeccessor(EdgeKind ekind);
  static std::pair<EdgeKind,bool> successor(EdgeKind ekind);
};

struct Dereference {
  static constexpr OperandKind opkind = OperandKind::dereference;
  static constexpr Pattern pattern = Pattern::unary_operator;
  static const std::string operand;

  static lnode_ptr predeccessor(lnode_ptr operand);
  static lnode_ptr successor(lnode_ptr operand);

  static std::pair<EdgeKind,bool> predeccessor(EdgeKind ekind);
  static std::pair<EdgeKind,bool> successor(EdgeKind ekind);
};

struct AddressOf {
  static constexpr OperandKind opkind = OperandKind::address_of;
  static constexpr Pattern pattern = Pattern::unary_operator;
  static const std::string operand;

  static lnode_ptr predeccessor(lnode_ptr operand);
  static lnode_ptr successor(lnode_ptr operand);

  static std::pair<EdgeKind,bool> predeccessor(EdgeKind ekind);
  static std::pair<EdgeKind,bool> successor(EdgeKind ekind);
};

struct VarRef {
  static constexpr OperandKind opkind = OperandKind::varref;
  static constexpr Pattern pattern = Pattern::reference;

  static lnode_ptr predeccessor(lnode_ptr decl);
  static lnode_ptr successor(lnode_ptr decl);

  static std::pair<EdgeKind,bool> predeccessor(EdgeKind ekind);
  static std::pair<EdgeKind,bool> successor(EdgeKind ekind);
};

struct FRef {
  static constexpr OperandKind opkind = OperandKind::fref;
  static constexpr Pattern pattern = Pattern::reference;

  static lnode_ptr predeccessor(lnode_ptr decl);
  static lnode_ptr successor(lnode_ptr decl);

  static std::pair<EdgeKind,bool> predeccessor(EdgeKind ekind);
  static std::pair<EdgeKind,bool> successor(EdgeKind ekind);
};

struct ThisRef {
  static constexpr OperandKind opkind = OperandKind::thisref;
  static constexpr Pattern pattern = Pattern::reference;

  static lnode_ptr predeccessor(lnode_ptr decl);
  static lnode_ptr successor(lnode_ptr decl);

  static std::pair<EdgeKind,bool> predeccessor(EdgeKind ekind);
  static std::pair<EdgeKind,bool> successor(EdgeKind ekind);
};

struct Call {
  static constexpr OperandKind opkind = OperandKind::call;
  static constexpr Pattern pattern = Pattern::call_operator;

  static std::pair<EdgeKind,bool> link();

  static lnode_ptr predeccessor(lnode_ptr fnc);
  static lnode_ptr successor(lnode_ptr fnc);

  static std::pair<EdgeKind,bool> predeccessor(EdgeKind ekind);
  static std::pair<EdgeKind,bool> successor(EdgeKind ekind);
};

}

}

#endif /* TYPEFORGE_COLLAPSE_RULES_H */

