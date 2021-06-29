// An example ROSE plugin
#ifndef CLASS_HIERARCHY_ANALYSIS_H
#define CLASS_HIERARCHY_ANALYSIS_H 1

#include <vector>
#include <unordered_map>
#include <tuple>
#include <functional>
#include <iostream>

#include "RoseCompatibility.h"
#include "CodeThornLib.h"
#include "FunctionId.h"


/*******
  How to use the Class Hierarchy Analysis

  sample code to set up the analyses:
  void runClassHierarchyAnalysis( CodeThorn::VariableIdMapping& varmap,
                                  const CodeThorn::FunctionIdMapping& funmap,
                                  SgProject* proj
                                )
  {
    // varmap's and funmap's lifetime must exceed the lifetime of rcb
    const RoseCompatibilityBridge     rcb{varmap, funmap};
    const ClassAnalysis               classAnalysis = analyzeClasses(rcb, proj);
    const VirtualFunctionCallAnalysis vfnAnalysis = virtualFunctionAnalysis(rcb, classAnalysis);

    ...
  }

********/


namespace
{
  // auxiliary functions
  //~ inline std::ostream& logTrace() { return std::cerr; }
  //~ inline std::ostream& logInfo()  { return std::cerr; }
  //~ inline std::ostream& logWarn()  { return std::cerr; }
  //~ inline std::ostream& logError() { return std::cerr; }

  inline
  auto logTrace() -> decltype(CodeThorn::logger[Sawyer::Message::TRACE])
  {
    return CodeThorn::logger[Sawyer::Message::TRACE];
  }

  inline
  auto logInfo() -> decltype(CodeThorn::logger[Sawyer::Message::INFO])
  {
    return CodeThorn::logger[Sawyer::Message::INFO];
  }

  inline
  auto logWarn() -> decltype(CodeThorn::logger[Sawyer::Message::WARN])
  {
    return CodeThorn::logger[Sawyer::Message::WARN];
  }

  inline
  auto logError() -> decltype(CodeThorn::logger[Sawyer::Message::ERROR])
  {
    return CodeThorn::logger[Sawyer::Message::ERROR];
  }

}


namespace CodeThorn
{

///
struct InheritanceDesc : std::tuple<ClassKeyType, bool, bool>
{
  using base = std::tuple<ClassKeyType, bool, bool>;
  using base::base;

  ClassKeyType getClass()       const { return std::get<0>(*this); }
  bool         isVirtual()      const { return std::get<1>(*this); }
  bool         isDirect()       const { return std::get<2>(*this); }

  void setDirect(bool v) { std::get<2>(*this) = v; }
};

struct OverrideDesc : std::tuple<FunctionId, bool>
{
  using base = std::tuple<FunctionId, bool>;
  using base::base;

  FunctionId functionId()      const { return std::get<0>(*this); }
  bool       covariantReturn() const { return std::get<1>(*this); }
};

using OverrideContainer = std::vector<OverrideDesc>;

struct VirtualFunctionDesc : std::tuple<ClassKeyType, bool, OverrideContainer, OverrideContainer>
{
  using base = std::tuple<ClassKeyType, bool, OverrideContainer, OverrideContainer>;

  VirtualFunctionDesc() = delete;

  ~VirtualFunctionDesc()                                     = default;
  VirtualFunctionDesc(const VirtualFunctionDesc&)            = default;
  VirtualFunctionDesc(VirtualFunctionDesc&&)                 = default;
  VirtualFunctionDesc& operator=(const VirtualFunctionDesc&) = default;
  VirtualFunctionDesc& operator=(VirtualFunctionDesc&&)      = default;

  VirtualFunctionDesc(ClassKeyType clKey, bool pure)
  : base(clKey, pure, OverrideContainer{}, OverrideContainer{})
  {}

  /// returns a the class where this function is declared
  ClassKeyType classId() const { return std::get<0>(*this); }

  //
  bool isPureVirtual() const { return std::get<1>(*this); }

  /// returns a (const) reference to all functions that are overriding this function
  /// \{
  const OverrideContainer& overriders() const { return std::get<2>(*this); }
  OverrideContainer&       overriders()       { return std::get<2>(*this); }
  /// \}

  /// returns a (const) reference to all functions that this function overrides
  /// \{
  OverrideContainer&       overridden()       { return std::get<3>(*this); }
  const OverrideContainer& overridden() const { return std::get<3>(*this); }
  /// \}

  /// returns a (const) reference to all virtual functions that this function hides
  /// \details
  ///   a function from an ancestor is hidden, if it has the same signature
  ///   but a different, non co-variant return type.
  /// \{
  //~ OverrideContainer&       shadows()       { return std::get<4>(*this); }
  //~ const OverrideContainer& shadows() const { return std::get<4>(*this); }
  /// \}
};

/// holds data a class in a program
struct ClassData
{
    using VirtualFunctionContainer = std::vector<FunctionId>;
    using DataMemberContainer      = std::vector<VariableId>;
    using AncestorContainer        = std::vector<InheritanceDesc>;
    using DescendantContainer      = std::vector<InheritanceDesc>;

    ClassData()                            = default;
    ClassData(const ClassData&)            = default;
    ClassData(ClassData&&)                 = default;
    ClassData& operator=(const ClassData&) = default;
    ClassData& operator=(ClassData&&)      = default;
    ~ClassData()                           = default;

    /// returns a (const) reference to all direct and indirect ancestors
    /// \{
    AncestorContainer&         ancestors()        { return allAncestors; }
    const AncestorContainer&   ancestors()  const { return allAncestors; }
    /// \}

    /// returns a (const) reference to all direct and indirect descendants
    /// \{
    DescendantContainer&       descendants()       { return allDescendants; }
    const DescendantContainer& descendants() const { return allDescendants; }
    /// \}

    /// returns a (const) reference to all virtual functions declared by this class
    /// \{
    VirtualFunctionContainer&       virtualFunctions()       { return allVirtualFunctions; }
    const VirtualFunctionContainer& virtualFunctions() const { return allVirtualFunctions; }
    /// \}

    /// returns a (const) reference to all data members declared by this class
    /// \{
    DataMemberContainer&       dataMembers()       { return allDataMembers; }
    const DataMemberContainer& dataMembers() const { return allDataMembers; }
    /// \}

    /// property indicating whether this class declares at least one virtual function
    bool declaresVirtualFunctions() const   { return virtualFunctions().size(); }

    /// property indicating whether this class inherits at least one virtual function
    /// \{
    void inheritsVirtualFunctions(bool val) { hasInheritedVirtualMethods = val;  }
    bool inheritsVirtualFunctions() const   { return hasInheritedVirtualMethods; }
    /// \}

    /// returns true, iff the class requires a virtual table (vtable).
    /// \details
    ///   a vtable is required if a class uses virtual inheritance or virtual functions.
    /// \note
    ///   a vtable is a common but not the only way to implement the C++ object model.
    bool hasVirtualTable() const;

    /// returns true, iff the class \ref clazz directly or indirectly derives
    /// from a class using virtual inheritance.
    bool hasVirtualInheritance() const;

    /// returns true, iff the class \ref clazz or any of its base classes contain virtual functions.
    bool hasVirtualFunctions() const;

  private:
    AncestorContainer        allAncestors;
    DescendantContainer      allDescendants;
    VirtualFunctionContainer allVirtualFunctions;
    DataMemberContainer      allDataMembers;

    bool                     hasInheritedVirtualMethods = false;
};

/// holds data about all classes in a program
struct ClassAnalysis : private std::unordered_map<ClassKeyType, ClassData>
{
  using base = std::unordered_map<ClassKeyType, ClassData>;
  using base::base;

  using base::value_type;
  using base::key_type;
  using base::begin;
  using base::end;
  using base::iterator;
  using base::const_iterator;
  using base::operator[];
  using base::at;
  using base::emplace;
  using base::find;
  using base::size;

  /// adds an inheritance edge to both classes \ref descendant and \ref ancestorKey
  /// \param descendant the entry for the descendant class
  /// \param ancestorKey the key of the ancestor class
  /// \param isVirtual indicates if the inheritance is virtual
  /// \param isDirect indicates if \ref descendant and \ref ancestorKey are child and parent
  void
  addInheritanceEdge(value_type& descendant, ClassKeyType ancestorKey, bool isVirtual, bool isDirect);

  /// adds an inheritance edge to both classes \ref descendant and \ref ancestorKey
  ///   based on the information in \ref ancestor.
  void
  addInheritanceEdge(value_type& descendant, const InheritanceDesc& ancestor);

  /// returns true, iff \ref ancestorKey is a (direct or indirect) base class
  /// of \ref descendantKey.
  /// \details
  ///   returns false when ancestorKey == descendantKey
  bool
  areBaseDerived(ClassKeyType ancestorKey, ClassKeyType descendantKey) const;
};


/// describes cast as from type to to type
struct CastDesc : std::tuple<TypeKeyType, TypeKeyType>
{
  using base = std::tuple<TypeKeyType, TypeKeyType>;
  using base::base;
};

}

namespace std
{
  template<> struct hash<CodeThorn::CastDesc>
  {
    std::size_t operator()(const CodeThorn::CastDesc& dsc) const noexcept
    {
      std::size_t h1 = std::hash<const void*>{}(std::get<0>(dsc));
      std::size_t h2 = std::hash<const void*>{}(std::get<1>(dsc));

      return h1 ^ (h2 >> 4);
    }
  };
}

namespace CodeThorn
{

/// collects casts and program locations where they occur
struct CastAnalysis : private std::unordered_map<CastDesc, std::vector<CastKeyType> >
{
  using base = std::unordered_map<CastDesc, std::vector<CastKeyType> >;
  using base::base;

  using base::value_type;
  using base::key_type;
  using base::iterator;
  using base::const_iterator;
  using base::begin;
  using base::end;
  using base::operator[];
  using base::at;
  using base::find;
  using base::emplace;
  using base::size;
};


/// stores the results of virtual function analysis by Id
struct VirtualFunctionAnalysis : private std::unordered_map<FunctionId, VirtualFunctionDesc, FunctionIdHashFunction>
{
  using base = std::unordered_map<FunctionId, VirtualFunctionDesc, FunctionIdHashFunction>;
  using base::base;

  using base::value_type;
  using base::key_type;
  using base::iterator;
  using base::const_iterator;
  using base::begin;
  using base::end;
  using base::operator[];
  using base::at;
  using base::find;
  using base::emplace;
  using base::size;
};


/// A tuple for both ClassAnalysis and CastAnalysis
struct AnalysesTuple : std::tuple<ClassAnalysis, CastAnalysis>
{
  using base = std::tuple<ClassAnalysis, CastAnalysis>;
  using base::base;

  ClassAnalysis&       classAnalysis()       { return std::get<0>(*this); }
  const ClassAnalysis& classAnalysis() const { return std::get<0>(*this); }

  CastAnalysis&       castAnalysis()       { return std::get<1>(*this); }
  const CastAnalysis& castAnalysis() const { return std::get<1>(*this); }
};

/// collects the class hierarchy and all casts from a project
AnalysesTuple analyzeClassesAndCasts(const RoseCompatibilityBridge& rcb, ASTRootType n);

/// collects the class hierarchy from a project
ClassAnalysis analyzeClasses(const RoseCompatibilityBridge& rcb, ASTRootType n);

/// functions type that are used for the class hierarchy traversals
/// \{
using ClassAnalysisFn      = std::function<void(ClassAnalysis::value_type&)>;
using ClassAnalysisConstFn = std::function<void(const ClassAnalysis::value_type&)>;
/// \}


/// computes function overriders for all virtual functions
VirtualFunctionAnalysis
virtualFunctionAnalysis(const RoseCompatibilityBridge& rcb, const ClassAnalysis& classes);

/// implements a top down traversal of the class hierarchy
/// \details
///    calls \ref fn for each class in \ref all exactly once. Guarantees that \ref fn
///    on a base class is called before \ref fn on a derived class.
/// \{
void topDownTraversal  (ClassAnalysis& all, ClassAnalysisFn fn);
void topDownTraversal  (const ClassAnalysis& all, ClassAnalysisConstFn fn);
/// \}

/// implements a bottom up traversal of the class hierarchy
/// \details
///    calls \ref fn for each class in \ref all exactly once. Guarantees that \ref fn
///    on a derived class is called before \ref fn on a base class.
/// \{
void bottomUpTraversal (ClassAnalysis& all, ClassAnalysisFn fn);
void bottomUpTraversal (const ClassAnalysis& all, ClassAnalysisConstFn fn);
/// \}

/// implements an unordered traversal of the class hierarchy
/// \details
///   \ref fn is called for each class exactly once
/// \{
void unorderedTraversal(ClassAnalysis& all, ClassAnalysisFn fn);
void unorderedTraversal(const ClassAnalysis& all, ClassAnalysisConstFn fn);
/// \}

}
#endif /* CLASS_HIERARCHY_ANALYSIS_H */
