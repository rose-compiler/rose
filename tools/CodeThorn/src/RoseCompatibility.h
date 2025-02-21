#ifndef ROSE_MAPPING
#define ROSE_MAPPING 1

// minimal header for mapping rose onto the class hierarchy analysis

#include <sage3basic.h>

//~ #include <VariableIdMapping.h>

#include <string>
#include <functional>

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


class ClassAnalysis;
class CastAnalysis;

template <class T>
using Optional = boost::optional<T>;

using CallDataBase = std::tuple< Optional<FunctionKeyType>,
                                 ExpressionKeyType,
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


/// a compatibility layer that abstracts functions and queries of
///   the AST and implements those capabilities for ROSE.
class RoseCompatibilityBridge
{
  public:
    enum ReturnTypeRelation : int
    {
      unrelated = 0,
      sametype  = 1,
      covariant = 2
    };

    RoseCompatibilityBridge()  = default;
    ~RoseCompatibilityBridge() = default;

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

    /// returns a unique numeric number for id
    std::intptr_t numericId(AnyKeyType id) const;

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
    ///    The comparison skips over typedef aliases and handles array to pointer decay.
    int compareTypes(FunctionKeyType lhs, FunctionKeyType rhs, bool exclReturnType = true) const;

    /// tests if \p drvId has the same or covariant return type with respect to \p basId
    /// \param classes the extracted class analysis
    /// \param basId   ID of a function in a base class
    /// \param drvId   ID of a function in a derived class
    /// \details
    ///    The function only checks the return type, and does not check whether the classes in which
    ///    the functions \p drvId and \p basId are defined are in a inheritance relationship.
    ReturnTypeRelation
    haveSameOrCovariantReturn(const ClassAnalysis&, FunctionKeyType basId, FunctionKeyType drvId) const;

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
    FuncNameFn functionNomenclator() const;

    /// returns a function that maps a ClassKeyType to std::string
    ClassNameFn classNomenclator() const;

    /// returns a function that maps a VariableId to std::string
    VarNameFn variableNomenclator() const;

    /// returns all constructors
    /// (does not contain compiler generated c'tors)
    std::vector<FunctionKeyType>
    constructors(ClassKeyType) const;

    /// returns the constructor if it was declared
    FunctionKeyType
    destructor(ClassKeyType) const;

    /// returns true whether a function with the signature as \p fnkey
    /// can be compiler generated by \p clkey.
    bool
    isAutoGeneratable(ClassKeyType clkey, FunctionKeyType fnkey) const;

    /// returns true if the class referenced by \p clkey is abstract.
    bool
    isAbstract(ClassKeyType clkey) const;

    /// returns all callees (if known)
    /// \param fn the function for which the result shall be computed
    /// \param withFunctionAddressTaken if true, a taken function address
    ///                                 will be included in the result
    ///                                 even if it is not a call.
    /// \return a set of functions called by \p fn
    std::vector<CallData>
    functionRelations(FunctionKeyType fn) const;

    std::vector<FunctionKeyType>
    allFunctionKeys(ASTRootType n) const;

  private:
    //~ RoseCompatibilityBridge()                                          = delete;
    RoseCompatibilityBridge(RoseCompatibilityBridge&&)                 = delete;
    RoseCompatibilityBridge& operator=(RoseCompatibilityBridge&&)      = delete;
    RoseCompatibilityBridge(const RoseCompatibilityBridge&)            = delete;
    RoseCompatibilityBridge& operator=(const RoseCompatibilityBridge&) = delete;
};

/// wrapper class to produce informative debug output about casts
struct CastWriterDbg
{
    CastWriterDbg(CastKeyType n)
    : node(n)
    {}

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

/// returns the class definition for \p n.
/// \pre isSgClassDeclaration(n.get_definingDeclaration())
SgClassDefinition& getClassDef(const SgDeclarationStatement& n);

/// returns the class definition where \p n is defined
SgClassDefinition& getClassDef(const SgMemberFunctionDeclaration& n);

/// returns the class definition associated with expression \p n
/// \details
///    strips modifiers, aliases, references, and pointers
/// returns nullptr if a class definition cannot be found
SgClassDefinition* getClassDefOpt(const SgExpression& n, bool skipUpCasts = false);

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
#endif /* ROSE_MAPPING */
