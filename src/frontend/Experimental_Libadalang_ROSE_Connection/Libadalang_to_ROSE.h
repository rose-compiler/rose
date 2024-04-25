
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

struct ExtendedPragmaID : std::tuple<int, SgStatement*>
{
  using base = std::tuple<int, SgStatement*>;

  ExtendedPragmaID(int hash, SgStatement* s = nullptr)
  : base(hash, s)
  {}

  int   id()   const { return std::get<0>(*this); }
  SgStatement* stmt() const { return std::get<1>(*this); }
};

/// Ada identifier that can be used in maps/lookup tables
/// \brief
///   converts each identifier to a common representation (i.e., upper case)
struct AdaIdentifier : std::string
{
  using base = std::string;

  AdaIdentifier()                                = default;
  AdaIdentifier(const AdaIdentifier&)            = default;
  AdaIdentifier(AdaIdentifier&&)                 = default;
  AdaIdentifier& operator=(const AdaIdentifier&) = default;
  AdaIdentifier& operator=(AdaIdentifier&&)      = default;

  AdaIdentifier(const std::string& rep)
  : std::string(boost::to_upper_copy(rep))
  {}

  AdaIdentifier(const char* rep)
  : AdaIdentifier(std::string(rep))
  {}

  AdaIdentifier(const char* rep, int n)
  : AdaIdentifier(std::string(rep, n))
  {}
};

struct OperatorKey : std::tuple<const SgScopeStatement*, AdaIdentifier>
{
  using base = std::tuple<const SgScopeStatement*, AdaIdentifier>;
  using base::base;

        std::tuple_element<0, base>::type  scope() const { return std::get<0>(*this); }
  const std::tuple_element<1, base>::type& name()  const { return std::get<1>(*this); }
};

} //End Libadalang_ROSE_Translation

// define specializations of std::hash for user defined types
//   as recommended by the C++11 standard.
// needed if std::unordered_map is used instead of std::map to define map_t.
namespace std
{
  template <>
  struct hash<::Libadalang_ROSE_Translation::AdaIdentifier>
  {
    std::size_t operator()(const ::Libadalang_ROSE_Translation::AdaIdentifier& el) const
    {
      return std::hash<::Libadalang_ROSE_Translation::AdaIdentifier::base>()(el);
    }
  };

/*
  template <>
  struct hash<::Libadalang_ROSE_Translation::InheritedSymbolKey>
  {
    std::size_t operator()(const ::Libadalang_ROSE_Translation::InheritedSymbolKey& el) const
    {
      static constexpr std::uint8_t lshift = 7;
      static constexpr std::uint8_t rshift = (sizeof(std::size_t) * CHAR_BIT) - lshift;

      std::size_t val = std::hash<const void*>()(el.function());

      return ( ((val << lshift) + (val >> rshift))
             ^ std::hash<const void*>()(el.associatedType())
             );
    }
  };
*/

  template <>
  struct hash<::Libadalang_ROSE_Translation::OperatorKey>
  {
    std::size_t operator()(const ::Libadalang_ROSE_Translation::OperatorKey& el) const
    {
      static constexpr std::uint8_t lshift = 7;
      static constexpr std::uint8_t rshift = (sizeof(std::size_t) * CHAR_BIT) - lshift;

      std::size_t val = std::hash<const void*>()(el.scope());

      return ( ((val << lshift) + (val >> rshift))
             ^ std::hash<::Libadalang_ROSE_Translation::AdaIdentifier>()(el.name())
             );
    }
  };
}

namespace Libadalang_ROSE_Translation {

struct OperatorDesc : std::tuple<SgFunctionDeclaration*, std::uint8_t>
{
  enum { COMPILER_GENERATED = 1, DECLARED_IN_STANDARD = 2, USER_DEFINED = 3 };

  using base = std::tuple<SgFunctionDeclaration*, std::uint8_t>;
  using base::base;

  std::tuple_element<0, base>::type function() const { return std::get<0>(*this); }

  std::tuple_element<1, base>::type flags()    const { return std::get<1>(*this); }
  bool isCompilerGenerated()  const { return flags() == COMPILER_GENERATED; }
  bool isDeclaredInStandard() const { return flags() == DECLARED_IN_STANDARD; }
  bool isUserDefined()        const { return flags() == USER_DEFINED; }
};

// default map used in the translation
template <class KeyType, class SageNode>
using map_t = std::unordered_map<KeyType, SageNode>;

/// returns a mapping from hash to SgInitializedName
map_t<int, SgInitializedName*>& libadalangVars();

/// returns a mapping from hash to SgDeclarationStatement
map_t<int, SgDeclarationStatement*>& libadalangDecls();

/// returns a mapping from hash to ROSE type declaration
map_t<int, SgDeclarationStatement*>& libadalangTypes();

/// returns a mapping from string to standard type nodes
map_t<int, SgType*>& adaTypes();

/// stores all expressions that were generated where operator declarations were expected
std::vector<SgExpression*>& operatorExprs();

/// returns a map with all functions that a type supports
/// \details
///   maps stores information about explicitly or implicitly defined operators on a principal type.
///     a type may have multiple operators with the same name (e.g., "&"(string, char), "&"(string, string))
map_t<OperatorKey, std::vector<OperatorDesc> >& operatorSupport();

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


    /// Handles the lal_unit_root node
    ada_base_entity* unit_root() const { return lal_unit_root; }
    AstContext       unit_root(ada_base_entity* unit_root_lal) const;
    /// \}

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
    ada_base_entity*             lal_unit_root           = nullptr;
    //~ Element_Struct*      elem;
};

//
// debugging

//~ struct AdaDbgTraversalExit {};

//
// logging

extern Sawyer::Message::Facility mlog;


/// converts all nodes reachable through the units in \ref analysis_unit to ROSE
/// \param root           entry point to the Libadalang tree
/// \param file           the ROSE root for the translation unit
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

void logKind(const char* kind, int elemID);

void handleElement(ada_base_entity* lal_element, AstContext ctx, bool isPrivate = false);

} //End Libadalang_ROSE_Translation

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

  /// records a node (value) \ref val with key \ref key in map \ref m.
  /// \param m       the map
  /// \param key     the recorded key
  /// \param val     the new value
  /// \param replace true, if the key is already in the map, false otherwise
  ///        (this is used for consistency checks).
  /// \pre key is not in the map yet
  template <class MapT, class ValT>
  inline
  void
  recordNode(MapT& m, typename MapT::key_type key, ValT& val, bool replace = false)
  {
    //~ ADA_ASSERT(replace || m.find(key) == m.end());
    if (!(replace || m.find(key) == m.end()))
    {
      logFlaw() << "replace node " << typeid(*m[key]).name()
                << " with " << typeid(val).name()
                << std::endl;
    }

    m[key] = &val;
  }


  /// records the first mapping that appears in the translation
  /// secondary mappings are ignored, but do not trigger an error.
  /// \note use for non-defining/defining decls and
  ///       other nodes that do not have a single defining mapping.
  template <class MapT, class ValT>
  inline
  void
  recordNonUniqueNode(MapT& m, typename MapT::key_type key, ValT& val, bool replace = false)
  {
    const bool nodeExists = (m.find(key) != m.end());

    if (nodeExists && !replace)
      return;

    recordNode(m, key, val, nodeExists);
  }

  /// \private
  /// base case when a declaration is not in the map
  template <class MapT>
  inline
  typename MapT::mapped_type
  findFirst(const MapT&)
  {
    return {};
  }

  /// tries one or more keys to find a declaration from map \ref m
  /// returns the default value (e.g., nullptr) if none of the keys exist.
  template <class MapT, class Key0T, class... KeysT>
  inline
  typename MapT::mapped_type
  findFirst(const MapT& m, Key0T key0, KeysT... keys)
  {
    typename MapT::const_iterator pos = m.find(key0);

    return pos != m.end() ? pos->second : findFirst(m, keys...);
  }

}

#endif //_LIBADALANG_TO_ROSE_H
