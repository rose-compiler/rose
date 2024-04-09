
#ifndef _LIBADALANG_TO_ROSE_H
#define _LIBADALANG_TO_ROSE_H 1

#include <map>
#include <unordered_map>
#include <vector>
#include <utility>
#include <sstream>

#include <boost/algorithm/string/case_conv.hpp>

#include "Rose/Diagnostics.h"
#include "sage3basic.h"
#include "sageGeneric.h"
#include "libadalang.h"

namespace Libadalang_ROSE_Translation {


static constexpr bool LOG_FLAW_AS_ERROR = false;

std::string dot_ada_text_type_to_string(ada_text_type input_text);

std::string dot_ada_unbounded_text_type_to_string(ada_unbounded_text_type_array input_text);

std::string dot_ada_full_sloc(ada_base_entity *node);

//Function to hash a unique int from a node using the node's kind and location.
//The kind and location can be provided, but if not they will be determined in the function
int hash_node(ada_base_entity *node, int kind, std::string full_sloc);

struct ExtendedPragmaID : std::tuple<int, SgStatement*>
{
  using base = std::tuple<int, SgStatement*>;

  ExtendedPragmaID(int hash, SgStatement* s = nullptr)
  : base(hash, s)
  {}

  int   id()   const { return std::get<0>(*this); }
  SgStatement* stmt() const { return std::get<1>(*this); }
};

/// The context class for translation from Asis to ROSE
///   containts context that is passed top-down
struct AstContext
{
    using StatementHandler            = std::function<void(AstContext, SgStatement&)>;
    using PragmaContainer             = std::vector<ExtendedPragmaID>;
    using DeferredCompletion          = std::function<void()>;
    using DeferredCompletionContainer = std::vector<DeferredCompletion>;

    AstContext()                             = default;
    AstContext(AstContext&&)                 = default;
    AstContext& operator=(AstContext&&)      = default;
    AstContext(const AstContext&)            = default;
    AstContext& operator=(const AstContext&) = default;

    /// sets scope without parent check (no-parent-check)
    ///   e.g., when the parent node is built after the scope \ref s (e.g., if statements)
    /// \note the passed object needs to survive the lifetime of the returned AstContext
    //~ AstContext scope_npc(SgScopeStatement& s) const;

    /// sets scope and checks that the parent of \ref s is set properly
    /// \note the passed object needs to survive the lifetime of the returned AstContext
    AstContext scope(SgScopeStatement& s) const;

    /// returns the current scope
    SgScopeStatement& scope()  const { return SG_DEREF(the_scope); }

    /// sets a new label manager
    /// \note the passed object needs to survive the lifetime of the returned AstContext
    //AstContext labelsAndLoops(LabelAndLoopManager& lm) const;

    /// returns the current label manager
    //LabelAndLoopManager& labelsAndLoops() const { return SG_DEREF(all_labels_loops); }

    /// unit file name
    /// \note the passed object needs to survive the lifetime of the returned AstContext
    AstContext sourceFileName(std::string& file) const;

    /// returns the source file name
    /// \note the Asis source names do not always match the true source file name
    ///       e.g., loop_exit.adb contains a top level function Compute, and the Asis
    ///             nodes under Compute report Compute.adb as the source file.
    const std::string& sourceFileName() const { return SG_DEREF(unit_file_name); }

    /// instantiation property
    /// \details
    ///   Inside an instantiation, the Asis representation may be incomplete
    ///   Thus, the argument mapping needs to switch to lookup mode to find
    ///   generic arguments, if the Asis link is not present.
    /// \note the passed object needs to survive the lifetime of the returned AstContext
    /// \{
    SgAdaGenericInstanceDecl* instantiation() const { return enclosing_instantiation; }
    AstContext                instantiation(SgAdaGenericInstanceDecl& instance) const;
    /// \}

    /// pragma container property
    /// \note the passed object needs to survive the lifetime of the returned AstContext
    /// \details
    ///   collects all pragmas during body processing in a user supplied container
    /// \{
    PragmaContainer& pragmas() const { return SG_DEREF(all_pragmas); }
    AstContext       pragmas(PragmaContainer& ids) const;
    bool             collectsPragmas() const { return all_pragmas != nullptr; }
    /// \}

    /// parent node for pragma and aspect processing
    /// \details
    ///   the translator will lazily create a declaration scope when needed
    ///   to create new unbound identifiers that appear in pragmas and aspects.
    /// \{
    AstContext pragmaAspectAnchor(SgDeclarationStatement& dcl) const;
    SgDeclarationStatement* pragmaAspectAnchor() const { return pragma_aspect_anchor; };
    /// \}

    /// handles deferred unit completions (currently only aspects)
    /// \{
    AstContext deferredUnitCompletionContainer(DeferredCompletionContainer& cont) const;
    void storeDeferredUnitCompletion(DeferredCompletion completion) const;
    /// \}


    /// appends new statements to \ref blk instead of the current scope, \ref the_scope.
    AstContext unscopedBlock(SgAdaUnscopedBlock& blk) const;

    void appendStatement(SgStatement& s) const { stmtHandler(*this, s); }

    //
    // policies for building the AST depending on context

    static
    void defaultStatementHandler(AstContext, SgStatement&);

  private:
    SgDeclarationStatement*      pragma_aspect_anchor    = nullptr;
    SgScopeStatement*            the_scope               = nullptr;
    //LabelAndLoopManager*         all_labels_loops        = nullptr;
    const std::string*           unit_file_name          = nullptr;
    SgAdaGenericInstanceDecl*    enclosing_instantiation = nullptr;
    PragmaContainer*             all_pragmas             = nullptr;
    DeferredCompletionContainer* unit_completions        = nullptr;
    StatementHandler             stmtHandler             = defaultStatementHandler;
    //~ Element_Struct*      elem;
};

//
// debugging

//~ struct AdaDbgTraversalExit {};

//
// logging

extern Sawyer::Message::Facility mlog;


/// converts all nodes reachable through the units in \ref analysis_unit to ROSE
/// \param root    entry point to the Libadalang tree
/// \param file    the ROSE root for the translation unit
void convertLibadalangToROSE(ada_base_entity* root, SgSourceFile* file);


/// attaches the source location information from \ref elem to
///   the AST node \ref n.
/// \note If an expression has decayed to a located node, the operator position will not be set.
/// @{
void attachSourceLocation(SgLocatedNode& n, ada_base_entity* lal_element, AstContext ctx);
void attachSourceLocation(SgExpression& n, ada_base_entity* lal_element, AstContext ctx);
void attachSourceLocation(SgPragma& n, ada_base_entity* lal_element, AstContext ctx);
/// @}


/// tests if \ref s starts with \ref sub
/// \param  s    a string
/// \param  sub  a potential substring of s
/// \return true if \ref s starts with \ref sub
bool startsWith(const std::string& s, const std::string& sub);

void handleElement(ada_base_entity* lal_element, AstContext ctx, bool isPrivate = false);

}

namespace{
  inline
  auto logTrace() -> decltype(Libadalang_ROSE_Translation::mlog[Sawyer::Message::TRACE])
  {
    return Libadalang_ROSE_Translation::mlog[Sawyer::Message::TRACE];
  }

  inline
  auto logInfo() -> decltype(Libadalang_ROSE_Translation::mlog[Sawyer::Message::INFO])
  {
    return Libadalang_ROSE_Translation::mlog[Sawyer::Message::INFO];
  }

  inline
  auto logWarn() -> decltype(Libadalang_ROSE_Translation::mlog[Sawyer::Message::WARN])
  {
    return Libadalang_ROSE_Translation::mlog[Sawyer::Message::WARN];
  }

  inline
  auto logError() -> decltype(Libadalang_ROSE_Translation::mlog[Sawyer::Message::ERROR])
  {
    return Libadalang_ROSE_Translation::mlog[Sawyer::Message::ERROR];
  }

  inline
  auto logFatal() -> decltype(Libadalang_ROSE_Translation::mlog[Sawyer::Message::FATAL])
  {
    return Libadalang_ROSE_Translation::mlog[Sawyer::Message::FATAL];
  }

  inline
  auto logFlaw() -> decltype(Libadalang_ROSE_Translation::mlog[Sawyer::Message::ERROR])
  {
    decltype(Libadalang_ROSE_Translation::mlog[Sawyer::Message::ERROR]) res = (Libadalang_ROSE_Translation::LOG_FLAW_AS_ERROR ? logError() : logWarn());

    res << " *FLAW* ";
    return res;
  }
}

#endif //_LIBADALANG_TO_ROSE_H
