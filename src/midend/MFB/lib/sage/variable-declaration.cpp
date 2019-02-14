
#include "MFB/Sage/variable-declaration.hpp"
#include "MFB/Sage/namespace-declaration.hpp"

#include "sage3basic.h"

#ifndef VERBOSE
# define VERBOSE 0
#endif

namespace MFB {

bool ignore(const std::string & name);
bool ignore(SgScopeStatement * scope);

template <>
bool Driver<Sage>::resolveValidParent<SgVariableSymbol>(SgVariableSymbol * symbol) {
  SgSymbol * parent = NULL;
  
  if (p_valid_symbols.find(symbol) != p_valid_symbols.end()) return true;

  SgNamespaceDefinitionStatement * namespace_scope = isSgNamespaceDefinitionStatement(symbol->get_scope());
  SgClassDefinition              * class_scope     = isSgClassDefinition             (symbol->get_scope());
  if (namespace_scope != NULL) {
    SgNamespaceDeclarationStatement * parent_decl = namespace_scope->get_namespaceDeclaration();
    assert(parent_decl != NULL);
    parent = SageInterface::lookupNamespaceSymbolInParentScopes(parent_decl->get_name(), parent_decl->get_scope());
    assert(parent != NULL);

    if (!resolveValidParent<SgNamespaceSymbol>((SgNamespaceSymbol *)parent)) return false;
    assert(p_valid_symbols.find(parent) != p_valid_symbols.end());
  }
  else if (class_scope != NULL) {
    SgClassDeclaration * parent_decl = class_scope->get_declaration();
    assert(parent_decl != NULL);
    parent = SageInterface::lookupClassSymbolInParentScopes(parent_decl->get_name(), parent_decl->get_scope());
    assert(parent != NULL);

    if (!resolveValidParent<SgClassSymbol>((SgClassSymbol *)parent)) return false;
    assert(p_valid_symbols.find(parent) != p_valid_symbols.end());
  }

  p_valid_symbols.insert(symbol);
  p_parent_map.insert(std::pair<SgSymbol *, SgSymbol *>(symbol, parent));
  p_variable_symbols.insert(symbol);

  return true;
}

template <>
void  Driver<Sage>::loadSymbols<SgVariableDeclaration>(size_t file_id, SgSourceFile * file) {
  std::vector<SgVariableDeclaration *> variable_decl = SageInterface::querySubTree<SgVariableDeclaration>(file);

  std::set<SgVariableSymbol *> variable_symbols;
  std::vector<SgVariableDeclaration *>::const_iterator it_variable_decl;
  for (it_variable_decl = variable_decl.begin(); it_variable_decl != variable_decl.end(); it_variable_decl++) {
    SgVariableDeclaration * variable_decl = *it_variable_decl;
#if VERBOSE
    std::cout << "[Debug] (MFB::Driver<Sage>::loadSymbols<SgVariableDeclaration>) variable_decl = " << variable_decl << " (" << variable_decl->get_name().str() << ")" << std::endl;
#endif
    ROSE_ASSERT(variable_decl->get_variables().size() == 1);

    SgTemplateVariableDeclaration * tplvar_decl = isSgTemplateVariableDeclaration(variable_decl);

    SgInitializedName * init_name = variable_decl->get_variables()[0];
    ROSE_ASSERT(init_name != NULL);
    ROSE_ASSERT(init_name->get_scope() != NULL);

    SgInitializedName * prev_init_name = init_name->get_prev_decl_item();
    ROSE_ASSERT( !( prev_init_name != NULL ) || ( prev_init_name->get_scope() != NULL ) );
#if 1
    if (prev_init_name != NULL) {
      ROSE_ASSERT(init_name->get_scope() == prev_init_name->get_scope());
    }
#endif
    if (prev_init_name != NULL) continue;

    SgScopeStatement * scope = variable_decl->get_scope();
    ROSE_ASSERT(scope != NULL);
    if (ignore(scope)) continue;

    if (init_name->get_name().getString() == "") {
#if VERBOSE
      std::cout << "[Debug] (MFB::Driver<Sage>::loadSymbols<SgVariableDeclaration>) SgInitializedName with empty name!" << std::endl;
      std::cout << "[Debug] Parents: " << std::endl;
      SgNode * p = init_name->get_parent();
      while (p != NULL && !isSgFile(p)) {
        std::cout << "[Debug]  - " << std::hex << p << " (" << p->class_name() << ")" << std::endl;
        p = p->get_parent();
      }
#endif
      continue; // FIXME ROSE‌-1465
    }

    if (ignore(init_name->get_name().getString())) continue;

    SgVariableSymbol * variable_sym = NULL;
    if (tplvar_decl == NULL) {
      variable_sym = SageInterface::lookupVariableSymbolInParentScopes(init_name->get_name(), scope);
      if (variable_sym == NULL) {
        std::cerr << "[Error] (MFB::Driver<Sage>::loadSymbols<SgVariableDeclaration>) Not found: " << init_name->get_name().getString() << std::endl;
      }
      ROSE_ASSERT(variable_sym != NULL);
    } else {
      variable_sym = SageInterface::lookupTemplateVariableSymbolInParentScopes(init_name->get_name(), &(tplvar_decl->get_templateParameters()), &(tplvar_decl->get_templateSpecializationArguments()), scope);
      if (variable_sym == NULL) {
        std::cerr << "[Error] (MFB::Driver<Sage>::loadSymbols<SgVariableDeclaration>) Not found: " << init_name->get_name().getString() << " (templated variable)" << std::endl;
        continue;  // FIXME ROSE‌-1465
      }
      ROSE_ASSERT(variable_sym != NULL);
      ROSE_ASSERT(isSgTemplateVariableSymbol(variable_sym));
    }

    variable_symbols.insert(variable_sym);
  }

  std::set<SgVariableSymbol *>::iterator it;
  for (it = variable_symbols.begin(); it != variable_symbols.end(); it++)
    if (resolveValidParent<SgVariableSymbol>(*it)) {
      p_symbol_to_file_id_map[*it] = file_id;
#if VERBOSE
      std::cerr << "[Info] (MFB::Driver<Sage>::loadSymbols<SgVariableDeclaration>) Add: " << (*it)->get_name().getString() << " from File #" << file_id << std::endl;
#endif
    }
}

Sage<SgVariableDeclaration>::object_desc_t::object_desc_t(
  std::string name_,
  SgType * type_,
  SgInitializer * initializer_,
  SgSymbol * parent_,
  size_t file_id_,
  bool is_static_,
  bool create_definition_
) :
  name(name_),
  type(type_),
  initializer(initializer_),
  parent(parent_),
  file_id(file_id_),
  is_static(is_static_),
  create_definition(create_definition_)
{}


template <>
Sage<SgVariableDeclaration>::build_result_t Driver<Sage>::build<SgVariableDeclaration>(const Sage<SgVariableDeclaration>::object_desc_t & desc) {
  Sage<SgVariableDeclaration>::build_result_t result;

  SgScopeStatement * decl_scope = getBuildScopes<SgVariableDeclaration>(desc);

  SgVariableDeclaration * var_decl = SageBuilder::buildVariableDeclaration(desc.name, desc.type, desc.initializer, decl_scope);
  SageInterface::appendStatement(var_decl, decl_scope);

  result.symbol = decl_scope->lookup_variable_symbol(desc.name);
  assert(result.symbol != NULL);

  result.definition = var_decl->get_variables()[0];
  assert(result.definition != NULL);

  if (isSgClassSymbol(desc.parent) != NULL) {
    std::map<SgSymbol *, size_t>::iterator it_sym_to_file = p_symbol_to_file_id_map.find(desc.parent);
    assert(it_sym_to_file != p_symbol_to_file_id_map.end());
    p_symbol_to_file_id_map.insert(std::pair<SgSymbol *, size_t>(result.symbol, it_sym_to_file->second));
  }
  else
    p_symbol_to_file_id_map.insert(std::pair<SgSymbol *, size_t>(result.symbol, desc.file_id));

  p_valid_symbols.insert(result.symbol);
  p_parent_map.insert(std::pair<SgSymbol *, SgSymbol *>(result.symbol, desc.parent));
  p_variable_symbols.insert(result.symbol);

  return result;
}

template <>
Sage<SgVariableDeclaration>::build_scopes_t Driver<Sage>::getBuildScopes<SgVariableDeclaration>(const Sage<SgVariableDeclaration>::object_desc_t & desc) {
  Sage<SgVariableDeclaration>::build_scopes_t result = NULL;

  SgClassSymbol * class_symbol = isSgClassSymbol(desc.parent);
  SgNamespaceSymbol * namespace_symbol = isSgNamespaceSymbol(desc.parent);

  std::map<size_t, SgSourceFile *>::iterator it_file = id_to_file_map.find(desc.file_id);
  assert(it_file != id_to_file_map.end());
  SgSourceFile * file = it_file->second;
  assert(file != NULL);

  if (desc.parent == NULL)
    result = file->get_globalScope();
  else if (namespace_symbol != NULL)
    result = Sage<SgNamespaceDeclarationStatement>::getDefinition(namespace_symbol, file);
  else if (class_symbol != NULL)
    result = ((SgClassDeclaration *)class_symbol->get_declaration()->get_definingDeclaration())->get_definition();
  else assert(false); 

  assert(result != NULL);

  return result;
}

template <>
void Driver<Sage>::createForwardDeclaration<SgVariableDeclaration>(Sage<SgVariableDeclaration>::symbol_t symbol, size_t target_file_id) {
  std::map<size_t, SgSourceFile *>::iterator it_target_file = id_to_file_map.find(target_file_id);
  assert(it_target_file != id_to_file_map.end());
  SgSourceFile * target_file = it_target_file->second;
  assert(target_file != NULL);

  assert(false);
}

}

