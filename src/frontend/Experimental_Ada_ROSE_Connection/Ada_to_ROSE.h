
#ifndef _ADA_TO_ROSE
#define _ADA_TO_ROSE 1

#include <map>
#include <unordered_map>
#include <vector>
#include <utility>
#include <sstream>

#include <boost/algorithm/string/case_conv.hpp>

#include "Rose/Diagnostics.h"
#include "sageGeneric.h"

#include "Ada_to_ROSE_translation.h"

#define ADA_ASSERT(COND)      (sg::report_error_if(!(COND), "assertion failed: ", __FILE__, __LINE__))

namespace Ada_ROSE_Translation
{

static constexpr bool LOG_FLAW_AS_ERROR = false;

//
// debugging

//~ struct AdaDbgTraversalExit {};

//
// logging

extern Sawyer::Message::Facility mlog;


//
// entry point to conversion routine

/// converts all nodes reachable through the units in \ref head_nodes to ROSE
/// \param head_nodes  entry point to ASIS
/// \param file        the ROSE root for the translation unit
void convertAsisToROSE(_Nodes_Struct& head_nodes, SgSourceFile* file);

/// initialize translation settins
// void initialize(const Rose::Cmdline::Ada::CmdlineSettings& settings);

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

/// returns a map that collects inherited function symbols for
struct InheritedSymbolKey : std::tuple<const SgFunctionDeclaration*, const SgNamedType*>
{
  using base = std::tuple<const SgFunctionDeclaration*, const SgNamedType*>;
  using base::base;

  std::tuple_element<0, base>::type function()        const { return std::get<0>(*this); }
  std::tuple_element<1, base>::type associatedType()  const { return std::get<1>(*this); }
};


struct OperatorKey : std::tuple<const SgScopeStatement*, AdaIdentifier>
{
  using base = std::tuple<const SgScopeStatement*, AdaIdentifier>;
  using base::base;

        std::tuple_element<0, base>::type  scope() const { return std::get<0>(*this); }
  const std::tuple_element<1, base>::type& name()  const { return std::get<1>(*this); }
};

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

}

// define specializations of std::hash for user defined types
//   as recommended by the C++11 standard.
// needed if std::unordered_map is used instead of std::map to define map_t.
namespace std
{
  template <>
  struct hash<::Ada_ROSE_Translation::AdaIdentifier>
  {
    std::size_t operator()(const ::Ada_ROSE_Translation::AdaIdentifier& el) const
    {
      return std::hash<::Ada_ROSE_Translation::AdaIdentifier::base>()(el);
    }
  };

/*
  template <>
  struct hash<::Ada_ROSE_Translation::InheritedSymbolKey>
  {
    std::size_t operator()(const ::Ada_ROSE_Translation::InheritedSymbolKey& el) const
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
  struct hash<::Ada_ROSE_Translation::OperatorKey>
  {
    std::size_t operator()(const ::Ada_ROSE_Translation::OperatorKey& el) const
    {
      static constexpr std::uint8_t lshift = 7;
      static constexpr std::uint8_t rshift = (sizeof(std::size_t) * CHAR_BIT) - lshift;

      std::size_t val = std::hash<const void*>()(el.scope());

      return ( ((val << lshift) + (val >> rshift))
             ^ std::hash<::Ada_ROSE_Translation::AdaIdentifier>()(el.name())
             );
    }
  };
}


namespace Ada_ROSE_Translation
{

/// returns a mapping from Unit_ID to constructed root node in AST
//~ map_t<int, SgDeclarationStatement*>& asisUnits();

/// returns a mapping from Element_ID to SgInitializedName
map_t<int, SgInitializedName*>& asisVars();

/// returns a mapping from Element_ID to Exception declaration
/// \todo revise with representation of Exceptions
map_t<int, SgInitializedName*>& asisExcps();

/// returns a mapping from Declaration_ID to SgDeclarationStatement
map_t<int, SgDeclarationStatement*>& asisDecls();

/// returns a mapping from Element_ID to ROSE type declaration
map_t<int, SgDeclarationStatement*>& asisTypes();

/// returns a mapping from Element_ID to blocks
map_t<int, SgBasicBlock*>& asisBlocks();

//
// the following functions provide access to elements that are
// defined with the standard package, which is currently not
// provided by Asis.

/// returns a mapping from string to standard type nodes
map_t<AdaIdentifier, SgType*>& adaTypes();

/// returns a mapping from string to builtin exception types
map_t<AdaIdentifier, SgInitializedName*>& adaExcps();

/// returns a mapping from string to builtin exception types
map_t<AdaIdentifier, SgAdaPackageSpecDecl*>& adaPkgs();

/// returns a mapping from string to builtin function declarations
// map_t<AdaIdentifier, std::vector<SgFunctionDeclaration*> >& adaFuncs();

/// returns a mapping from string to variables
/// \note currently used to support the obsolescent ASCII package,
///       as long as there is no proper Character Type.
map_t<AdaIdentifier, SgInitializedName*>& adaVars();

std::map<InheritedSymbolKey, SgAdaInheritedFunctionSymbol*>& inheritedSymbols();

/// returns a map with all functions that a type supports
/// \details
///   maps stores information about explicitly or implicitly defined operators on a principal type.
///     a type may have multiple operators with the same name (e.g., "&"(string, char), "&"(string, string))
map_t<OperatorKey, std::vector<OperatorDesc> >& operatorSupport();

/// stores all expressions that were generated were operator declarations were expected
std::vector<SgExpression*>& operatorExprs();

/// deferred unit completions are lambda functions that run at the end of
///   a unit, when all unit-level declarations have been created.
/// \note
///    used for translating aspects; The Ada spec defers the elaboration of
///    aspects to brake circularity.
///    http://www.ada-auth.org/standards/12rat/html/Rat12-2-2.html .
void storeUnitCompletion(std::function<void()> completion);


//
// auxiliary functions and types

/// \brief resolves all goto statements to labels
///        at the end of procedures or functions.
struct LabelAndLoopManager
{
    LabelAndLoopManager() = default;

    /// patch gotos with target (a label statement)
    ///   at the end of a procudure / function.
    ~LabelAndLoopManager();

    /// records a new labeled statement \ref lblstmt with key \ref id.
    void label(Element_ID id, SgLabelStatement& lblstmt);

    /// records a new goto statement \ref gotostmt with label key \ref id.
    void gotojmp(Element_ID id, SgGotoStatement& gotostmt);

    /// returns a mapping from an Element_ID to a loop statement
    map_t<int, SgStatement*>& asisLoops() { return loops; }

  private:
    typedef std::map<Element_ID, SgLabelStatement*>               LabelContainer;
    typedef std::vector<std::pair<SgGotoStatement*, Element_ID> > GotoContainer;
    typedef map_t<int, SgStatement*>                              LoopMap;

    LabelContainer labels;
    GotoContainer  gotos;
    LoopMap        loops;

    LabelAndLoopManager(const LabelAndLoopManager&)            = delete;
    LabelAndLoopManager(LabelAndLoopManager&&)                 = delete;
    LabelAndLoopManager& operator=(const LabelAndLoopManager&) = delete;
    LabelAndLoopManager& operator=(LabelAndLoopManager&&)      = delete;
};


struct ExtendedPragmaID : std::tuple<Element_ID, SgStatement*>
{
  using base = std::tuple<Element_ID, SgStatement*>;

  ExtendedPragmaID(Element_ID id, SgStatement* s = nullptr)
  : base(id, s)
  {}

  Element_ID   id()   const { return std::get<0>(*this); }
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
    AstContext labelsAndLoops(LabelAndLoopManager& lm) const;

    /// returns the current label manager
    LabelAndLoopManager& labelsAndLoops() const { return SG_DEREF(all_labels_loops); }

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
    LabelAndLoopManager*         all_labels_loops        = nullptr;
    const std::string*           unit_file_name          = nullptr;
    SgAdaGenericInstanceDecl*    enclosing_instantiation = nullptr;
    PragmaContainer*             all_pragmas             = nullptr;
    DeferredCompletionContainer* unit_completions        = nullptr;
    StatementHandler             stmtHandler             = defaultStatementHandler;
    //~ Element_Struct*      elem;
};

/// returns true if an assertion failure should be triggered,
///   or if some fallback processing should continue
bool FAIL_ON_ERROR(AstContext ctx);



/// functor to create elements that are added to the current scope
struct ElemCreator
{
    explicit
    ElemCreator(AstContext astctx, bool privateItems = false)
    : ctx(astctx), privateElems(privateItems)
    {}

    void operator()(_Element_Struct& elem);

  private:
    AstContext ctx;
    bool       privateElems;

    ElemCreator() = delete;
};

/// attaches the source location information from \ref elem to
///   the AST node \ref n.
/// \note If an expression has decayed to a located node, the operator position will not be set.
/// @{
void attachSourceLocation(SgLocatedNode& n, _Element_Struct& elem, AstContext ctx);
void attachSourceLocation(SgExpression& n, _Element_Struct& elem, AstContext ctx);
void attachSourceLocation(SgPragma& n, _Element_Struct& elem, AstContext ctx);
/// @}

/// computes a nodes source location from its children
///   e.g., a try stmt includes try block and handlers
///         a try block ranges from first to last statement
///         a handler list ranges from first handler to last handler
/// \{
void computeSourceRangeFromChildren(SgLocatedNode&);
void computeSourceRangeFromChildren(SgLocatedNode*);
/// \}

/// computes a source location for the entire subtree
void computeSourceRangeForSubtree(SgNode&);

/// tests if node \ref n has a valid source location
bool hasValidSourceLocation(const SgLocatedNode&);


/// logs that an asis element kind \ref kind has been explored
/// \param kind a C-string naming the Asis kind
/// \param elemID the Asis ID associated with this element
void logKind(const char* kind, int elemID = -1);


/// tests if \ref s starts with \ref sub
/// \param  s    a string
/// \param  sub  a potential substring of s
/// \return true if \ref s starts with \ref sub
bool startsWith(const std::string& s, const std::string& sub);


/// A range abstraction for a contiguous sequence
template <class T>
struct Range : std::pair<T, T>
{
  Range(T lhs, T rhs)
  : std::pair<T, T>(lhs, rhs)
  {}

  Range() : Range(T{}, T{}) {}

  bool empty() const { return this->first == this->second; }
  int  size()  const { return this->second - this->first; }
};

/// A range of Asis Units
/*
struct UnitIdRange : Range<Unit_ID_Ptr>
{
  using base = Range<Unit_ID_Ptr>;
  using value_type = Unit_Struct;

  using base::base;
};
*/

/// A range of Asis Elements
struct ElemIdRange : Range<Element_ID_Ptr>
{
  using base = Range<Element_ID_Ptr>;
  using value_type = _Element_Struct;

  using base::base;
};


/// non-tracing alternative
//~ static inline
//~ void logKind(const char*, bool = false) {}

/// anonymous namespace for auxiliary templates and functions
namespace
{
  /// upcasts an object of type Derived to an object of type Base
  /// \note useful mainly in the context of overloaded functions
  template <class Base, class Derived>
  inline
  Base& as(Derived& obj)
  {
    Base& res = obj;

    return res;
  }

  //
  // loggers

#ifndef USE_SIMPLE_STD_LOGGER

  inline
  auto logTrace() -> decltype(Ada_ROSE_Translation::mlog[Sawyer::Message::TRACE])
  {
    return Ada_ROSE_Translation::mlog[Sawyer::Message::TRACE];
  }

  inline
  auto logInfo() -> decltype(Ada_ROSE_Translation::mlog[Sawyer::Message::INFO])
  {
    return Ada_ROSE_Translation::mlog[Sawyer::Message::INFO];
  }

  inline
  auto logWarn() -> decltype(Ada_ROSE_Translation::mlog[Sawyer::Message::WARN])
  {
    return Ada_ROSE_Translation::mlog[Sawyer::Message::WARN];
  }

  inline
  auto logError() -> decltype(Ada_ROSE_Translation::mlog[Sawyer::Message::ERROR])
  {
    return Ada_ROSE_Translation::mlog[Sawyer::Message::ERROR];
  }

  inline
  auto logFatal() -> decltype(Ada_ROSE_Translation::mlog[Sawyer::Message::FATAL])
  {
    return Ada_ROSE_Translation::mlog[Sawyer::Message::FATAL];
  }

#else /* USE_SIMPLE_STD_LOGGER */

  inline
  std::ostream& logTrace()
  {
    return std::cerr << "[TRACE] ";
  }

  inline
  std::ostream logInfo()
  {
    return std::cerr << "[INFO] ";
  }

  inline
  std::ostream& logWarn()
  {
    return std::cerr << "[WARN] ";
  }

  inline
  std::ostream& logError()
  {
    return std::cerr << "[ERROR] ";
  }

  inline
  std::ostream& logFatal()
  {
    return std::cerr << "[FATAL] ";
  }

  void logInit() {}
#endif /* USE_SIMPLE_STD_LOGGER */

  inline
  auto logFlaw() -> decltype(Ada_ROSE_Translation::mlog[Sawyer::Message::ERROR])
  {
    decltype(Ada_ROSE_Translation::mlog[Sawyer::Message::ERROR]) res = (LOG_FLAW_AS_ERROR ? logError() : logWarn());

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

  /// retrieves a node from map \ref m with key \ref key.
  template <class MapT>
  inline
  auto
  lookupNode(const MapT& m, typename MapT::key_type key) -> decltype(*m.at(key))
  {
    return *m.at(key);
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


  //
  // retrieve from ASIS map

  /// retrieves data from the big Asis map
  /// returns a nullptr if the element is not in the map.
  inline
  _Element_Struct*
  retrieveElemOpt(const ASIS_element_id_to_ASIS_MapType& map, int key)
  {
    ADA_ASSERT(key >= 0); // fails on invalid elements

    //~ logInfo() << "key: " << key << std::endl;
    auto pos = map.find(key);

    return pos != map.end() ? pos->second : nullptr;
  }

  /// retrieves data from the big Asis map
  inline
  _Element_Struct&
  retrieveElem(const ASIS_element_id_to_ASIS_MapType& map, int key)
  {
    ADA_ASSERT(key != 0); // fails on optional elements

    return SG_DEREF(retrieveElemOpt(map, key));
  }

  /// Type mapping for range element types
  /*
  template <class T>
  struct range_types {};

  template <>
  struct range_types<Element_ID_List>
  {
    using type = ElemIdRange;
  };

  template <>
  struct range_types<Unit_ID_Array_Struct>
  {
    using type = UnitIdRange;
  };
  */

  /// traverses an Asis linked list and calls \ref functor
  ///   for each element in the range [\ref first, \ref limit).
  ///   e.g., functor(*first)
  /// \returns a copy of the functor
  template <class P, class FnT>
  inline
  FnT traverse(P* first, P* limit, FnT functor)
  {
    while (first != limit)
    {
      functor(*first);

      first = first->Next;
    }

    return functor;
  }


  /// traverses a list of pointers to Elements (or Units) in the range [\ref first, \ref limit),
  ///   looks up the associated Asis struct, and passes it as argument to \ref func.
  ///   e.g., func(*retrieveElem(map, *first))
  /// \tparam ElemT    the type of the element (Unit or Element)
  /// \tparam PtrT     pointer to elements
  /// \tparam AsisMapT the map type
  /// \tparam FnT      the functor
  /// \returns a copy of \ref func after all elements have been traversed
  /// \todo split the map into two, one for elements, one for units, in order
  ///       to eliminate the need for casting.
  template <class ElemT, class PtrT, class AsisMapT, class FnT>
  inline
  FnT
  traverseIDs(PtrT first, PtrT limit, AsisMapT& map, FnT func)
  {
    while (first != limit)
    {
      if (ElemT* el = retrieveElemOpt(map, *first))
      {
        func(*el);
      }
      else
      {
        logWarn() << "asis-element of type " << typeid(el).name()
                  << " not available -- asismap[" << *first << "]=nullptr"
                  << std::endl;
      }

      ++first;
    }

    return func;
  }

  /// traverses all IDs in the range \ref range and calls functor with the associated
  ///   struct of each element.
  /// \returns a copy of \ref func after all elements have been traversed
  template <class AsisMapT, class FnT>
  inline
  FnT
  traverseIDs(ElemIdRange range, AsisMapT& map, FnT functor)
  {
    return traverseIDs<ElemIdRange::value_type>(range.first, range.second, map, functor);
  }

  /// creates a range for a contiguous sequence of IDs
  inline
  ElemIdRange
  idRange(_Element_ID_Array_Struct lst)
  {
    return ElemIdRange{lst.IDs, lst.IDs + lst.Length};
  }


  /// \brief tests whether the link in an Asis node is valid (set)
  ///        or incomplete.
  /// \note  function should become obsolete eventually.
  inline
  bool isInvalidId(int id) { return id == -1; }

  inline
  bool isValidId(int id) { return !isInvalidId(id); }

  /// \brief invokes fn(*lhsbeg, *rhsbeg) for N element pairs.
  ///        N = std::distance(lhsbeg, lhslim).
  /// \returns a copy of fn.
  /// \pre   *(rhsbeg+N) is a valid operation
  template <class LhsIterator, class RhsIterator, class BinaryFunction>
  BinaryFunction
  foreach_pair( LhsIterator lhsbeg, LhsIterator lhslim,
                RhsIterator rhsbeg,
                BinaryFunction fn
              )
  {
    while (lhsbeg != lhslim)
    {
      fn(*lhsbeg, *rhsbeg);
      ++lhsbeg; ++rhsbeg;
    }

    return fn;
  }
} // anonymous


}

#endif /* _ADA_TO_ROSE */
