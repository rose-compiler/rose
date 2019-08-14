
#ifndef TYPEFORGE_OPERAND_DATA_H
#  define TYPEFORGE_OPERAND_DATA_H

#include <set>
#include <map>
#include <vector>
#include <string>

#include "OperandNetwork.h"

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

  std::set<SgLocatedNode *> predeccessors;
  std::set<SgLocatedNode *> successors;

  bool can_be_changed;
  bool from_system_files;

  std::vector<SgType *> casts;

  OperandData<OperandKind::base>(SgLocatedNode * const __lnode, OperandKind const & __opkind);

  static OperandData<OperandKind::base> * build(SgLocatedNode * node);
};

template <>
struct OperandData<OperandKind::variable> : OperandData<OperandKind::base> {
  // TODO fields

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
  // TODO fields

  OperandData<OperandKind::parameter>(SgInitializedName * const iname);
};

template <>
struct OperandData<OperandKind::varref> : OperandData<OperandKind::base> {
  // TODO fields

  OperandData<OperandKind::varref>(SgVarRefExp * const vexp);
};

template <>
struct OperandData<OperandKind::fref> : OperandData<OperandKind::base> {
  // TODO fields

  OperandData<OperandKind::fref>(SgFunctionRefExp * const vexp); // FIXME Ctor for methods too
};

template <>
struct OperandData<OperandKind::value> : OperandData<OperandKind::base> {
  // TODO fields

  OperandData<OperandKind::value>(SgValueExp * const vexp);
};

template <>
struct OperandData<OperandKind::arithmetic> : OperandData<OperandKind::base> {
  // TODO fields

  OperandData<OperandKind::arithmetic>(SgExpression * const expr); // FIXME Ctor for each *valid* AST node kind?
};

template <>
struct OperandData<OperandKind::call> : OperandData<OperandKind::base> {
  // TODO fields

  OperandData<OperandKind::call>(SgFunctionCallExp * const fcall);
};

template <>
struct OperandData<OperandKind::array_access> : OperandData<OperandKind::base> {
  // TODO fields

  OperandData<OperandKind::array_access>(SgPntrArrRefExp * const arrref);
};

template <>
struct OperandData<OperandKind::address_of> : OperandData<OperandKind::base> {
  // TODO fields

  OperandData<OperandKind::address_of>(SgAddressOfOp * const addof);
};

template <>
struct OperandData<OperandKind::dereference> : OperandData<OperandKind::base> {
  // TODO fields

  OperandData<OperandKind::dereference>(SgPointerDerefExp * const ptrref);
};

template <>
struct OperandData<OperandKind::unknown> : OperandData<OperandKind::base> {
  // TODO fields

  OperandData<OperandKind::unknown>(SgLocatedNode * const lnode);
};

}

#endif /* TYPEFORGE_OPERAND_DATA_H */

