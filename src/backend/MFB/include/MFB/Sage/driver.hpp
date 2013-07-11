
#ifndef __MFB_SAGE_DRIVER_HPP__
#define __MFB_SAGE_DRIVER_HPP__

#include "MFB/multi-file-builder.hpp"

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

namespace MultiFileBuilder {

template <typename Object>
class Sage {};

template <>
class Driver<Sage> {
  private:
    unsigned long file_id_counter;

    std::map<unsigned long, std::string> id_to_name_map;

    std::map<unsigned long, std::pair<SgSourceFile *, SgSourceFile *> > file_pair_map;
    std::map<std::string, unsigned long> file_pair_name_map;

    std::map<unsigned long, SgSourceFile *> standalone_source_file_map;
    std::map<std::string, unsigned long> standalone_source_file_name_map;

    std::map<SgSourceFile *, unsigned long> file_to_id_map;

    std::map<SgSourceFile *, std::set<unsigned long> > file_id_to_accessible_file_id_map;

    std::map<SgSymbol *, unsigned long> symbol_to_file_id_map;

    std::map<SgSymbol *, SgSymbol *> parent_map;

  private:
    void addIncludeDirectives(SgSourceFile * target_file, unsigned long to_be_included_file_id);

    template <typename Object>
    void createForwardDeclaration(typename Sage<Object>::symbol_t symbol, SgSourceFile * target_file);

  public:
    SgProject * project;

  public:
    Driver(SgProject * project_ = NULL);

    unsigned long createPairOfFiles(const std::string & name);

    unsigned long createStandaloneSourceFile(const std::string & name, std::string suffix = "cpp");
    unsigned long addStandaloneSourceFile(SgSourceFile * source_file);

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

}

#ifndef NO_TEMPLATE_DEFINITION
#include "MFB/Sage/driver.tpp"
#endif

#endif /* __MFB_SAGE_DRIVER_HPP__ */
