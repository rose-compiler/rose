#ifndef Rosebud_BasicTypes_H
#define Rosebud_BasicTypes_H

#include <Sawyer/Clexer.h>

/** Rosebud is a tool to generate code for ROSE. */
namespace Rosebud {

/** A token parsed from the input file. */
using Token = Sawyer::Language::Clexer::Token;

/** A stream of tokens from the input file. */
using TokenStream = Sawyer::Language::Clexer::TokenStream;

/** How to obtain text when converting a sequence of tokens to a string. */
enum class Expand {
    NONE,                                               /**< Each token's [begin,end) individually. */
    INTER,                                              /**< From first token's begin to last token's end. */
    PRIOR                                               /**< From first token's prior to last token's end. */
};

/** When something should be done. */
enum class When {
    NEVER,                                              /**< Never do it. */
    ALWAYS,                                             /**< Always do it. */
    AUTO                                                /**< Sometimes do it. */
};

class Generator;
class Serializer;

// AST node types
namespace Ast {
class Node;
using NodePtr = std::shared_ptr<Node>;                  /**< Shared-ownership pointer to a @ref Node. */
class TokenList;
using TokenListPtr = std::shared_ptr<TokenList>;        /**< Shared-ownership pointer to a @ref TokenList. */
class ArgumentList;
using ArgumentListPtr = std::shared_ptr<ArgumentList>;  /**< Shared-ownership pointer to a @ref ArgumentList. */
class CppStack;
using CppStackPtr = std::shared_ptr<CppStack>;          /**< Shared-ownership pointer to a @ref CppStack. */
class Attribute;
using AttributePtr = std::shared_ptr<Attribute>;        /**< Shared-ownership pointer to a @ref Attribute. */
class Definition;
using DefinitionPtr = std::shared_ptr<Definition>;      /**< Shared-ownership pointer to a @ref Definition. */
class Property;
using PropertyPtr = std::shared_ptr<Property>;          /**< Shared-ownership pointer to a @ref Property. */
class Class;
using ClassPtr = std::shared_ptr<Class>;                /**< Shared-ownership pointer to a @ref Class. */
class File;
using FilePtr = std::shared_ptr<File>;                  /**< Shared-ownership pointer to a @ref File. */
class Project;
using ProjectPtr = std::shared_ptr<Project>;            /**< Shared-ownership pointer to a @ref Project. */
} // namespace

} // namespace
#endif
