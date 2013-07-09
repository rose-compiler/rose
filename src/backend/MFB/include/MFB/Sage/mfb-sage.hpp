
#ifndef __SCSM_SAGE_HPP__
#define __SCSM_SAGE_HPP__

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

// Model: Sage

template <typename Object>
class Sage {};

template <>
template <typename Object>
typename Sage<Object>::symbol_t Driver<Sage>::useSymbol(typename Sage<Object>::symbol_t symbol, SgSourceFile * file, bool need_forward_only);

template <>
template <typename Object>
typename Sage<Object>::symbol_t Driver<Sage>::useSymbol(typename Sage<Object>::symbol_t symbol, unsigned long file_id, bool needed_in_header, bool need_forward_only);

template <>
template <typename Object>
typename Sage<Object>::symbol_t Driver<Sage>::useSymbol(typename Sage<Object>::symbol_t symbol, SgScopeStatement * scope, bool need_forward_only);

template <>
template <typename Object>
void Driver<Sage>::createForwardDeclaration(typename Sage<Object>::symbol_t symbol, SgSourceFile * target_file);

// SgFunctionDeclaration

template <>
class Sage<SgFunctionDeclaration> {
  public:
    typedef SgScopeStatement * scope_t;
    typedef SgFunctionSymbol * symbol_t;

    struct build_scopes_t {
      SgScopeStatement * decl_scope;
      SgScopeStatement * defn_scope;
    };

    struct object_desc_t {
      std::string name;
      SgType * return_type;
      SgFunctionParameterList * params;

      SgNamespaceSymbol * parent;
      unsigned long file_id; // For declaration and definition scopes

      bool is_static;

      bool create_definition;

      object_desc_t(
        std::string name_,
        SgType * return_type_,
        SgFunctionParameterList * params_,
        SgNamespaceSymbol * parent_namespace_,
        unsigned long file_id_,
        bool is_static_ = false,
        bool create_definition_ = true
      );
    };

    struct build_result_t {
      SgFunctionSymbol * symbol;
      SgFunctionDefinition * definition;
    };
};

template <>
template <>
Sage<SgFunctionDeclaration>::build_result_t Driver<Sage>::build<SgFunctionDeclaration>(const Sage<SgFunctionDeclaration>::object_desc_t & desc);

template <>
template <>
Sage<SgFunctionDeclaration>::build_scopes_t Driver<Sage>::getBuildScopes<SgFunctionDeclaration>(const Sage<SgFunctionDeclaration>::object_desc_t & desc);

template <>
template <>
void Driver<Sage>::createForwardDeclaration<SgFunctionDeclaration>(Sage<SgFunctionDeclaration>::symbol_t symbol, SgSourceFile * target_file);

// SgMemberFunctionDeclaration

template <>
class Sage<SgMemberFunctionDeclaration> {
  public:
    typedef SgScopeStatement * scope_t;
    typedef SgMemberFunctionSymbol * symbol_t;

    struct build_scopes_t {
      SgScopeStatement * decl_scope;
      SgScopeStatement * defn_scope;
    };

    struct object_desc_t {
      std::string name;
      SgType * return_type;
      SgFunctionParameterList * params;

      SgClassSymbol * parent;
      unsigned long file_id; // only used for definition scope (declaration scope depends on parent)

      bool is_static;
      bool is_virtual;
      bool is_constructor;
      bool is_destructor;

      bool create_definition;

      object_desc_t(
        std::string name_,
        SgType * return_type_,
        SgFunctionParameterList * params_,
        SgClassSymbol * parent_class_,
        unsigned long file_id_ = 0,
        bool is_static_ = false,
        bool is_virtual_ = false,
        bool is_constructor_ = false,
        bool is_destructor_ = false,
        bool create_definition_ = true
      );
    };

    struct build_result_t {
      SgMemberFunctionSymbol * symbol;
      SgFunctionDefinition * definition;
    };
};

template <>
template <>
Sage<SgMemberFunctionDeclaration>::build_result_t Driver<Sage>::build<SgMemberFunctionDeclaration>(const Sage<SgMemberFunctionDeclaration>::object_desc_t & desc);

template <>
template <>
Sage<SgMemberFunctionDeclaration>::build_scopes_t Driver<Sage>::getBuildScopes<SgMemberFunctionDeclaration>(const Sage<SgMemberFunctionDeclaration>::object_desc_t & desc);

// SgTemplateInstantiationMemberFunctionDecl

// SgTemplateInstantiationFunctionDecl

// SgClassDeclaration

template <>
class Sage<SgClassDeclaration> {
  public:
    typedef SgScopeStatement * scope_t;
    typedef SgClassSymbol * symbol_t;

    struct build_scopes_t {
      SgScopeStatement * decl_scope;
    };

    struct object_desc_t {
      std::string name;
      unsigned long kind;

      SgSymbol * parent;
      unsigned long file_id; // non-zero

      bool create_definition;

      object_desc_t(
        std::string name_,
        unsigned long kind_,
        SgSymbol * parent_,
        unsigned long file_id_,
        bool create_definition_ = true
      );
    };

    struct build_result_t {
      SgClassSymbol * symbol;
      SgClassDefinition * definition;
    };
};

template <>
template <>
Sage<SgClassDeclaration>::build_result_t Driver<Sage>::build<SgClassDeclaration>(const Sage<SgClassDeclaration>::object_desc_t & desc);

template <>
template <>
Sage<SgClassDeclaration>::build_scopes_t Driver<Sage>::getBuildScopes<SgClassDeclaration>(const Sage<SgClassDeclaration>::object_desc_t & desc);

template <>
template <>
void Driver<Sage>::createForwardDeclaration<SgClassDeclaration>(Sage<SgClassDeclaration>::symbol_t symbol, SgSourceFile * target_file);

// SgTemplateInstantiationDecl

// SgTypedefDeclaration

// SgVariableDeclaration

template <>
class Sage<SgVariableDeclaration> {
  public:
    typedef SgScopeStatement * scope_t;
    typedef SgVariableSymbol * symbol_t;

    struct build_scopes_t {
      SgScopeStatement * scope;
    };

    struct object_desc_t {
      std::string name;
      SgType * type;
      SgInitializer * initializer;

      bool is_static;

      SgSymbol * parent;
      unsigned long file_id; // if parent is a class and it is a static: only used for definition scope (declaration scope depends on parent)

      bool create_definition; // for static field (valid if is_static && isSgClassSymbol(parent))
    };

    struct build_result_t {
      SgVariableSymbol * symbol;
      SgInitializedName * definition;
    };
};

template <>
template <>
Sage<SgVariableDeclaration>::build_result_t Driver<Sage>::build<SgVariableDeclaration>(const Sage<SgVariableDeclaration>::object_desc_t & desc);

template <>
template <>
Sage<SgVariableDeclaration>::build_scopes_t Driver<Sage>::getBuildScopes<SgVariableDeclaration>(const Sage<SgVariableDeclaration>::object_desc_t & desc);

template <>
template <>
void Driver<Sage>::createForwardDeclaration<SgVariableDeclaration>(Sage<SgVariableDeclaration>::symbol_t symbol, SgSourceFile * target_file);

// SgNamespaceDeclarationStatement

template <>
class Sage<SgNamespaceDeclarationStatement> {
  public:
    typedef SgScopeStatement * scope_t;
    typedef SgNamespaceSymbol * symbol_t;

    typedef SgScopeStatement * build_scopes_t;

    struct object_desc_t {
      std::string name;
      SgNamespaceSymbol * parent;
    };

    typedef SgNamespaceSymbol * build_result_t;

  private:
    /// namespaces are "unified" over multiple files. We instantiate a namespace declaration only when needed. 
    static std::map<SgNamespaceSymbol *, std::map<SgSourceFile *, SgNamespaceDeclarationStatement *> > per_file_namespace_decl;

  public:
    static SgNamespaceDefinitionStatement * getDefinition(SgNamespaceSymbol * symbol, SgSourceFile * file);
/*
  public:
    static void init();
*/
};

template <>
template <>
Sage<SgNamespaceDeclarationStatement>::build_result_t Driver<Sage>::build<SgNamespaceDeclarationStatement>(const Sage<SgNamespaceDeclarationStatement>::object_desc_t & desc);

template <>
template <>
Sage<SgNamespaceDeclarationStatement>::build_scopes_t Driver<Sage>::getBuildScopes<SgNamespaceDeclarationStatement>(const Sage<SgNamespaceDeclarationStatement>::object_desc_t & desc);

}

#ifndef NO_TEMPLATE_DEFINITION
#include "MFB/Sage/mfb-sage.tpp"
#endif

#endif /* __SCSM_SAGE_HPP__ */
