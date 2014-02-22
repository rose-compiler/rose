/** 
 * \file MFB/include/MFB.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifdef NO_TEMPLATE_DEFINITION
#error "This is a template definition file and NO_TEMPLATE_DEFINITION is define..."
#endif

#include "sage3basic.h"

namespace MFB {

/*!
 * \addtogroup grp_mfb_sage_driver
 * @{
*/

template <typename Object>
typename Sage<Object>::symbol_t Driver<Sage>::useSymbol(typename Sage<Object>::symbol_t symbol, unsigned file_id, bool need_forward_only) {
  std::map<unsigned, SgSourceFile *>::iterator it_file = id_to_file_map.find(file_id);
  assert(it_file != id_to_file_map.end());

  std::map<SgSymbol *, unsigned>::iterator it_sym_decl_file_id = p_symbol_to_file_id_map.find(symbol);
  assert(it_sym_decl_file_id != p_symbol_to_file_id_map.end());

  unsigned long sym_decl_file_id = it_sym_decl_file_id->second;

  bool need_include_directive = !need_forward_only;

  // TODO find out wether we need a include directive or if we can do with a forward declaration

  if (need_include_directive) {
    std::map<unsigned, std::set<unsigned> >::iterator it_accessible_file_ids = file_id_to_accessible_file_id_map.find(file_id);
    assert(it_accessible_file_ids != file_id_to_accessible_file_id_map.end());

    std::set<unsigned> & accessible_file_ids = it_accessible_file_ids->second;
    std::set<unsigned>::iterator accessible_file_id = accessible_file_ids.find(sym_decl_file_id);
    if (accessible_file_id == accessible_file_ids.end()) {
      addIncludeDirectives(file_id, sym_decl_file_id);
      accessible_file_ids.insert(sym_decl_file_id);
    }

    addPointerToTopParentDeclaration(symbol, file_id);
  }
  else {
    createForwardDeclaration<Object>(symbol, file_id);
  }

  // No change of symbol
  return symbol;
}

template <typename Object>
void Driver<Sage>::createForwardDeclaration(typename Sage<Object>::symbol_t symbol, unsigned target_file_id) {
  assert(false);
}

/** @} */

}

