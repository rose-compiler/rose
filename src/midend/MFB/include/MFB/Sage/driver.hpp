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

class API;

template <>
class Driver<Sage> {
  public:
    SgProject * project;

  private:

  // Files management

    unsigned long file_id_counter;

    std::map<unsigned long, std::pair<SgSourceFile *, SgSourceFile *> > file_pair_map;

    std::map<unsigned long, SgSourceFile *> standalone_source_file_map;

    std::map<SgSourceFile *, unsigned long> file_to_id_map;

    std::map<SgSourceFile *, std::set<unsigned long> > file_id_to_accessible_file_id_map;

  // Symbols management

    std::map<SgSymbol *, unsigned long> p_symbol_to_file_id_map;

    std::set<SgSymbol *> p_valid_symbols;

    std::map<SgSymbol *, SgSymbol *> p_parent_map;

    std::set<SgNamespaceSymbol *>      p_namespace_symbols;
    std::set<SgFunctionSymbol *>       p_function_symbols;
    std::set<SgClassSymbol *>          p_class_symbols;
    std::set<SgVariableSymbol *>       p_variable_symbols;
    std::set<SgMemberFunctionSymbol *> p_member_function_symbols;

  private:
    void addIncludeDirectives(SgSourceFile * target_file, unsigned long to_be_included_file_id);

    template <typename Object>
    void createForwardDeclaration(typename Sage<Object>::symbol_t symbol, SgSourceFile * target_file);

    template <typename Object>
    void loadSymbolsFromPair(unsigned long file_id, SgSourceFile * header_file, SgSourceFile * source_file);

    template <typename Symbol>
    bool resolveValidParent(Symbol * symbol);
    
    void addPointerToTopParentDeclaration(SgSymbol * symbol, SgSourceFile * file);

  public:
    Driver(SgProject * project_ = NULL);

    unsigned long createPairOfFiles(const std::string & name);
    unsigned long   loadPairOfFiles(const std::string & name, const std::string & header_path, const std::string & source_path);

    unsigned long createStandaloneSourceFile(const std::string & name, std::string suffix = "cpp");
//  unsigned long   loadStandaloneSourceFile(const std::string & name, std::string suffix = "cpp");

    unsigned long addPairOfFiles(SgSourceFile * header_file, SgSourceFile * source_file);
    unsigned long addStandaloneSourceFile(SgSourceFile * source_file);

    void addExternalHeader(unsigned long file_id, std::string header_name, bool is_system_header = true);

    API * getAPI(unsigned long file_id) const;
    API * getAPI(const std::set<unsigned long> & file_ids) const;

    template <typename Object>
    typename Sage<Object>::symbol_t useSymbol(typename Sage<Object>::symbol_t symbol, SgSourceFile * file, bool need_forward_only = false);

    template <typename Object>
    typename Sage<Object>::symbol_t useSymbol(typename Sage<Object>::symbol_t symbol, unsigned long file_id, bool needed_in_header = false, bool need_forward_only = false);

    template <typename Object>
    typename Sage<Object>::symbol_t useSymbol(typename Sage<Object>::symbol_t symbol, SgScopeStatement * scope, bool need_forward_only = false);

    template <typename Object>
    typename Sage<Object>::build_result_t build(const typename Sage<Object>::object_desc_t & desc);

  protected:
    template <typename Object>
    typename Sage<Object>::build_scopes_t getBuildScopes(const typename Sage<Object>::object_desc_t & desc);
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

