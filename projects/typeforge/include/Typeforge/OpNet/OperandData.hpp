
#ifndef TYPEFORGE_OPERAND_DATA_H
#  define TYPEFORGE_OPERAND_DATA_H

#include "Typeforge/OpNet/commons.hpp"

#include <set>
#include <map>
#include <vector>
#include <string>

class SgType;
class SgLocatedNode;
class SgInitializedName;
class SgStatement;
class SgScopeStatement;
class SgVariableDeclaration;
class SgFunctionDeclaration;
class SgExpression;
class SgFunctionCallExp;
class SgPntrArrRefExp;
class SgAddressOfOp;
class SgPointerDerefExp;
class SgVarRefExp;
class SgFunctionRefExp;
class SgValueExp;

namespace Typeforge {

template <OperandKind opkind>
struct OperandData;

template <>
struct OperandData<OperandKind::base> {
  OperandKind opkind;

  SgLocatedNode * lnode;
  SgType * original_type;
  SgStatement * scope;

  std::string handle;
  std::string filename;
  size_t line_start;
  size_t column_start;
  size_t line_end;
  size_t column_end;

  bool can_be_changed;
  bool from_system_files;

  std::vector<SgType *> casts;

  std::map<std::string, SgLocatedNode *> edge_labels;

  OperandData<OperandKind::base>(SgLocatedNode * const __lnode, OperandKind const & __opkind);

  static OperandData<OperandKind::base> * build(SgLocatedNode * node);
};

template <>
struct OperandData<OperandKind::variable> : OperandData<OperandKind::base> {
  OperandData<OperandKind::variable>(SgVariableDeclaration * const vdecl);
};

template <>
struct OperandData<OperandKind::function> : OperandData<OperandKind::base> {
  // defining declaration
  SgFunctionDeclaration * definition;

  // associated tpl decl either SgTemplateFunctionDeclaration or SgTemplateMemberFunctionDeclaration
  SgFunctionDeclaration * tpl_decl;

  // return type in tpl. decl. is based directly (not subtype) on specified "template type parameter"
  //   - only valid if `tpl_decl != nullptr`
  size_t template_argument_position_for_return_type;

  // associates the type of each function parameter to the set of template parameters it depends on
  std::map<size_t, std::set<size_t> > fparam_type_to_tplparam_map;

  // Potential function overloads and associated parameter types
  std::map<SgFunctionDeclaration *, std::vector<SgType *> > potential_overloads;

  OperandData<OperandKind::function>(SgFunctionDeclaration * const fdecl);
};

template <>
struct OperandData<OperandKind::parameter> : OperandData<OperandKind::base> {
  OperandData<OperandKind::parameter>(SgInitializedName * const iname);
};

template <>
struct OperandData<OperandKind::varref> : OperandData<OperandKind::base> {
  OperandData<OperandKind::varref>(SgVarRefExp * const vexp);
};

template <>
struct OperandData<OperandKind::fref> : OperandData<OperandKind::base> {
  OperandData<OperandKind::fref>(SgFunctionRefExp * const fref);
  OperandData<OperandKind::fref>(SgMemberFunctionRefExp * const mfref);
};

template <>
struct OperandData<OperandKind::thisref> : OperandData<OperandKind::base> {
  OperandData<OperandKind::thisref>(SgThisExp * const thisexp);
};

template <>
struct OperandData<OperandKind::value> : OperandData<OperandKind::base> {
  OperandData<OperandKind::value>(SgValueExp * const vexp);
};

template <>
struct OperandData<OperandKind::assign> : OperandData<OperandKind::base> {
  OperandData<OperandKind::assign>(SgBinaryOp * const bop);
  OperandData<OperandKind::assign>(SgAssignInitializer * const assinit);
};

template <>
struct OperandData<OperandKind::member_access> : OperandData<OperandKind::base> {
  OperandData<OperandKind::member_access>(SgBinaryOp * const bop); // FIXME Ctor for each *valid* AST node kind?
};

template <>
struct OperandData<OperandKind::unary_arithmetic> : OperandData<OperandKind::base> {
  OperandData<OperandKind::unary_arithmetic>(SgUnaryOp * const uop);
};

template <>
struct OperandData<OperandKind::binary_arithmetic> : OperandData<OperandKind::base> {
  OperandData<OperandKind::binary_arithmetic>(SgBinaryOp * const binop);
};

template <>
struct OperandData<OperandKind::call> : OperandData<OperandKind::base> {
  OperandData<OperandKind::call>(SgFunctionCallExp * const fcall);
};

template <>
struct OperandData<OperandKind::array_access> : OperandData<OperandKind::base> {
  OperandData<OperandKind::array_access>(SgPntrArrRefExp * const arrref);
};

template <>
struct OperandData<OperandKind::address_of> : OperandData<OperandKind::base> {
  OperandData<OperandKind::address_of>(SgAddressOfOp * const addof);
};

template <>
struct OperandData<OperandKind::dereference> : OperandData<OperandKind::base> {
  OperandData<OperandKind::dereference>(SgPointerDerefExp * const ptrref);
};

template <>
struct OperandData<OperandKind::unknown> : OperandData<OperandKind::base> {
  OperandData<OperandKind::unknown>(SgLocatedNode * const lnode);
};

}

#endif /* TYPEFORGE_OPERAND_DATA_H */

