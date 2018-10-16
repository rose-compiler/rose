
#ifndef __MFB_SAGE_DRIVER_HPP__
#define __MFB_SAGE_DRIVER_HPP__

#include "MFB/mfb.hpp"

#include <string>
#include <utility>
#include <iostream>

#include <boost/filesystem.hpp>

#ifndef VERBOSE
# define VERBOSE 0
#endif

class SgType;

class SgGlobal;

class SgFunctionDeclaration;
class SgFunctionParameterList;
class SgFunctionSymbol;
class SgFunctionDefinition;

class SgClassDeclaration;
class SgClassSymbol;
class SgClassDefinition;

class SgEnumDeclaration;
class SgEnumSymbol;

class SgMemberFunctionDeclaration;
class SgMemberFunctionSymbol;

class SgNamespaceDeclarationStatement;
class SgNamespaceSymbol;
class SgNamespaceDefinitionStatement;

class SgVariableDeclaration;
class SgVariableSymbol;
class SgInitializer;
class SgInitializedName;

class SgTypedefDeclaration;
class SgTypedefSymbol;

class SgNonrealDecl;
class SgNonrealSymbol;

namespace MFB {

typedef size_t file_id_t;

template <typename Object>
class Sage {};

class api_t;

template <>
class Driver<Sage> {
  public:
    SgProject * project;

  private:

  // Files management

    file_id_t file_id_counter;
    
    std::map<boost::filesystem::path, file_id_t> path_to_id_map;

    std::map<file_id_t, SgSourceFile *> id_to_file_map;

    std::map<SgSourceFile *, file_id_t> file_to_id_map;

    std::map<file_id_t, std::set<file_id_t> > file_id_to_accessible_file_id_map;

  // Symbols management

    std::map<SgSymbol *, file_id_t> p_symbol_to_file_id_map;

    std::set<SgSymbol *> p_valid_symbols;

    std::map<SgSymbol *, SgSymbol *> p_parent_map;

    std::set<SgNamespaceSymbol *>      p_namespace_symbols;
    std::set<SgFunctionSymbol *>       p_function_symbols;
    std::set<SgClassSymbol *>          p_class_symbols;
    std::set<SgEnumSymbol *>           p_enum_symbols;
    std::set<SgVariableSymbol *>       p_variable_symbols;
    std::set<SgMemberFunctionSymbol *> p_member_function_symbols;
    std::set<SgTypedefSymbol *>        p_typedef_symbols;
    std::set<SgNonrealSymbol *>        p_nonreal_symbols;

  // Type management
    
    std::map<SgScopeStatement *, std::set<SgType *> > p_type_scope_map;

  private:
    template <typename Object>
    void loadSymbols(file_id_t file_id, SgSourceFile * file);
 
    void addIncludeDirectives(file_id_t target_file_id, file_id_t header_file_id);

    template <typename Object>
    void createForwardDeclaration(typename Sage<Object>::symbol_t symbol, file_id_t target_id);

    /// If a symbol is imported through include directive, we need to add a pointer to its outer most associated declaration scope (not global-scope).
    void addPointerToTopParentDeclaration(SgSymbol * symbol, file_id_t target_id);

    template <typename Symbol>
    bool resolveValidParent(Symbol * symbol);

    file_id_t add(SgSourceFile * file);

  protected:
    template <typename Object>
    typename Sage<Object>::build_scopes_t getBuildScopes(const typename Sage<Object>::object_desc_t & desc);

  public:
    Driver(SgProject * project_ = NULL);
    virtual ~Driver();

    /// Create or load a file
    file_id_t create(const boost::filesystem::path & path);
    file_id_t add(const boost::filesystem::path & path);

    file_id_t getFileID(const boost::filesystem::path & path) const;
    file_id_t getFileID(SgSourceFile * source_file) const;
    file_id_t getFileID(SgScopeStatement * scope) const;
    SgGlobal * getGlobalScope(file_id_t id) const;

    file_id_t getFileID(SgSymbol * sym) const;

    /// Set a file to be unparsed with the project (by default file added to the driver are *NOT* unparsed)
    void setUnparsedFile(file_id_t file_id) const;

    /// Set a file to be compiled with the project (by default file added to the driver are *NOT* compiled)
    void setCompiledFile(file_id_t file_id) const;

    /// Build API of one file
    api_t * getAPI(file_id_t file_id) const;

    /// Build API of a collection of files
    api_t * getAPI(const std::set<file_id_t> & file_ids) const;

    /// Build API for all loaded files
    api_t * getAPI() const;

    /// Add needed include statement or forward definition to use a symbol in a file (from file ID)
    template <typename Object>
    typename Sage<Object>::symbol_t useSymbol(typename Sage<Object>::symbol_t symbol, file_id_t file_id, bool need_forward_only = false);

    /// Add needed include statement or forward definition to use a symbol in a file (from SgSourceFile pointer)
    template <typename Object>
    typename Sage<Object>::symbol_t useSymbol(typename Sage<Object>::symbol_t symbol, SgSourceFile * file, bool need_forward_only = false) {
      return useSymbol<Object>(symbol, getFileID(file), need_forward_only);
    }

    /// Add needed include statement or forward definition to use a symbol in a file (from SgScopeStatement pointer)
    template <typename Object>
    typename Sage<Object>::symbol_t useSymbol(typename Sage<Object>::symbol_t symbol, SgScopeStatement * scope, bool need_forward_only = false) {
      return useSymbol<Object>(symbol, getFileID(scope), need_forward_only);
    }

    /// Build an object and add associated symbol to driver
    template <typename Object>
    typename Sage<Object>::build_result_t build(const typename Sage<Object>::object_desc_t & desc);

    void useType(SgType * type, SgScopeStatement * scope);
    void useType(SgType * type, file_id_t file_id);

    /// Import external header for a given file
    void addExternalHeader(file_id_t file_id, std::string header_name, bool is_system_header = true);

    /// Add a pragma at the begining of the file
    void addPragmaDecl(file_id_t file_id, std::string str);
};

// SgTemplateInstantiationMemberFunctionDecl

// SgTemplateInstantiationFunctionDecl

// SgTemplateInstantiationDecl

// SgTypedefDeclaration

template <typename Object>
typename Sage<Object>::symbol_t Driver<Sage>::useSymbol(typename Sage<Object>::symbol_t symbol, file_id_t file_id, bool need_forward_only) {
  std::map<file_id_t, SgSourceFile *>::iterator it_file = id_to_file_map.find(file_id);
  assert(it_file != id_to_file_map.end());

  std::map<SgSymbol *, file_id_t>::const_iterator it_sym_decl_file_id = p_symbol_to_file_id_map.find((SgSymbol *)symbol);
  if (it_sym_decl_file_id == p_symbol_to_file_id_map.end())
    std::cerr << "[Error] (MFB::Driver<Sage>::useSymbol) Cannot find a file for " << symbol->get_name().getString() << std::endl;
  assert(it_sym_decl_file_id != p_symbol_to_file_id_map.end());

  file_id_t sym_decl_file_id = it_sym_decl_file_id->second;

#if VERBOSE
  std::cerr << "[Info] (MFB::Driver<Sage>::useSymbol) Symbol for " << symbol->get_name().getString() << " found in file #" << sym_decl_file_id << std::endl;
#endif

  bool need_include_directive = !need_forward_only; // TODO find out wether we need a include directive or if we can do with a forward declaration

  if (need_include_directive) {
    std::map<file_id_t, std::set<file_id_t> >::iterator it_accessible_file_ids = file_id_to_accessible_file_id_map.find(file_id);
    assert(it_accessible_file_ids != file_id_to_accessible_file_id_map.end());

    std::set<file_id_t> & accessible_file_ids = it_accessible_file_ids->second;
    std::set<file_id_t>::iterator accessible_file_id = accessible_file_ids.find(sym_decl_file_id);
    if (accessible_file_id == accessible_file_ids.end()) {
      addIncludeDirectives(file_id, sym_decl_file_id);
      accessible_file_ids.insert(sym_decl_file_id);
    }

//  addPointerToTopParentDeclaration((SgSymbol *)symbol, file_id);
  }
  else {
    createForwardDeclaration<Object>(symbol, file_id);
  }

  // No change of symbol
  return symbol;
}

template <typename Object>
void Driver<Sage>::createForwardDeclaration(typename Sage<Object>::symbol_t symbol, size_t target_file_id) {
  assert(false);
}

Driver<Sage> * SageDriver(bool skip_rose_builtin = true, bool edg_il_to_graphviz = true);

}

#endif /* __MFB_SAGE_DRIVER_HPP__ */

