#ifndef ROSE_COMPATIBILTY_BRIDGE
#define ROSE_COMPATIBILTY_BRIDGE 1

// minimal header for mapping rose onto the class hierarchy analysis

#include <sage3basic.h>

//~ #include <VariableIdMapping.h>

#include <string>
#include <functional>
#include <boost/optional/optional.hpp>
#include <boost/utility/string_view.hpp>

namespace CodeThorn
{

constexpr
unsigned char STRIP_MODIFIER_ALIAS = SgType::STRIP_MODIFIER_TYPE | SgType::STRIP_TYPEDEF_TYPE;

using AnyKeyType            = const SgNode*;
using ClassKeyType          = const SgClassDefinition*;
using FunctionTypeKeyType   = const SgFunctionType*;
using TypeKeyType           = const SgType*;
using CastKeyType           = const SgCastExp*;
using VariableKeyType       = const SgInitializedName*;
// using MemberFunctionKeyType = const SgMemberFunctionDeclaration*;
using FunctionKeyType       = const SgFunctionDeclaration*;
using ExpressionKeyType     = const SgExpression*;
using ASTRootType           = SgProject*;

/// type of a class naming function
using ClassNameFn   = std::function<std::string(ClassKeyType)>;

/// type of a function naming function
using FuncNameFn    = std::function<std::string(FunctionKeyType)>;

/// type of a variable naming function
using VarNameFn     = std::function<std::string(VariableKeyType)>;

/// a predicate type for functions
using FunctionPredicate = std::function<bool(FunctionKeyType)>;


class ClassAnalysis;
class CastAnalysis;

template <class T>
using Optional = boost::optional<T>;

using StringView = boost::string_view;

using CallDataBase = std::tuple< Optional<FunctionKeyType>,
                                 ExpressionKeyType, // \todo may need optional since it is not present in ctor inits
                                 Optional<ClassKeyType>,
                                 ExpressionKeyType,
                                 bool
                               >;
struct CallData : CallDataBase
{
  using base = CallDataBase;
  using base::base;

  /// if the call can be resolved, the return value contains a valid FunctionKeyType
  Optional<FunctionKeyType> callee()      const { return std::get<0>(*this); }
  ExpressionKeyType         calleeExpr()  const { return std::get<1>(*this); }
  Optional<ClassKeyType>    typeBound()   const { return std::get<2>(*this); }
  ExpressionKeyType         call()        const { return std::get<3>(*this); }
  bool                      virtualCall() const { return std::get<4>(*this); }
};



using DataMemberTypeBase = std::tuple<ClassKeyType, bool, bool, bool, bool>;
struct DataMemberType : DataMemberTypeBase
{
  using base = DataMemberTypeBase;
  using base::base;

  ClassKeyType classType()      const { return std::get<0>(*this); }
  bool         isConst()        const { return std::get<1>(*this); }
  bool         hasReference()   const { return std::get<2>(*this); }
  bool         hasArray()       const { return std::get<3>(*this); }
  bool         hasInitializer() const { return std::get<4>(*this); }
};

using SourceLocationBase = std::tuple<StringView, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t>;
struct SourceLocation : SourceLocationBase
{
  using base = SourceLocationBase;
  using base::base;

  StringView    file()      const { return std::get<0>(*this); }
  std::uint32_t startLine() const { return std::get<1>(*this); }
  std::uint32_t startCol()  const { return std::get<2>(*this); }
  std::uint32_t limitLine() const { return std::get<3>(*this); }
  std::uint32_t limitCol()  const { return std::get<4>(*this); }
};


using SpecialMemberFunctionBase = std::tuple<FunctionKeyType, std::int8_t, bool, bool, bool>;
struct SpecialMemberFunction : SpecialMemberFunctionBase
{
    // \todo consider folding the 8bit kind and booleans into a 16bit kind

    using base = SpecialMemberFunctionBase;
    using base::base;

    enum Kind : std::uint8_t
    {
      notspecial = 0,
      ctor       = (1 << 0),
      dctor      = ctor + (1 << 1),
      cctor      = ctor + (1 << 2),
      mctor      = ctor + (1 << 3),
      dtor       = (1 << 4),
      cassign    = (1 << 5),
      massign    = (1 << 6)
    };

    FunctionKeyType
    function() const { return std::get<0>(*this); }

    std::int8_t
    kind() const { return std::get<1>(*this); }

    bool isSpecialFunc() const;
    bool isConstructor() const;
    bool isDefaultCtor() const;
    bool isCopyCtor()    const;
    bool isMoveCtor()    const;
    bool isDestructor()  const;
    bool isCopyAssign()  const;
    bool isMoveAssign()  const;

    /// returns true if the member function is compiler generated (as opposed to user defined).
    /// \note
    ///    if the member function is compiler generated it can still be declared
    ///    (meaning that function() returns a valid ID).
    ///    With the current EDG-ROSE connection this happens when an object
    ///    calls one of the generated special member functions, so ROSE will
    ///    find an implementation in the class scope though it will not have
    ///    an entry in the class member list.
    bool
    compilerGenerated() const { return std::get<2>(*this); }

    /// returns  if function is callable with a const reference argument (const T&).
    /// \note    ONLY VALID for copy constructor and copy assignment.
    /// \details when no user defined copy functionality (in the form of constructor
    ///          or assignment operator) is present the compiler attempts to
    ///          generate a version taking a const reference argument (const T&).
    ///          However, if a base class or data member offers copy functionality
    ///          for reference arguments (T&) only, the compiler will fall back to
    ///          generating a version with a reference argument.
    bool
    copyCallableWithConstRefArgument() const { return std::get<3>(*this); }

    /// returns true if a special member function has a standard conforming signature.
    /// \note
    ///    currently the only case where this returns false is the following:
    ///    T& operator=(const T) is accepted by most compilers (clang, gcc, EDG)
    ///    as a valid copy-assignment operator, but the standard says that it is not.
    ///    C++14 12.8.17:
    ///      "A user-declared copy assignment operator X::operator= is
    ///       a non-static non-template member function of class X with
    ///       exactly one parameter of type X, X&, const X&, volatile X&
    ///       or const volatile X&."
    bool
    standardConforming() const { return std::get<4>(*this); }
};

using SpecialMemberFunctionContainer = std::vector<SpecialMemberFunction>;


/// a compatibility layer that abstracts functions and queries of
///   the AST and implements those capabilities for ROSE.
class RoseCompatibilityBridge
{
  public:
    enum TypeRelation : int
    {
      unrelated = 0,
      sametype  = 1,
      covariant = 2
    };

    RoseCompatibilityBridge()                                          = default;
    RoseCompatibilityBridge(const RoseCompatibilityBridge&)            = default;
    RoseCompatibilityBridge& operator=(const RoseCompatibilityBridge&) = default;
    RoseCompatibilityBridge(RoseCompatibilityBridge&&)                 = default;
    RoseCompatibilityBridge& operator=(RoseCompatibilityBridge&&)      = default;
    ~RoseCompatibilityBridge()                                         = default;

    /// returns the variable-id for a variable or parameter \p var
    /// \{
    VariableKeyType variableId(SgInitializedName* var) const;
    VariableKeyType variableId(SgVariableDeclaration* var) const;
    /// \}

    /// returns the function-id for a (member) function \p fun
    FunctionKeyType functionId(const SgFunctionDeclaration* fun) const;

    /// returns a string representation for \p vid
    std::string nameOf(VariableKeyType vid) const;

    /// returns a string representation for \p vid
    std::string nameOf(FunctionKeyType fid) const;

    /// returns a unique numeric number for \p id
    /// \details
    ///    the ID is valid from within a program run; it is not stable across runs
    std::intptr_t numericId(AnyKeyType id) const;

    /// returns a unique string representing the object represented by \p id
    /// \throw  a runtime exception if no name can be generated for
    ///         a specific object. For example, CastKeyType and
    ///         ExpressionKeyType currently do not support
    ///         uniqueNames.
    /// \details
    ///    the unique string is valid and stable across runs
    std::string uniqueName(AnyKeyType id) const;

    /// returns the location of function implementation \p fid if available
    ///   otherwise returns the location of the first non-defining declaration.
    SourceLocation location(FunctionKeyType fid) const;

    /// compares the name of functions \p lhs and \p rhs
    /// \param lhs some function
    /// \param rhs some function
    /// \returns 0 iff lhs and rhs have the same name (wrt virtual function overriding)
    ///          1 iff lhs > rhs
    ///          -1 iff lhs < rhs
    int compareNames(FunctionKeyType lhs, FunctionKeyType rhs) const;

    /// compares the types of \p lhs and \p rhs
    /// \param lhs some function type
    /// \param rhs some function type
    /// \param exclReturnType if true the return types of \p lhs and \p rhs
    ///        are not considered (though the return types of potential arguments
    ///        and return types are). This can be useful for obtaining equality of
    ///        functions that have covariant return types.
    /// \returns 0 iff lhs and rhs have the same type
    ///          1 iff lhs > rhs
    ///          -1 iff lhs < rhs
    /// \details
    ///    The comparison skips over typedef aliases and handles array to pointer decay.
    int compareFunctionTypes(FunctionTypeKeyType lhs, FunctionTypeKeyType rhs, bool exclReturnType = true) const;

    /// compares the types of \p lhs and \p rhs
    /// \param lhs some function
    /// \param rhs some function
    /// \param exclReturnType if true the return types of \p lhs and \p rhs
    ///        are not considered (though the return types of potential arguments
    ///        and return types are). This can be useful for obtaining equality of
    ///        functions that have covariant return types.
    /// \returns 0 iff lhs and rhs have the same type
    ///          1 iff lhs > rhs
    ///          -1 iff lhs < rhs
    /// \details
    ///    The comparison skips over typedef aliases and handles array-pointer equivalence in parameter lists.
    int compareFunctionsByType(FunctionKeyType lhs, FunctionKeyType rhs, bool exclReturnType = true) const;

    /// compares the types \p lhs and \p rhs
    /// \param lhs some type
    /// \param rhs some type
    /// \returns 0 iff lhs and rhs resolve to the same type
    ///          1 iff lhs > rhs
    ///          -1 iff lhs < rhs
    int compareTypes(TypeKeyType lhs, TypeKeyType rhs) const;


    /// tests if \p drvId has the same or covariant return type with respect to \p basId
    /// \param classes the extracted class analysis
    /// \param basId   ID of a function in a base class
    /// \param drvId   ID of a function in a derived class
    /// \details
    ///    The function only checks the return type, and does not check whether the classes in which
    ///    the functions \p drvId and \p basId are defined are in a inheritance relationship.
    TypeRelation
    haveSameOrCovariantReturn(const ClassAnalysis&, FunctionKeyType basId, FunctionKeyType drvId) const;

    /// tests if \p drvId have the same or covariant return type with respect to \p basId
    /// \param classes the extracted class analysis
    /// \param basId   typed ID of the base
    /// \param drvId   typed ID of the derived
    TypeRelation
    areSameOrCovariant(const ClassAnalysis&, TypeKeyType basId, TypeKeyType drvId) const;


    /// extracts class and cast information under the root
    void extractFromProject(ClassAnalysis&, CastAnalysis&, ASTRootType) const;

    /// extracts class information from the memory pool
    void extractFromMemoryPool(ClassAnalysis&) const;

    /// extracts all classes and base classes of \p n and stores
    ///   the extracted data in \p n.
    void extractClassAndBaseClasses(ClassAnalysis& all, ClassKeyType n) const;

    /// returns iff \p id is a pure virtual member function
    bool isPureVirtual(FunctionKeyType id) const;

    /// returns iff a definition for \p id exists in the AST
    bool hasDefinition(FunctionKeyType id) const;

    /// returns a function that maps a FunctionKeyType to std::string
    FuncNameFn functionNaming() const;

    /// returns a function that maps a ClassKeyType to std::string
    ClassNameFn classNaming() const;

    /// returns a function that maps a VariableId to std::string
    VarNameFn variableNaming() const;

    /// returns true whether a function with the signature as \p fnkey
    /// can be compiler generated in class \p clkey.
    /// \note if fnkey is already in clkey, isAutoGeneratable returns falls
    ///       because fnkey is already in the class.
    bool
    isAutoGeneratable(const ClassAnalysis& all, ClassKeyType clkey, FunctionKeyType fnkey) const;


    /// returns true if the class referenced by \p clkey is abstract.
    bool
    isAbstract(ClassKeyType clkey) const;


    /// returns all callees (if known)
    /// \param fn the function for which the result shall be computed
    /// \param isVirtualFunction a predicate that returns true if
    ///                          a function is explciticely or implcitely
    ///                          virtual.
    /// \return a set of functions called by \p fn
    /// \{
    std::vector<CallData>
    functionRelations(FunctionKeyType fn, FunctionPredicate isVirtualFunction) const;
    ///}

    /// returns a predicate testing if a function has name \p name.
    FunctionPredicate
    functionNamePredicate(std::string name) const;

    /// returns all functions reachable from n
    std::vector<FunctionKeyType>
    allFunctionKeys(ASTRootType n) const;

    /// returns all functions reachable from n where pred holds.
    std::vector<FunctionKeyType>
    allFunctionKeys(ASTRootType n, FunctionPredicate pred) const;

    DataMemberType
    typeOf(VariableKeyType var) const;

    SpecialMemberFunctionContainer
    specialMemberFunctions(ClassKeyType clazz) const;

    Optional<ClassKeyType>
    classType(FunctionKeyType fn) const;
};

/// wrapper class to produce informative debug output about casts
struct CastWriterDbg
{
    CastKeyType node;
};

std::ostream& operator<<(std::ostream& os, const CastWriterDbg&);

/// returns a string representation of \p key
std::string typeNameOf(ClassKeyType key);

/// tests if \p key has a templated ancestor
/// \param  key the class key (SgClassDefinition*)
/// \return a string representation of the templated ancestor (if found)
Optional<std::string>
missingDiagnostics(ClassKeyType key);

/// produces a string with additional debug output
std::string dbgInfo(ClassKeyType key, std::size_t numParents = 3);


/// calls the callback function \p fn(derived, base, isvirtual)
///   for all direct base classes of \p clkey
void inheritanceEdges( ClassKeyType clkey,
                       std::function<void(ClassKeyType, ClassKeyType, bool)> fn
                     );
/// returns the class/base-type information for a given type \p tykey
/// \details
///   - if the type \p tykey does not refer to a class ClassKeyType{}
///     is returned.
///   - base type refers to a type after aliases, references, pointers, decltype, and modifiers.
std::pair<ClassKeyType, TypeKeyType> getClassCastInfo(TypeKeyType tykey);

//
// ROSE utilities
// \todo move to sageGeneric/sageInterface

struct IsNotNull
{
  template <class TPtr>
  bool operator()(TPtr ptr) const { return ptr != nullptr; }
};


/// returns the class definition for \p n.
/// \pre isSgClassDeclaration(n.get_definingDeclaration())
SgClassDefinition& getClassDef(const SgDeclarationStatement& n);

/// returns the class definition where \p n is defined
SgClassDefinition& getClassDefForFunction(const SgMemberFunctionDeclaration& n);

/// returns the class definition of \p n
/// returns nullptr if a class definition cannot be found
SgClassDefinition* getClassDefOpt(const SgClassType& n);

/// returns the associated class definitions of \p n
/// \details
///   strips arrays, modifiers, and typedefs.
///   DOES NOT strip references and pointers.
/// returns nullptr if a class definition cannot be found
SgClassDefinition* getClassDefOpt(const SgInitializedName& n);

/// returns the representative declaration for \p memfn
SgFunctionDeclaration& keyDecl(SgFunctionDeclaration& fn);

}
#endif /* ROSE_COMPATIBILTY_BRIDGE */
