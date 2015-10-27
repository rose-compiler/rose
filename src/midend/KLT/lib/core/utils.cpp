
#include "sage3basic.h"

#include "KLT/Core/descriptor.hpp"
#include "KLT/Core/utils.hpp"

#ifndef VERBOSE
# define VERBOSE 0
#endif

namespace KLT {

namespace Utils {

SgVariableSymbol * getTranslation(SgVariableSymbol * symbol, const symbol_map_t::vsym_translation_t & map) {
  symbol_map_t::vsym_translation_t::const_iterator it = map.find(symbol);
  if (it != map.end())
    return it->second;
  else
    return NULL;
}

SgVarRefExp * symbol_map_t::translate(SgVarRefExp * var_ref) const {
  SgVariableSymbol * symbol = var_ref->get_symbol();
  assert(symbol != NULL);

#if VERBOSE
  std::cerr << "[Info] (KLT::Utils::symbol_map_t::translate, SgVarRefExp) symbol = " << symbol->get_name().getString() << std::endl;
#endif

  SgVariableSymbol * loop_symbol = getTranslation(symbol, orig_loops);
  if (loop_symbol != NULL) return SageBuilder::buildVarRefExp(loop_symbol);

  SgVariableSymbol * data_symbol = getTranslation(symbol, data_trans);
  if (data_symbol != NULL) return SageBuilder::buildVarRefExp(data_symbol);

  SgVariableSymbol * param_symbol = getTranslation(symbol, parameters);
  if (param_symbol != NULL) return SageBuilder::buildVarRefExp(param_symbol);

  return NULL;
}

SgPntrArrRefExp * symbol_map_t::translate(SgPntrArrRefExp * arr_ref) const {

  // data(A) = { A , [ (0,VarRef(n)) , (0,VarRef(m)) , (0,VarRef(p)) ] }

  // ast(A[i][j][k])  = ArrRef(ArrRef(ArrRef(VarRef(A),VarRef(i)),VarRef(j)),VarRef(k));

  // translate(ast(A[i][j][k])) = ast(A[(i * m + j) * p + k])
  //     = ArrRef(VarRef(A), Add(Mul( Add(Mul(VarRef(i), VarRef(m)), VarRef(j)), VarRef(p)), VarRef(k)))

  SgPntrArrRefExp * parent_arr_ref = isSgPntrArrRefExp(arr_ref->get_parent());

  if (parent_arr_ref != NULL) return NULL;

  while (isSgPntrArrRefExp(arr_ref->get_lhs_operand_i()) != NULL) {
    arr_ref = isSgPntrArrRefExp(arr_ref->get_lhs_operand_i());
    assert(arr_ref != NULL);
  }

  parent_arr_ref = isSgPntrArrRefExp(arr_ref->get_parent());

  assert(arr_ref->get_lhs_operand_i() != NULL);

#if VERBOSE
  std::cerr << "[Info] (KLT::Utils::symbol_map_t::translate, SgPntrArrRefExp) lhs_var_ref = " << arr_ref->get_lhs_operand_i()->class_name() << std::endl;
#endif

  SgVarRefExp * lhs_var_ref = isSgVarRefExp(arr_ref->get_lhs_operand_i());
  if (lhs_var_ref == NULL) {
    // Either field or local: we should check
    return NULL;
  }
  SgVariableSymbol * data_symbol = isSgVariableSymbol(lhs_var_ref->get_symbol());
  assert(data_symbol != NULL);

#if VERBOSE
  std::cerr << "[Info] (KLT::Utils::symbol_map_t::translate, SgPntrArrRefExp) data_symbol = " << data_symbol->get_name().getString() << " (" << data_symbol << ")" << std::endl;
  std::cerr << "[Info] (KLT::Utils::symbol_map_t::translate, SgPntrArrRefExp) data.size() = " << data.size() << std::endl;
  {
    for (data_map_t::const_iterator it_data = data.begin(); it_data != data.end(); it_data++) {
      std::cerr << "[Info] (KLT::Utils::symbol_map_t::translate, SgPntrArrRefExp)     data.symbol = " << it_data->second->symbol->get_name().getString() << " (" << it_data->second->symbol << "," << it_data->first << ")" << std::endl;
    }
  }
#endif

  data_map_t::const_iterator it_data = data.find(data_symbol);
  if (it_data == data.end()) {
#if VERBOSE
    std::cerr << "[Warning] (KLT::Utils::symbol_map_t::translate, SgPntrArrRefExp) Variable \"" << data_symbol->get_name().getString() << "\" accessed as array but not a data. Assuming local declaration." << std::endl;
#endif
    return NULL;
  }
  std::vector<Descriptor::section_t *>::const_iterator it_section = it_data->second->sections.begin();
  // index = copy(rhs); // rhs starts with the first subscript
  SgExpression * index = SageInterface::copyExpression(arr_ref->get_rhs_operand_i());
  it_section++;
  while (it_section != it_data->second->sections.end()) {
    assert(parent_arr_ref != NULL);

    // index = Mul(index, copy(section.length)); // section.length starts at the second section
    index = SageBuilder::buildMultiplyOp(index, SageInterface::copyExpression((*it_section)->length));

    arr_ref = parent_arr_ref;
    parent_arr_ref = isSgPntrArrRefExp(arr_ref->get_parent());

    // index = Add(index, copy(rhs)); // 
    index = SageBuilder::buildAddOp(index, SageInterface::copyExpression(arr_ref->get_rhs_operand_i()));

    it_section++;
  }
  assert(parent_arr_ref == NULL);

  return SageBuilder::buildPntrArrRefExp(SageBuilder::buildVarRefExp(data_symbol), index);
}

SgExpression * symbol_map_t::translate(SgExpression * expr) const {
  SgExpression * res = SageInterface::copyExpression(expr);

  std::vector<SgPntrArrRefExp *> arr_refs = SageInterface::querySubTree<SgPntrArrRefExp>(res);
  std::map<SgPntrArrRefExp *, SgPntrArrRefExp *> arr_replacement_map;
  std::vector<SgPntrArrRefExp *>::const_iterator it_arr_ref;
  for (it_arr_ref = arr_refs.begin(); it_arr_ref != arr_refs.end(); it_arr_ref++) {
    SgPntrArrRefExp * arr_ref = translate(*it_arr_ref);
    if (arr_ref != NULL)
      arr_replacement_map.insert(std::pair<SgPntrArrRefExp *, SgPntrArrRefExp *>(*it_arr_ref, arr_ref));
  }
  std::map<SgPntrArrRefExp *, SgPntrArrRefExp *>::const_iterator it_arr_replacement;
  for (it_arr_replacement = arr_replacement_map.begin(); it_arr_replacement != arr_replacement_map.end(); it_arr_replacement++) 
    SageInterface::replaceExpression(it_arr_replacement->first, it_arr_replacement->second);

  std::vector<SgVarRefExp *> var_refs = SageInterface::querySubTree<SgVarRefExp>(res);
  std::vector<SgVarRefExp *>::const_iterator it_var_ref;
  for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
    SgVarRefExp * var_ref = translate(*it_var_ref);
    if (var_ref != NULL)
      SageInterface::replaceExpression(*it_var_ref, var_ref);
  }

  return res;
}

SgStatement * symbol_map_t::translate(SgStatement * stmt) const {
  SgStatement * res = SageInterface::copyStatement(stmt);

  std::vector<SgPntrArrRefExp *> arr_refs = SageInterface::querySubTree<SgPntrArrRefExp>(res);
  std::map<SgPntrArrRefExp *, SgPntrArrRefExp *> arr_replacement_map;
  std::vector<SgPntrArrRefExp *>::const_iterator it_arr_ref;
  for (it_arr_ref = arr_refs.begin(); it_arr_ref != arr_refs.end(); it_arr_ref++) {
    SgPntrArrRefExp * arr_ref = translate(*it_arr_ref);
    if (arr_ref != NULL)
      arr_replacement_map.insert(std::pair<SgPntrArrRefExp *, SgPntrArrRefExp *>(*it_arr_ref, arr_ref));
  }
  std::map<SgPntrArrRefExp *, SgPntrArrRefExp *>::const_iterator it_arr_replacement;
  for (it_arr_replacement = arr_replacement_map.begin(); it_arr_replacement != arr_replacement_map.end(); it_arr_replacement++) 
    SageInterface::replaceExpression(it_arr_replacement->first, it_arr_replacement->second);

  std::vector<SgVarRefExp *> var_refs = SageInterface::querySubTree<SgVarRefExp>(res);
  std::vector<SgVarRefExp *>::const_iterator it_var_ref;
  for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
    SgVarRefExp * var_ref = translate(*it_var_ref);
    if (var_ref != NULL)
      SageInterface::replaceExpression(*it_var_ref, var_ref);
  }

  return res;
}

}

}

