
#ifdef NO_TEMPLATE_DEFINITION
#error "This is a template definition file and NO_TEMPLATE_DEFINITION is define..."
#endif


#include "sage3basic.h"

namespace MultiFileBuilder {

template <typename Object>
typename Sage<Object>::symbol_t Driver<Sage>::useSymbol(typename Sage<Object>::symbol_t symbol, SgSourceFile * file, bool need_forward_only) {
  std::map<SgSymbol *, unsigned long>::iterator it_sym_decl_file_id = p_symbol_to_file_id_map.find(symbol);
  assert(it_sym_decl_file_id != p_symbol_to_file_id_map.end());

  unsigned long sym_decl_file_id = it_sym_decl_file_id->second;

  bool need_include_directive = !need_forward_only;

  // TODO find out wether we need a include directive or if we can do with a forward declaration

  if (need_include_directive) {
    std::map<SgSourceFile *, std::set<unsigned long> >::iterator it_accessible_file_ids = file_id_to_accessible_file_id_map.find(file);
    assert(it_accessible_file_ids != file_id_to_accessible_file_id_map.end());

    std::set<unsigned long> & accessible_file_ids = it_accessible_file_ids->second;
    std::set<unsigned long>::iterator accessible_file_id = accessible_file_ids.find(sym_decl_file_id);
    if (accessible_file_id == accessible_file_ids.end()) {
      addIncludeDirectives(file, sym_decl_file_id);
      accessible_file_ids.insert(sym_decl_file_id);
    }

  }
  else {
    createForwardDeclaration<Object>(symbol, file);
  }

  // No change of symbol
  return symbol;
}

template <typename Object>
typename Sage<Object>::symbol_t Driver<Sage>::useSymbol(typename Sage<Object>::symbol_t symbol, unsigned long file_id, bool needed_in_header, bool need_forward_only) {
  SgSourceFile * target_file = NULL;

  std::map<unsigned long, std::pair<SgSourceFile *, SgSourceFile *> >::iterator it_file_pair = file_pair_map.find(file_id);
  std::map<unsigned long, SgSourceFile *>::iterator it_standalone_source_file = standalone_source_file_map.find(file_id);

  assert((it_file_pair != file_pair_map.end()) xor (it_standalone_source_file != standalone_source_file_map.end()));

  if (it_file_pair != file_pair_map.end()) {
    if (needed_in_header)
      target_file = it_file_pair->second.first;
    else
      target_file = it_file_pair->second.second;
  }
  else if (it_standalone_source_file != standalone_source_file_map.end())
    target_file = it_standalone_source_file->second;
  else assert(false);

  return useSymbol<Object>(symbol, target_file, need_forward_only);
}

template <typename Object>
typename Sage<Object>::symbol_t Driver<Sage>::useSymbol(typename Sage<Object>::symbol_t symbol, SgScopeStatement * scope, bool need_forward_only) {
  SgFile * enclosing_file = SageInterface::getEnclosingFileNode(scope);
  assert(enclosing_file != NULL); // FIXME Contingency : Scope Stack

  SgSourceFile * enclosing_source_file = isSgSourceFile(enclosing_file);
  assert(enclosing_source_file != NULL);

  return useSymbol<Object>(symbol, enclosing_source_file, need_forward_only);
}

template <typename Object>
void Driver<Sage>::createForwardDeclaration(typename Sage<Object>::symbol_t symbol, SgSourceFile * target_file) {
  assert(false);
}

}

