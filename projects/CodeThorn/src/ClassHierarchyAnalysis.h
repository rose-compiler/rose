// An example ROSE plugin
#ifndef CLASS_HIERARCHY_ANALYSIS_H
#define CLASS_HIERARCHY_ANALYSIS_H 1

#include <vector>
#include <unordered_map>
#include <tuple>
#include <functional>
#include <iostream>

#include "RoseCompatibility.h"

namespace
{
  // auxiliary functions
  inline std::ostream& logTrace() { return std::cerr; }
  inline std::ostream& logError() { return std::cerr; }
  inline std::ostream& logInfo()  { return std::cerr; }
  inline std::ostream& logWarn()  { return std::cerr; }
}

namespace
{
  template <class Map>
  inline
  auto
  lookup(Map& m, const typename Map::key_type& key) -> decltype(*m.find(key))
  {
    auto pos = m.find(key);
    assert(pos != m.end());

    return *pos;
  }
}

namespace CodeThorn
{

std::string typeNameOfClassKeyType(ClassKeyType def);

///
struct InheritanceDesc : std::tuple<ClassKeyType, bool, bool, bool>
{
  using base = std::tuple<ClassKeyType, bool, bool, bool>;
  using base::base;

  ClassKeyType getClass()       const { return std::get<0>(*this); }
  bool         isVirtual()      const { return std::get<1>(*this); }
  bool         isDirect()       const { return std::get<2>(*this); }
  bool         throughVirtual() const { return std::get<3>(*this); }

  void setDirect(bool v)         { std::get<2>(*this) = v; }
  void setThroughVirtual(bool v) { std::get<3>(*this) = v; }
};

/// holds data a class in a program
struct ClassData
{
    using AncestorContainer   = std::vector<InheritanceDesc>;
    using DescendantContainer = std::vector<InheritanceDesc>;

    ClassData()                            = default;
    ClassData(const ClassData&)            = default;
    ClassData(ClassData&&)                 = default;
    ClassData& operator=(const ClassData&) = default;
    ClassData& operator=(ClassData&&)      = default;
    ~ClassData()                           = default;

    AncestorContainer&         parents()         { return allAncestors; }
    const AncestorContainer&   parents()  const  { return allAncestors; }
    DescendantContainer&       children()        { return allDescendants; }
    const DescendantContainer& children() const  { return allDescendants; }

    void virtualMethods(bool val) { hasVirtualMethods = val;  }
    bool virtualMethods() const   { return hasVirtualMethods; }

    void inheritsVirtualMethods(bool val) { hasInheritedVirtualMethods = val;  }
    bool inheritsVirtualMethods() const   { return hasInheritedVirtualMethods; }

  private:
    AncestorContainer   allAncestors;
    DescendantContainer allDescendants;

    bool                hasVirtualMethods          = false;
    bool                hasInheritedVirtualMethods = false;
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

  void
  addInheritanceEdge(value_type& descendant, ClassKeyType ancestorKey, bool isVirtual, bool isDirect, bool throughVirtual = false);

  void
  addInheritanceEdge(value_type& descendant, const InheritanceDesc& ancestor);

  bool
  isBaseDerived(ClassKeyType ancestorKey, ClassKeyType descendantKey) const;
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
struct CastAnalysis : std::unordered_map<CastDesc, std::vector<CastKeyType> >
{
  using base = std::unordered_map<CastDesc, std::vector<CastKeyType> >;
  using base::base;
};

/// all analyses results
using AnalysesTuple        = std::tuple<ClassAnalysis, CastAnalysis>;

/// functions type that are used for the class hierarchy traversals
/// \{
using ClassAnalysisFn      = std::function<void(ClassAnalysis::value_type&)>;
using ClassAnalysisConstFn = std::function<void(const ClassAnalysis::value_type&)>;
/// |}

/// collects all classes and casts from a project
AnalysesTuple extractFromProject(SgProject* n);

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

/// returns true, iff the class \ref clazz directly or indirectly derives
/// from a class using virtual inheritance.
bool hasVirtualInheritance(const ClassAnalysis::value_type& clazz);

/// returns true, iff the class \ref clazz or any of its base classes contain virtual functions.
bool hasVirtualFunctions(const ClassAnalysis::value_type& clazz);

/// returns true, iff the class \ref clazz requires a virtual table (vtable).
/// \details
///   a vtable is required if a class uses virtual inheritance or virtual functions.
/// \note
///   a vtable is a common but not the only way to implement the C++ object model.
bool hasVirtualTable(const ClassAnalysis::value_type& clazz);

}
#endif /* CLASS_HIERARCHY_ANALYSIS_H */
