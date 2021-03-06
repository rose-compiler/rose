#ifndef ROSE_MAPPING
#define ROSE_MAPPING 1

// minimal header for mapping rose onto the class hierarchy analysis

#include <sage3basic.h>

#include <VariableIdMapping.h>
#include <FunctionId.h>

#include <string>
#include <functional>

namespace CodeThorn
{

using ClassKeyType = const SgClassDefinition*;
using TypeKeyType  = const SgType*;
using CastKeyType  = const SgCastExp*;
using ASTRootType  = SgProject*;

/// returns the name for a class key
using ClassNameFn   = std::function<std::string(ClassKeyType)>;

/// returns the name for a class key
using FuncNameFn    = std::function<std::string(FunctionId)>;


struct ClassAnalysis;
struct CastAnalysis;

/// a compatibility layer that abstracts functions and queries of
///   the AST and implements those capabilities for ROSE.
struct RoseCompatibilityBridge
{
    enum ReturnTypeRelation : int
    {
      unrelated = 0,
      sametype  = 1,
      covariant = 2
    };

    explicit
    RoseCompatibilityBridge(VariableIdMapping& varIdMap, const FunctionIdMapping& funIdMap)
    : varMap(varIdMap), funMap(funIdMap)
    {}

    ~RoseCompatibilityBridge() = default;

    /// returns the variable-id for a variable or parameter \ref var
    /// \{
    VariableId variableId(SgInitializedName* var) const;
    VariableId variableId(SgVariableDeclaration* var) const;
    /// \}

    /// returns the function-id for a (member) function \ref fun
    FunctionId functionId(const SgFunctionDeclaration* fun) const;

    /// returns a string representation for \ref vid
    std::string nameOf(VariableId vid) const;

    /// returns a string representation for \ref vid
    std::string nameOf(FunctionId fid) const;

    /// compares the name of \ref lhs and \ref rhs
    /// \param lhs some function
    /// \param rhs some function
    /// \returns 0 iff lhs and rhs have the same name (wrt virtual function overriding)
    ///          1 iff lhs > rhs
    ///          -1 iff lhs < rhs
    int compareNames(FunctionId lhs, FunctionId rhs) const;

    /// compares the types of \ref lhs and \ref rhs
    /// \param lhs some function
    /// \param rhs some function
    /// \param exclReturnType if true the return types of \ref lhs and \ref rhs
    ///        are not considered (though the return types of potential arguments
    ///        and return types are). This can be useful for obtaining equality of
    ///        functions that have covariant return types.
    /// \returns 0 iff lhs and rhs have the same type
    ///          1 iff lhs > rhs
    ///          -1 iff lhs < rhs
    /// \details
    ///    The comparison skips over typedef aliases and handles array to pointer decay.
    int compareTypes(FunctionId lhs, FunctionId rhs, bool exclReturnType = true) const;

    /// tests if \ref drvId has the same or covariant return type with respect to \ref basId
    /// \param classes the extracted class analysis
    /// \param basId   ID of a function in a base class
    /// \param drvId   ID of a function in a derived class
    /// \details
    ///    The function only checks the return type, and does not check whether the classes in which
    ///    the functions \ref drvId and \ref basId are defined are in a inheritance relationship.
    ReturnTypeRelation
    haveSameOrCovariantReturn(const ClassAnalysis&, FunctionId basId, FunctionId drvId) const;

    /// extracts class and cast information under the root
    void extractFromProject(ClassAnalysis&, CastAnalysis&, ASTRootType) const;

    /// returns iff \ref id is a pure virtual member function
    bool isPureVirtual(FunctionId id) const;

    /// returns a function that maps a FunctionId to std::string
    FuncNameFn functionNomenclator() const;

    /// returns a function that maps a ClassKeyType to std::string
    ClassNameFn classNomenclator() const;

  private:
    RoseCompatibilityBridge()                                          = delete;
    RoseCompatibilityBridge(RoseCompatibilityBridge&&)                 = delete;
    RoseCompatibilityBridge& operator=(RoseCompatibilityBridge&&)      = delete;
    RoseCompatibilityBridge(const RoseCompatibilityBridge&)            = delete;
    RoseCompatibilityBridge& operator=(const RoseCompatibilityBridge&) = delete;

    VariableIdMapping& varMap;
    const FunctionIdMapping& funMap;
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

/// returns a string representation of \ref key
std::string typeNameOf(ClassKeyType key);


/// calls the callback function \ref fn(derived, base, isvirtual)
///   for all direct base calsses of \ref clkey
void inheritanceEdges( ClassKeyType clkey,
                       std::function<void(ClassKeyType, ClassKeyType, bool)> fn
                     );
/// returns the class/base-type information for a given type \ref tykey
/// \details
///   - if the type \ref tykey does not refer to a class ClassKeyType{}
///     is returned.
///   - base type refers to a type after aliases, references, pointers, decltype, and modifiers.
std::pair<ClassKeyType, TypeKeyType> getClassCastInfo(TypeKeyType tykey);

}
#endif /* ROSE_MAPPING */
