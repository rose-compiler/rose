/** 
 * \file MFB/include/MFB.hpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#ifndef __MFB_SAGE_DRIVER_HPP__
#define __MFB_SAGE_DRIVER_HPP__

#include "MFB/mfb.hpp"

#include <string>
#include <utility>

#include <boost/filesystem.hpp>

class SgType;

class SgFunctionDeclaration;
class SgFunctionParameterList;
class SgFunctionSymbol;
class SgFunctionDefinition;

class SgClassDeclaration;
class SgClassSymbol;
class SgClassDefinition;

class SgMemberFunctionDeclaration;
class SgMemberFunctionSymbol;

class SgNamespaceDeclarationStatement;
class SgNamespaceSymbol;
class SgNamespaceDefinitionStatement;

class SgVariableDeclaration;
class  SgVariableSymbol;
class SgInitializer;
class SgInitializedName;

namespace MFB {

/*!
 * \addtogroup grp_mfb_sage_driver
 * @{
*/

template <typename Object>
class Sage {};

class api_t;

template <>
class Driver<Sage> {
  public:
    SgProject * project;

  private:

  // Files management

    unsigned file_id_counter;
    
    std::map<boost::filesystem::path, unsigned> path_to_id_map;

    std::map<unsigned, SgSourceFile *> id_to_file_map;

    std::map<SgSourceFile *, unsigned> file_to_id_map;

    std::map<unsigned, std::set<unsigned> > file_id_to_accessible_file_id_map;

  // Symbols management

    std::map<SgSymbol *, unsigned> p_symbol_to_file_id_map;

    std::set<SgSymbol *> p_valid_symbols;

    std::map<SgSymbol *, SgSymbol *> p_parent_map;

    std::set<SgNamespaceSymbol *>      p_namespace_symbols;
    std::set<SgFunctionSymbol *>       p_function_symbols;
    std::set<SgClassSymbol *>          p_class_symbols;
    std::set<SgVariableSymbol *>       p_variable_symbols;
    std::set<SgMemberFunctionSymbol *> p_member_function_symbols;

  private:
    template <typename Object>
    void loadSymbols(unsigned file_id, SgSourceFile * file);
 
    void addIncludeDirectives(unsigned target_file_id, unsigned header_file_id);

    template <typename Object>
    void createForwardDeclaration(typename Sage<Object>::symbol_t symbol, unsigned target_id);

    /// If a symbol is imported through include directive, we need to add a pointer to its outer most associated declaration scope (not global-scope).
    void addPointerToTopParentDeclaration(SgSymbol * symbol, unsigned target_id);

    template <typename Symbol>
    bool resolveValidParent(Symbol * symbol);

    unsigned getFileID(const boost::filesystem::path & path) const;
    unsigned getFileID(SgSourceFile * source_file) const;
    unsigned getFileID(SgScopeStatement * scope) const;

  protected:
    template <typename Object>
    typename Sage<Object>::build_scopes_t getBuildScopes(const typename Sage<Object>::object_desc_t & desc);

  public:
    Driver(SgProject * project_ = NULL);
    virtual ~Driver();

    /// Create or load a file
    unsigned add(const boost::filesystem::path & path);

    /// Set a file to be unparsed with the project (by default file added to the driver are *NOT* unparsed)
    void setUnparsedFile(unsigned file_id) const;

    /// Build API of one file
    api_t * getAPI(unsigned file_id) const;

    /// Build API of a collection of files
    api_t * getAPI(const std::set<unsigned> & file_ids) const;

    /// Build API for all loaded files
    api_t * getAPI() const;

    /// Add needed include statement or forward definition to use a symbol in a file (from file ID)
    template <typename Object>
    typename Sage<Object>::symbol_t useSymbol(typename Sage<Object>::symbol_t symbol, unsigned file_id, bool need_forward_only = false);

    /// Add needed include statement or forward definition to use a symbol in a file (from SgSourceFile pointer)
    template <typename Object>
    typename Sage<Object>::symbol_t useSymbol(typename Sage<Object>::symbol_t symbol, SgSourceFile * file, bool need_forward_only = false) {
      return useSymbol(symbol, getFileID(file), need_forward_only);
    }

    /// Add needed include statement or forward definition to use a symbol in a file (from SgScopeStatement pointer)
    template <typename Object>
    typename Sage<Object>::symbol_t useSymbol(typename Sage<Object>::symbol_t symbol, SgScopeStatement * scope, bool need_forward_only = false) {
      return useSymbol(symbol, getFileID(scope), need_forward_only);
    }

    /// Build an object and add associated symbol to driver
    template <typename Object>
    typename Sage<Object>::build_result_t build(const typename Sage<Object>::object_desc_t & desc);

    /// Import external header for a given file
    void addExternalHeader(unsigned file_id, std::string header_name, bool is_system_header = true);

    /// Add a string at the begining of the file
    void attachArbitraryText(unsigned file_id, std::string str);
};

// SgTemplateInstantiationMemberFunctionDecl

// SgTemplateInstantiationFunctionDecl

// SgTemplateInstantiationDecl

// SgTypedefDeclaration

/** @} */

}

#ifndef NO_TEMPLATE_DEFINITION
#include "MFB/Sage/driver.tpp"
#endif

#endif /* __MFB_SAGE_DRIVER_HPP__ */

