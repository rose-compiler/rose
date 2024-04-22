#ifndef OBJECT_LAYOUT_ANALYSIS_H
#define OBJECT_LAYOUT_ANALYSIS_H 1

#include "RoseCompatibility.h"

// with C++ 17
// #include <variant>
#include <boost/variant.hpp>

#include "ClassHierarchyAnalysis.h"

namespace CodeThorn
{

struct Subobject
{
  ClassKeyType ref;
  bool         isVirtual;
  bool         isDirect;
};

struct Field
{
  CodeThorn::VariableKeyType id;
};

struct VTable
{
  ClassKeyType ref;
  bool         isPrimary;
};


// in C++17 use STL
// using ObjectLayoutElement = std::variant<Subobject, Field>;
using ObjectLayoutElement = boost::variant<Subobject, Field, VTable>;

using ObjectLayoutEntryBase = std::tuple<size_t, ObjectLayoutElement>;

struct ObjectLayoutEntry : ObjectLayoutEntryBase
{
  using base = ObjectLayoutEntryBase;
  using base::base;

  size_t                     offset()  const { return std::get<0>(*this); }
  const ObjectLayoutElement& element() const { return std::get<1>(*this); }
};


class ObjectLayout : private std::vector<ObjectLayoutEntry>
{
  public:
    using base = std::vector<ObjectLayoutEntry>;
    using base::base;

    using base::value_type;
    using base::iterator;
    using base::const_iterator;
    using base::begin;
    using base::end;
    using base::emplace_back;

    /// returns true, iff this is an abstract class
    /// \note this information is carried over from ClassData
    /// \{
    bool abstractClass()          const { return hasAbstractMethods; }
    void abstractClass(bool bval)       { hasAbstractMethods = bval; }
    /// \}
    private:
      bool hasAbstractMethods = false;
};

using ObjectLayoutContainer = std::unordered_map<ClassKeyType, ObjectLayout>;

/// computes a sample object layout table for classes in \ref all
/// \param all                   the result of the class hierarchy analysis
/// \param onlyClassesWithVTable if true, only classes the layout is only computed for classes with a vtable
/// \details
///    The generated object layout assumes an object model that is vtable based.
///    The generated layout is similar but not exactly the same as the IA64 Itanium object model.
ObjectLayoutContainer
computeObjectLayouts(const ClassAnalysis& all, bool onlyClassesWithVTable = true);


/// \note part of the IA64 object model, currently not computed
struct VirtualCallOffset : std::tuple<std::ptrdiff_t, ClassKeyType>
{
  using base = std::tuple<std::ptrdiff_t, ClassKeyType>;
  using base::base;

  std::tuple_element<0, base>::type
  offset() const { return std::get<0>(*this); }

  std::tuple_element<1, base>::type
  assoicatedClass() const { return std::get<1>(*this); }
};

/// \note part of the IA64 object model, currently not computed
struct VirtualBaseOffset : std::tuple<std::ptrdiff_t, ClassKeyType>
{
  using base = std::tuple<std::ptrdiff_t, ClassKeyType>;
  using base::base;

  std::tuple_element<0, base>::type
  offset() const { return std::get<0>(*this); }

  std::tuple_element<1, base>::type
  associatedClass() const { return std::get<1>(*this); }
};

/// \brief the offset to the top of the object (e.g., used for casts to void*)
///        returns the number of v-table entries, between the sub-object and
///        the address of the primary vtable.
struct OffsetToTop : std::tuple<std::ptrdiff_t>
{
  using base = std::tuple<std::ptrdiff_t>;
  using base::base;

  /// A return value of 0 indicates the primary object,
  /// a return value of 1 indicates a secondary object.
  std::tuple_element<0, base>::type
  offset() const { return std::get<0>(*this); }
};

/// \brief pointer to RTTI
struct TypeInfoPointer : std::tuple<ClassKeyType>
{
  using base = std::tuple<ClassKeyType>;
  using base::base;

  std::tuple_element<0, base>::type
  classtype() const { return std::get<0>(*this); }
};


/// \brief virtual function pointer
struct VirtualFunctionEntry : std::tuple<FunctionKeyType, ClassKeyType, bool, bool, bool>
{
  using base = std::tuple<FunctionKeyType, ClassKeyType, bool, bool, bool>;
  using base::base;

  /// returns the function representation
  std::tuple_element<0, base>::type function() const { return std::get<0>(*this); }

  /// returns the class where this function is (will be) defined
  /// \details if needsCompilerGeneration is set, the class indicates a compiler
  ///          generated function that has not been generated in the AST.
  ///          In this case function() returns an overriden function in a base class.
  std::tuple_element<1, base>::type getClass() const { return std::get<1>(*this); }
  void setClass(std::tuple_element<1, base>::type val) { std::get<1>(*this) = val; }

  /// returns if the returned object requires a this pointer adjustment compared
  ///   to the original entry in the vtable.
  /// \todo seem comment in OverriderDesc
  std::tuple_element<2, base>::type adjustReturnObj() const { return std::get<2>(*this); }

  /// returns true if this is a pure virtual function, making the
  /// the class pure virtual.
  std::tuple_element<3, base>::type isPureVirtual() const { return std::get<3>(*this); }

  /// returns true if this is a pure virtual function, making the
  /// the class pure virtual.
  std::tuple_element<4, base>::type needsCompilerGeneration() const { return std::get<4>(*this); }
  void needsCompilerGeneration(std::tuple_element<4, base>::type val) { std::get<4>(*this) = val; }
};


using VTableLayoutElement = boost::variant< VirtualCallOffset,
                                            VirtualBaseOffset,
                                            OffsetToTop,
                                            TypeInfoPointer,
                                            VirtualFunctionEntry
                                          >;

/*
struct VTableSection
{
  ClassKeyType ref;
  size_t       numInherited;
  size_t       numTotal;
  bool         isVirtual;
};
*/

struct VTableSection : std::tuple<ClassKeyType, std::uint32_t, std::uint32_t, std::uint32_t, bool>
{
  using base = std::tuple<ClassKeyType, std::uint32_t, std::uint32_t, std::uint32_t, bool>;
  using base::base;

  std::tuple_element<0, base>::type associatedClass() const { return std::get<0>(*this); }
  std::tuple_element<1, base>::type numInherited()    const { return std::get<1>(*this); }
  std::tuple_element<2, base>::type numTotal()        const { return std::get<2>(*this); }
  std::tuple_element<3, base>::type startOffset()     const { return std::get<3>(*this); }
  std::tuple_element<4, base>::type virtualBase()     const { return std::get<4>(*this); }

  void associatedClass(std::tuple_element<0, base>::type val) { std::get<0>(*this) = val; }
  void numInherited(std::tuple_element<1, base>::type val)    { std::get<1>(*this) = val; }
  void numTotal(std::tuple_element<2, base>::type val)        { std::get<2>(*this) = val; }
  void startOffset(std::tuple_element<3, base>::type val)     { std::get<3>(*this) = val; }
  void virtualBase(std::tuple_element<4, base>::type val)     { std::get<4>(*this) = val; }
};


class VTableLayout : private std::vector<VTableLayoutElement>
{
  public:
    using base = std::vector<VTableLayoutElement>;
    // using base::base;

    using VTableSections = std::vector<VTableSection>;

    explicit
    VTableLayout(ClassKeyType cl)
    : base(), clazz(cl), sections(), isAbstract(false)
    {}

    VTableLayout() = delete;

    using base::value_type;
    using base::iterator;
    using base::const_iterator;
    using base::begin;
    using base::insert;
    using base::end;
    //~ using base::emplace_back;
    using base::push_back;

          VTableSections& vtableSections()       { return sections; }
    const VTableSections& vtableSections() const { return sections; }

    const VTableSection& virtualBaseSection(ClassKeyType) const;

    VTableSection& createVTableSection()
    {
      sections.emplace_back(nullptr, 0, 0, size(), false);
      return sections.back();
    }

    // returns the vtable base address (the index where a vtable would point at)
    size_t vtableAddress() const;

    ClassKeyType getClass() const { return clazz; }

    bool   isAbstractClass() const { return isAbstract; }
    void   isAbstractClass(bool v) { isAbstract = v; }

  private:
    ClassKeyType   clazz;
    VTableSections sections;
    bool           isAbstract;

};


using VTableLayoutContainer = std::unordered_map<ClassKeyType, VTableLayout>;

/// computes a sample post-object construction virtual function table for classes in \ref all
/// \param all the result of the class hierarchy analysis
/// \param vfa result of virtual function analysis
/// \details
///    The generated layout is similar but not exactly the same as the IA64 Itanium object model.
///    virtual function tables for constructors are different, and their layout generation unsupported at this time.
VTableLayoutContainer
computeVTableLayouts(const ClassAnalysis& all, const VirtualFunctionAnalysis& vfa);

VTableLayoutContainer
computeVTableLayouts(const ClassAnalysis& all, const VirtualFunctionAnalysis& vfa, const RoseCompatibilityBridge& rcb);

}

#endif
