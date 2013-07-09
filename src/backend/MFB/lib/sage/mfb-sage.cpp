
#include "MFB/Sage/mfb-sage.hpp"

#include "sage3basic.h"

#define PATCHING_SAGE_BUILDER_ISSUES 1

namespace MultiFileBuilder {

// SgFunctionDeclaration

Sage<SgFunctionDeclaration>::object_desc_t::object_desc_t(
  std::string name_,
  SgType * return_type_,
  SgFunctionParameterList * params_,
  SgNamespaceSymbol * parent_namespace_,
  unsigned long file_id_,
  bool is_static_, 
  bool create_definition_
) :
  name(name_),
  return_type(return_type_),
  params(params_),
  parent(parent_namespace_),
  file_id(file_id_),
  is_static(is_static_),
  create_definition(create_definition_)
{}

template <>
template <>
Sage<SgFunctionDeclaration>::build_result_t Driver<Sage>::build<SgFunctionDeclaration>(const Sage<SgFunctionDeclaration>::object_desc_t & desc) {
  Sage<SgFunctionDeclaration>::build_result_t result;

  Sage<SgFunctionDeclaration>::build_scopes_t scopes = getBuildScopes<SgFunctionDeclaration>(desc);

  SgScopeStatement * decl_scope = scopes.decl_scope;
  SgScopeStatement * defn_scope = scopes.defn_scope;

  SgFunctionDeclaration * func_decl = NULL;
  {
    func_decl = SageBuilder::buildNondefiningFunctionDeclaration(desc.name, desc.return_type, desc.params, decl_scope, NULL, false, NULL);
    SageInterface::appendStatement(func_decl, decl_scope);

    assert(func_decl->get_definition() == NULL);
  }

  SgFunctionDeclaration * func_defn = NULL;
  {
    func_defn = SageBuilder::buildDefiningFunctionDeclaration(desc.name, desc.return_type, desc.params, defn_scope, NULL, false, func_decl, NULL);
    SageInterface::appendStatement(func_defn, defn_scope);
  
    result.definition = func_defn->get_definition();
    assert(result.definition != NULL);
  }

  {
    result.symbol = decl_scope->lookup_function_symbol(desc.name);
    assert(result.symbol != NULL);

    symbol_to_file_id_map.insert(std::pair<SgSymbol *, unsigned long>(result.symbol, desc.file_id));

    parent_map.insert(std::pair<SgSymbol *, SgSymbol *>(result.symbol, desc.parent));
  }

#if PATCHING_SAGE_BUILDER_ISSUES
  {
    func_decl->set_definingDeclaration(func_defn);
    func_decl->set_firstNondefiningDeclaration(func_decl);

    func_defn->set_definingDeclaration(func_defn);
    func_defn->set_firstNondefiningDeclaration(func_decl);
  }
#endif

  return result;
}

template <>
template <>
Sage<SgFunctionDeclaration>::build_scopes_t Driver<Sage>::getBuildScopes<SgFunctionDeclaration>(const Sage<SgFunctionDeclaration>::object_desc_t & desc) {
  Sage<SgFunctionDeclaration>::build_scopes_t result;

  assert(desc.file_id != 0);

  std::map<unsigned long, std::pair<SgSourceFile *, SgSourceFile *> >::iterator it_file_pair = file_pair_map.find(desc.file_id);
  std::map<unsigned long, SgSourceFile *>::iterator it_standalone_source_file = standalone_source_file_map.find(desc.file_id);

  assert((it_file_pair != file_pair_map.end()) xor (it_standalone_source_file != standalone_source_file_map.end()));

  SgSourceFile * decl_file = NULL;
  SgSourceFile * defn_file = NULL;
  if (it_file_pair != file_pair_map.end()) {
    decl_file = it_file_pair->second.first;
    defn_file = it_file_pair->second.second;
  }
  else if (it_standalone_source_file != standalone_source_file_map.end()) {
    decl_file = it_standalone_source_file->second;
    defn_file = it_standalone_source_file->second;
  }
  else assert(false);

  assert(decl_file != NULL && defn_file != NULL);

  if (desc.parent == NULL) {
    result.decl_scope = decl_file->get_globalScope();
    result.defn_scope = defn_file->get_globalScope();
  }
  else {
    result.decl_scope = Sage<SgNamespaceDeclarationStatement>::getDefinition(desc.parent, decl_file);
    result.defn_scope = Sage<SgNamespaceDeclarationStatement>::getDefinition(desc.parent, defn_file);
  }

  assert(result.decl_scope != NULL && result.defn_scope != NULL);

  return result;
}

template <>
template <>
void Driver<Sage>::createForwardDeclaration<SgFunctionDeclaration>(Sage<SgFunctionDeclaration>::symbol_t symbol, SgSourceFile * target_file) {
  assert(false);
}

// SgMemberFunctionDeclaration

Sage<SgMemberFunctionDeclaration>::object_desc_t::object_desc_t(
  std::string name_,
  SgType * return_type_,
  SgFunctionParameterList * params_,
  SgClassSymbol * parent_class_,
  unsigned long file_id_,
  bool is_static_,
  bool is_virtual_,
  bool is_constructor_,
  bool is_destructor_,
  bool create_definition_
) :
  name(name_),
  return_type(return_type_),
  params(params_),
  parent(parent_class_),
  file_id(file_id_),
  is_static(is_static_),
  is_virtual(is_virtual_),
  is_constructor(is_constructor_),
  is_destructor(is_destructor_),
  create_definition(create_definition_)
{}

template <>
template <>
Sage<SgMemberFunctionDeclaration>::build_result_t Driver<Sage>::build<SgMemberFunctionDeclaration>(const Sage<SgMemberFunctionDeclaration>::object_desc_t & desc) {
  Sage<SgMemberFunctionDeclaration>::build_result_t result;

  Sage<SgMemberFunctionDeclaration>::build_scopes_t scopes = getBuildScopes<SgMemberFunctionDeclaration>(desc);
  
  SgScopeStatement * decl_scope = scopes.decl_scope;
  SgScopeStatement * defn_scope = scopes.defn_scope;

  assert(isSgClassDefinition(decl_scope));
  assert(decl_scope != defn_scope);

  // Build Declaration
  SgMemberFunctionDeclaration * mfunc_decl = NULL;
  {
    mfunc_decl = SageBuilder::buildNondefiningMemberFunctionDeclaration(desc.name, desc.return_type, desc.params, decl_scope, NULL, 0, false, NULL);
    SageInterface::appendStatement(mfunc_decl, decl_scope);

    if (desc.is_constructor)
      mfunc_decl->get_specialFunctionModifier().setConstructor();

    assert(mfunc_decl->get_definition() == NULL);
    assert(mfunc_decl->get_associatedClassDeclaration() != NULL);
  }

  // Build Definition
  SgMemberFunctionDeclaration * mfunc_defn = NULL;
  {
    mfunc_defn = SageBuilder::buildDefiningMemberFunctionDeclaration(desc.name, desc.return_type, desc.params, decl_scope, NULL, 0, false, mfunc_decl, NULL);
    SageInterface::appendStatement(mfunc_defn, defn_scope);

    if (desc.is_constructor)
      mfunc_defn->get_specialFunctionModifier().setConstructor();

    assert(mfunc_defn->get_associatedClassDeclaration() != NULL);

    result.definition = mfunc_defn->get_definition();
    assert(result.definition != NULL);
  }

  { // Symbol handling
    result.symbol = isSgMemberFunctionSymbol(decl_scope->lookup_function_symbol(desc.name));
    assert(result.symbol != NULL);

    std::map<SgSymbol *, unsigned long>::iterator it_symbol_to_file_id = symbol_to_file_id_map.find(desc.parent);
    assert(it_symbol_to_file_id != symbol_to_file_id_map.end());

    symbol_to_file_id_map.insert(std::pair<SgSymbol *, unsigned long>(result.symbol, it_symbol_to_file_id->second));

    parent_map.insert(std::pair<SgSymbol *, SgSymbol *>(result.symbol, desc.parent));
  }

#if PATCHING_SAGE_BUILDER_ISSUES
  // Decl and Defn
  { 
    mfunc_decl->set_definingDeclaration(mfunc_defn);
    mfunc_decl->set_firstNondefiningDeclaration(mfunc_decl);

    mfunc_defn->set_definingDeclaration(mfunc_defn);
    mfunc_defn->set_firstNondefiningDeclaration(mfunc_decl);
  }
#endif

  return result;
}

template <>
template <>
Sage<SgMemberFunctionDeclaration>::build_scopes_t Driver<Sage>::getBuildScopes<SgMemberFunctionDeclaration>(const Sage<SgMemberFunctionDeclaration>::object_desc_t & desc) {
  Sage<SgMemberFunctionDeclaration>::build_scopes_t result;

  assert(desc.parent != NULL);

  {
    SgClassDeclaration * class_decl = desc.parent->get_declaration();
    assert(class_decl != NULL);
    class_decl = isSgClassDeclaration(class_decl->get_definingDeclaration());
    assert(class_decl != NULL);
    SgClassDefinition * class_defn = isSgClassDefinition(class_decl->get_definition());
    assert(class_defn != NULL);
    result.decl_scope = class_defn;
  }

  if (desc.create_definition) {
    unsigned long file_id = desc.file_id;

    if (file_id == 0) {
      std::map<SgSymbol *, unsigned long>::iterator it_symbol_to_file_id = symbol_to_file_id_map.find(desc.parent);
      assert(it_symbol_to_file_id != symbol_to_file_id_map.end());

      file_id = it_symbol_to_file_id->second;
    }
    assert(file_id != 0);

    std::map<unsigned long, std::pair<SgSourceFile *, SgSourceFile *> >::iterator it_file_pair = file_pair_map.find(file_id);
    std::map<unsigned long, SgSourceFile *>::iterator it_standalone_source_file = standalone_source_file_map.find(file_id);

    assert((it_file_pair != file_pair_map.end()) xor (it_standalone_source_file != standalone_source_file_map.end()));

    SgSourceFile * defn_file = NULL;
    if (it_file_pair != file_pair_map.end())
      defn_file = it_file_pair->second.second; // the source file
    else if (it_standalone_source_file != standalone_source_file_map.end())
      defn_file = it_standalone_source_file->second; // decl local to the source file
    else assert(false);
    assert(defn_file != NULL);

    result.defn_scope = defn_file->get_globalScope();
  }
  else result.defn_scope = NULL;

  return result;
}

// SgTemplateInstantiationMemberFunctionDecl

// SgTemplateInstantiationFunctionDecl

// SgClassDeclaration

Sage<SgClassDeclaration>::object_desc_t::object_desc_t(
  std::string name_,
  unsigned long kind_,
  SgSymbol * parent_,
  unsigned long file_id_,
  bool create_definition_
) :
  name(name_),
  kind(kind_),
  parent(parent_),
  file_id(file_id_),
  create_definition(create_definition_)
{}

template <>
template <>
Sage<SgClassDeclaration>::build_result_t Driver<Sage>::build<SgClassDeclaration>(const Sage<SgClassDeclaration>::object_desc_t & desc) {
  Sage<SgClassDeclaration>::build_result_t result;

  assert(desc.file_id != 0);

  Sage<SgClassDeclaration>::build_scopes_t scopes = getBuildScopes<SgClassDeclaration>(desc);

  SgScopeStatement * decl_scope = scopes.decl_scope;

  // Decl
  SgClassDeclaration * class_decl = SageBuilder::buildNondefiningClassDeclaration_nfi(desc.name, (SgClassDeclaration::class_types)desc.kind, decl_scope, false, NULL);

  // Symbol handling
  {
    result.symbol = decl_scope->lookup_class_symbol(desc.name);
    assert(result.symbol != NULL);

    symbol_to_file_id_map.insert(std::pair<SgSymbol *, unsigned long>(result.symbol, desc.file_id));

    parent_map.insert(std::pair<SgSymbol *, SgSymbol *>(result.symbol, desc.parent));
  }

  // Defining decl
  SgClassDeclaration * class_defn = NULL;
  {
    class_defn = SageBuilder::buildNondefiningClassDeclaration_nfi(desc.name, (SgClassDeclaration::class_types)desc.kind, decl_scope, false, NULL);
    SageInterface::appendStatement(class_defn, decl_scope);

    result.definition = SageBuilder::buildClassDefinition_nfi(class_defn, false);
    class_defn->set_definition(result.definition);
    class_defn->unsetForward(); 
  
    assert(class_decl->get_definition() == NULL);
    assert(class_defn->get_definition() != NULL);
    assert(!class_defn->isForward());
  }

#if PATCHING_SAGE_BUILDER_ISSUES
  { // connection between decl/defn
    class_decl->set_definingDeclaration(class_defn);
    class_decl->set_firstNondefiningDeclaration(class_decl);

    class_defn->set_definingDeclaration(class_defn);
    class_defn->set_firstNondefiningDeclaration(class_decl);
  }
#endif

  return result;
}

template <>
template <>
Sage<SgClassDeclaration>::build_scopes_t Driver<Sage>::getBuildScopes<SgClassDeclaration>(const Sage<SgClassDeclaration>::object_desc_t & desc) {
  Sage<SgClassDeclaration>::build_scopes_t result;

  SgClassSymbol * class_parent = isSgClassSymbol(desc.parent);
  SgNamespaceSymbol * namespace_parent = isSgNamespaceSymbol(desc.parent);

  assert(desc.parent == NULL || ((class_parent != NULL) xor (namespace_parent != NULL))); // if parent, it needs to be either a class or a namespace

  if (class_parent != NULL) {
    assert(desc.file_id == 0);

    assert(false); // NIY
  }
  else {
    std::map<unsigned long, std::pair<SgSourceFile *, SgSourceFile *> >::iterator it_file_pair = file_pair_map.find(desc.file_id);
    std::map<unsigned long, SgSourceFile *>::iterator it_standalone_source_file = standalone_source_file_map.find(desc.file_id);

    assert((it_file_pair != file_pair_map.end()) xor (it_standalone_source_file != standalone_source_file_map.end()));

    SgSourceFile * decl_file = NULL;
    if (it_file_pair != file_pair_map.end())
      decl_file = it_file_pair->second.first; // the header file
    else if (it_standalone_source_file != standalone_source_file_map.end())
      decl_file = it_standalone_source_file->second; // decl local to the source file
    else assert(false);

    assert(decl_file != NULL);

    if (namespace_parent == NULL)
      result.decl_scope = decl_file->get_globalScope();
    else {
      result.decl_scope = Sage<SgNamespaceDeclarationStatement>::getDefinition(namespace_parent, decl_file);
    }
  }

  return result;
}

template <>
template <>
void Driver<Sage>::createForwardDeclaration<SgClassDeclaration>(Sage<SgClassDeclaration>::symbol_t symbol, SgSourceFile * target_file) {
  std::map<SgSymbol *, SgSymbol *>::iterator it_parent = parent_map.find(symbol);
  assert(it_parent != parent_map.end());

  SgClassDeclaration * orig_decl = symbol->get_declaration();
  SgScopeStatement * scope = NULL;
  SgScopeStatement * insert_scope = NULL;

  SgSymbol * parent = it_parent->second;
  if (parent == NULL) {
    scope = target_file->get_globalScope();
    insert_scope = scope;
  }
  else {
    assert(false); // TODO
  }

  SgClassDeclaration * class_decl = SageBuilder::buildNondefiningClassDeclaration_nfi(symbol->get_name(), orig_decl->get_class_type(), scope, false, NULL);
  SageInterface::prependStatement(class_decl, insert_scope);
}

// SgTemplateInstantiationDecl

// SgTypedefDeclaration

// SgVariableDeclaration

template <>
template <>
Sage<SgVariableDeclaration>::build_result_t Driver<Sage>::build<SgVariableDeclaration>(const Sage<SgVariableDeclaration>::object_desc_t & desc) {
  Sage<SgVariableDeclaration>::build_result_t result;

  Sage<SgVariableDeclaration>::build_scopes_t scopes = getBuildScopes<SgVariableDeclaration>(desc);

  SgScopeStatement * decl_scope = scopes.scope;

  SgVariableDeclaration * var_decl = SageBuilder::buildVariableDeclaration(desc.name, desc.type, desc.initializer, decl_scope);

  result.symbol = decl_scope->lookup_variable_symbol(desc.name);
  assert(result.symbol != NULL);

  result.definition = var_decl->get_variables()[0];
  assert(result.definition != NULL);

  return result;
}

template <>
template <>
Sage<SgVariableDeclaration>::build_scopes_t Driver<Sage>::getBuildScopes<SgVariableDeclaration>(const Sage<SgVariableDeclaration>::object_desc_t & desc) {
  Sage<SgVariableDeclaration>::build_scopes_t result;

  assert(false);
/*
  if (desc.parent == NULL) {
    assert(header_files.size() == 1);
    result.scope = (*header_files.begin())->get_globalScope();
  }
  else {
    SgClassSymbol * class_symbol = isSgClassSymbol(desc.parent);
    SgNamespaceSymbol * namespace_symbol = isSgNamespaceSymbol(desc.parent);

    assert(class_symbol != NULL xor namespace_symbol != NULL);

    if (class_symbol != NULL) {
      result.scope = ((SgClassDeclaration *)class_symbol->get_declaration()->get_definingDeclaration())->get_definition();
    }
    if (namespace_symbol != NULL) {
      assert(false);
    }
    
    assert(result.scope != NULL);
  }
*/
  return result;
}

template <>
template <>
void Driver<Sage>::createForwardDeclaration<SgVariableDeclaration>(Sage<SgVariableDeclaration>::symbol_t symbol, SgSourceFile * target_file) {
  assert(false);
}

// SgTemplateVariableDeclaration

// SgNamespaceDeclarationStatement

template <>
template <>
Sage<SgNamespaceDeclarationStatement>::build_result_t Driver<Sage>::build<SgNamespaceDeclarationStatement>(const Sage<SgNamespaceDeclarationStatement>::object_desc_t & desc) {
  Sage<SgNamespaceDeclarationStatement>::build_result_t result;

  SgScopeStatement * decl_scope = getBuildScopes<SgNamespaceDeclarationStatement>(desc);

  SgNamespaceDeclarationStatement * namespace_decl = SageBuilder::buildNamespaceDeclaration(desc.name, decl_scope);
  SageInterface::appendStatement(namespace_decl, decl_scope);

  result = decl_scope->lookup_namespace_symbol(desc.name);
  assert(result != NULL);

  parent_map.insert(std::pair<SgSymbol *, SgSymbol *>(result, desc.parent));

  return result;
}

template <>
template <>
Sage<SgNamespaceDeclarationStatement>::build_scopes_t Driver<Sage>::getBuildScopes<SgNamespaceDeclarationStatement>(const Sage<SgNamespaceDeclarationStatement>::object_desc_t & desc) {
  Sage<SgNamespaceDeclarationStatement>::build_scopes_t result = NULL;

  if (desc.parent != NULL)
    result = Sage<SgNamespaceDeclarationStatement>::getDefinition(desc.parent, NULL); // get definition for the project global scope
  else
    result = project->get_globalScopeAcrossFiles();

  assert(result != NULL);

  return result;
}

std::map<SgNamespaceSymbol *, std::map<SgSourceFile *, SgNamespaceDeclarationStatement *> > Sage<SgNamespaceDeclarationStatement>::per_file_namespace_decl;

SgNamespaceDefinitionStatement * Sage<SgNamespaceDeclarationStatement>::getDefinition(SgNamespaceSymbol * symbol, SgSourceFile * file) {
  SgNamespaceDefinitionStatement * result = NULL;

  std::map<SgNamespaceSymbol *, std::map<SgSourceFile *, SgNamespaceDeclarationStatement *> >::iterator it_per_file_namespace_decl = per_file_namespace_decl.find(symbol);
  assert(it_per_file_namespace_decl != per_file_namespace_decl.end());

  SgNamespaceDeclarationStatement * namespace_decl = NULL;

  std::map<SgSourceFile *, SgNamespaceDeclarationStatement *>::iterator it_decl_to_file = it_per_file_namespace_decl->second.find(file);
  if (it_decl_to_file != it_per_file_namespace_decl->second.end())
    namespace_decl = it_decl_to_file->second;
  else if (file != NULL) {
    it_decl_to_file = it_per_file_namespace_decl->second.find(NULL);
    assert(it_decl_to_file != it_per_file_namespace_decl->second.end());

    assert(false); // TODO build decl and defn for the requested file
  }
  else assert(false);

  assert(namespace_decl != NULL);

  result = namespace_decl->get_definition();

  assert(result != NULL);

  return result;
}

/*
void Sage<SgNamespaceDeclarationStatement>::init() {
  
}
*/
}

