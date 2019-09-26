
#include "Typeforge/OpNet/CollapseRules.hpp"

namespace Typeforge {

namespace CollapseRules {

//////////////////////
// AssignOperator

std::pair<EdgeKind,bool> AssignOperator::link() {
  return std::pair<EdgeKind,bool>(EdgeKind::value, true);
}

lnode_ptr AssignOperator::predeccessor(lnode_ptr lhs, lnode_ptr rhs) {
  return lhs;
}

lnode_ptr AssignOperator::successor(lnode_ptr lhs, lnode_ptr rhs) {
  return lhs;
}

std::pair<EdgeKind,bool> AssignOperator::predeccessor(EdgeKind ekind) {
  return std::pair<EdgeKind,bool>(ekind | EdgeKind::value, true);
}

std::pair<EdgeKind,bool> AssignOperator::successor(EdgeKind ekind) {
  return std::pair<EdgeKind,bool>(EdgeKind::value | ekind, true);
}

const std::string AssignOperator::lhs{"ref"};
const std::string AssignOperator::rhs{"val"};

//////////////////////
// MemberAccess

std::pair<EdgeKind,bool> MemberAccess::link() {
  return std::pair<EdgeKind,bool>(EdgeKind::unknown, false);
}

lnode_ptr MemberAccess::predeccessor(lnode_ptr lhs, lnode_ptr rhs) {
  return rhs;
}

lnode_ptr MemberAccess::successor(lnode_ptr lhs, lnode_ptr rhs) {
  return rhs;
}

std::pair<EdgeKind,bool> MemberAccess::predeccessor(EdgeKind ekind) {
  return std::pair<EdgeKind,bool>(ekind | EdgeKind::value, true);
}

std::pair<EdgeKind,bool> MemberAccess::successor(EdgeKind ekind) {
  return std::pair<EdgeKind,bool>(EdgeKind::value | ekind, true);
}

const std::string MemberAccess::lhs{"obj"};
const std::string MemberAccess::rhs{"fld"};

//////////////////////
// ArrayAccess

std::pair<EdgeKind,bool> ArrayAccess::link() {
  return std::pair<EdgeKind,bool>(EdgeKind::unknown, false);
}

lnode_ptr ArrayAccess::predeccessor(lnode_ptr lhs, lnode_ptr rhs) {
  return lhs;
}

lnode_ptr ArrayAccess::successor(lnode_ptr lhs, lnode_ptr rhs) {
  return lhs;
}

std::pair<EdgeKind,bool> ArrayAccess::predeccessor(EdgeKind ekind) {
  return std::pair<EdgeKind,bool>(ekind | EdgeKind::deref, true);
}

std::pair<EdgeKind,bool> ArrayAccess::successor(EdgeKind ekind) {
  return std::pair<EdgeKind,bool>(EdgeKind::deref | ekind, true);
}

const std::string ArrayAccess::lhs{"arr"};
const std::string ArrayAccess::rhs{"idx"};

//////////////////////
// Dereference

lnode_ptr Dereference::predeccessor(lnode_ptr operand) {
  return operand;
}

lnode_ptr Dereference::successor(lnode_ptr operand) {
  return operand;
}

std::pair<EdgeKind,bool> Dereference::predeccessor(EdgeKind ekind) {
  return std::pair<EdgeKind,bool>(ekind | EdgeKind::deref, true);
}

std::pair<EdgeKind,bool> Dereference::successor(EdgeKind ekind) {
  return std::pair<EdgeKind,bool>(EdgeKind::deref | ekind, true);
}

const std::string Dereference::operand{"ref"};

//////////////////////
// AddressOf

lnode_ptr AddressOf::predeccessor(lnode_ptr operand) {
  return operand;
}

lnode_ptr AddressOf::successor(lnode_ptr operand) {
  return operand;
}

std::pair<EdgeKind,bool> AddressOf::predeccessor(EdgeKind ekind) {
  return std::pair<EdgeKind,bool>(ekind | EdgeKind::address, true);
}

std::pair<EdgeKind,bool> AddressOf::successor(EdgeKind ekind) {
  return std::pair<EdgeKind,bool>(EdgeKind::address | ekind, true);
}

const std::string AddressOf::operand{"obj"};

//////////////////////
// VarRef

lnode_ptr VarRef::predeccessor(lnode_ptr decl) {
  return decl;
}

lnode_ptr VarRef::successor(lnode_ptr decl) {
  return decl;
}

std::pair<EdgeKind,bool> VarRef::predeccessor(EdgeKind ekind) {
  return std::pair<EdgeKind,bool>(ekind | EdgeKind::value, true);
}

std::pair<EdgeKind,bool> VarRef::successor(EdgeKind ekind) {
  return std::pair<EdgeKind,bool>(EdgeKind::value | ekind, true);
}

//////////////////////
// FRef

lnode_ptr FRef::predeccessor(lnode_ptr decl) {
  return decl;
}

lnode_ptr FRef::successor(lnode_ptr decl) {
  return decl;
}

std::pair<EdgeKind,bool> FRef::predeccessor(EdgeKind ekind) {
  return std::pair<EdgeKind,bool>(ekind | EdgeKind::value, true);
}

std::pair<EdgeKind,bool> FRef::successor(EdgeKind ekind) {
  return std::pair<EdgeKind,bool>(EdgeKind::value | ekind, true);
}

//////////////////////
// ThisRef

lnode_ptr ThisRef::predeccessor(lnode_ptr decl) {
  return nullptr;
}

lnode_ptr ThisRef::successor(lnode_ptr decl) {
  return nullptr;
}

std::pair<EdgeKind,bool> ThisRef::predeccessor(EdgeKind ekind) {
  return std::pair<EdgeKind,bool>(EdgeKind::unknown, true);
}

std::pair<EdgeKind,bool> ThisRef::successor(EdgeKind ekind) {
  return std::pair<EdgeKind,bool>(EdgeKind::unknown, true);
}

//////////////////////
// Call

std::pair<EdgeKind,bool> Call::link() {
  return std::pair<EdgeKind,bool>(EdgeKind::unknown, false);
}

lnode_ptr Call::predeccessor(lnode_ptr fnc) {
  return fnc;
}

lnode_ptr Call::successor(lnode_ptr fnc) {
  return fnc;
}

std::pair<EdgeKind,bool> Call::predeccessor(EdgeKind ekind) {
  return std::pair<EdgeKind,bool>(ekind | EdgeKind::value, true);
}

std::pair<EdgeKind,bool> Call::successor(EdgeKind ekind) {
  return std::pair<EdgeKind,bool>(EdgeKind::value | ekind, true);
}

}

}

